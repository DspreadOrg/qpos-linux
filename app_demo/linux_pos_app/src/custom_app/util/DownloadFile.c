/*
 * chunk_download.c  —  Download a file over HTTP / HTTPS using Range requests
 *
 * Compile:  gcc -O2 -Wall -o chunk_download chunk_download.c -lssl -lcrypto
 * Run:      ./chunk_download <URL> <output_file>
 * Example:  ./chunk_download https://example.com/big.iso ./big.iso
 *           ./chunk_download http://192.168.1.10:8080/data.bin ./data.bin
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include "appinc.h"
#include "DownloadFile.h"
/* ------------------------------------------------------------------ */
/* Configuration                                                      */
/* ------------------------------------------------------------------ */
#define CHUNK_SIZE   (1024 * 32)   /* Download 1 MiB per Range request */
#define HDR_BUF      8192
#define BODY_BUF     16384

/* ------------------------------------------------------------------ */
/* URL parsing                                                        */
/* ------------------------------------------------------------------ */
typedef struct {
    char scheme[8];          /* "http" or "https" */
    char host[256];
    int  port;
    char path[2048];
} url_t;

static int parse_url(const char *url, url_t *u)
{
    memset(u, 0, sizeof(*u));

    const char *p = strstr(url, "://");
    if (!p) {
        OsLog(LOG_DEBUG, "Invalid URL: missing scheme (http:// or https://)\n");
        return -1;
    }
    size_t slen = (size_t)(p - url);
    if (slen == 0 || slen >= sizeof(u->scheme)) return -1;
    memcpy(u->scheme, url, slen);
    u->scheme[slen] = '\0';

    if (strcmp(u->scheme, "http") != 0 && strcmp(u->scheme, "https") != 0) {
        OsLog(LOG_DEBUG, "Unsupported scheme: %s\n", u->scheme);
        return -1;
    }
    p += 3;

    const char *host_start = p, *host_end;

    if (*p == '[') {                       /* IPv6 literal [::1] */
        const char *rb = strchr(p, ']');
        if (!rb) { OsLog(LOG_DEBUG, "Invalid IPv6 address\n"); return -1; }
        host_start = p + 1;
        host_end   = rb;
        p = rb + 1;
    } else {
        host_end = p;
        while (*host_end && *host_end != ':' && *host_end != '/') host_end++;
        p = host_end;
    }

    size_t hlen = (size_t)(host_end - host_start);
    if (hlen == 0 || hlen >= sizeof(u->host)) { OsLog(LOG_DEBUG, "Empty host\n"); return -1; }
    memcpy(u->host, host_start, hlen);
    u->host[hlen] = '\0';

    u->port = (strcmp(u->scheme, "https") == 0) ? 443 : 80;

    if (*p == ':') {
        p++;
        u->port = atoi(p);
        if (u->port <= 0 || u->port > 65535) { OsLog(LOG_DEBUG, "Invalid port\n"); return -1; }
        while (*p && *p != '/') p++;
    }

    if (*p == '/')
        snprintf(u->path, sizeof(u->path), "%s", p);
    else
        snprintf(u->path, sizeof(u->path), "/");

    return 0;
}

/* ------------------------------------------------------------------ */
/* Connection abstraction (TCP or TLS)                                */
/* ------------------------------------------------------------------ */
typedef struct {
    int      fd;
    SSL     *ssl;
    SSL_CTX *ctx;
    int      is_ssl;
} conn_t;

static SSL_CTX *g_ssl_ctx = NULL;

/* 是否校验证书链 / 主机名。
 * 默认 0：不认证。设备上没有可用的 CA 库（libcrypto 编译时 OPENSSLDIR 指向了
 * 构建机目录），S3 的证书链无法验证，因此这里直接跳过验证。
 * 需要认证时改成 1，并把 CA 包放到 res/ca.pem。 */
#ifndef DOWNLOAD_TLS_VERIFY
#define DOWNLOAD_TLS_VERIFY 0
#endif

#if DOWNLOAD_TLS_VERIFY
/* 校验证书时使用的 CA 包，按顺序取第一个存在且可读的 */
static const char *g_ca_files[] = {
    "res/ca.pem",
    "./res/ca.pem",
    "/etc/ssl/certs/ca-certificates.crt",     /* Debian / Ubuntu 系 */
    "/etc/pki/tls/certs/ca-bundle.crt",       /* RedHat / CentOS 系 */
    "/etc/ssl/cert.pem",
    NULL
};

static int ssl_store_cert_count(SSL_CTX *ctx)
{
    X509_STORE *store = SSL_CTX_get_cert_store(ctx);
    STACK_OF(X509_OBJECT) *objs;
    int i, n = 0;

    if (!store) return 0;
    objs = X509_STORE_get0_objects(store);
    if (!objs) return 0;

    for (i = 0; i < sk_X509_OBJECT_num(objs); i++) {
        if (X509_OBJECT_get_type(sk_X509_OBJECT_value(objs, i)) == X509_LU_X509)
            n++;
    }
    return n;
}
#endif

static const char *ssl_err_name(int e)
{
    switch (e) {
    case SSL_ERROR_NONE:         return "NONE";
    case SSL_ERROR_SSL:          return "SSL_ERROR_SSL";
    case SSL_ERROR_WANT_READ:    return "SSL_ERROR_WANT_READ";
    case SSL_ERROR_WANT_WRITE:   return "SSL_ERROR_WANT_WRITE";
    case SSL_ERROR_SYSCALL:      return "SSL_ERROR_SYSCALL";
    case SSL_ERROR_ZERO_RETURN:  return "SSL_ERROR_ZERO_RETURN";
    default:                     return "SSL_ERROR_OTHER";
    }
}

static int ssl_init(void)
{
    if (g_ssl_ctx) return 0;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    g_ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!g_ssl_ctx) {
        OsLog(LOG_DEBUG, "SSL_CTX_new failed\n");
        return -1;
    }

    /* AWS S3 只接受 TLS1.2 及以上 */
    SSL_CTX_set_min_proto_version(g_ssl_ctx, TLS1_2_VERSION);

#if DOWNLOAD_TLS_VERIFY
    {
        int i, n_ca;

        SSL_CTX_set_default_verify_paths(g_ssl_ctx);
        n_ca = ssl_store_cert_count(g_ssl_ctx);

        for (i = 0; n_ca == 0 && g_ca_files[i]; i++) {
            if (access(g_ca_files[i], R_OK) != 0) continue;
            if (SSL_CTX_load_verify_locations(g_ssl_ctx, g_ca_files[i], NULL) == 1) {
                n_ca = ssl_store_cert_count(g_ssl_ctx);
                OsLog(LOG_DEBUG, "[TLS] loaded CA bundle %s (%d certs)\n",
                      g_ca_files[i], n_ca);
            }
        }
        if (n_ca == 0)
            OsLog(LOG_DEBUG, "[TLS] WARNING: no CA cert loaded, default dir = %s\n",
                  X509_get_default_cert_dir());
    }
    SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER, NULL);
#else
    /* 不做证书链与主机名校验：设备没有 CA、系统时间不准确时也能正常握手 */
    SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_NONE, NULL);
    OsLog(LOG_DEBUG, "[TLS] certificate verification disabled\n");
#endif

    return 0;
}

static int tcp_connect(const char *host, int port)
{
    struct addrinfo hints, *res = NULL, *rp;
    char portstr[16];
    int fd = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(portstr, sizeof(portstr), "%d", port);

    int rc = getaddrinfo(host, portstr, &hints, &res);
    if (rc != 0) {
        OsLog(LOG_DEBUG, "getaddrinfo(%s): %s\n", host, gai_strerror(rc));
        return -1;
    }

    for (rp = res; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(fd);
        fd = -1;
    }

    freeaddrinfo(res);
    if (fd < 0) OsLog(LOG_DEBUG, "Connect to %s:%d failed: %s\n", host, port, strerror(errno));
    return fd;
}

static void conn_close(conn_t *c)
{
    if (c->ssl) {
        SSL_shutdown(c->ssl);
        SSL_free(c->ssl);
        c->ssl = NULL;
    }
    if (c->fd >= 0) {
        close(c->fd);
        c->fd = -1;
    }
}

static int conn_open(conn_t *c, const url_t *u)
{
    memset(c, 0, sizeof(*c));
    c->fd = -1;

    c->fd = tcp_connect(u->host, u->port);
    if (c->fd < 0) return -1;

    /* Small optimization: disable Nagle to reduce round-trip latency */
    {
        int one = 1;
        setsockopt(c->fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    }

    if (strcmp(u->scheme, "https") == 0) {
        if (ssl_init() < 0) { conn_close(c); return -1; }

        c->ctx = g_ssl_ctx;
        c->ssl = SSL_new(g_ssl_ctx);
        if (!c->ssl) { conn_close(c); return -1; }

        SSL_set_fd(c->ssl, c->fd);
        if (SSL_set_tlsext_host_name(c->ssl, u->host) != 1)
            OsLog(LOG_DEBUG, "[TLS] SNI set failed for %s\n", u->host);
#if DOWNLOAD_TLS_VERIFY
        if (SSL_set1_host(c->ssl, u->host) != 1)
            OsLog(LOG_DEBUG, "[TLS] hostname verification set failed for %s\n", u->host);
#endif

        {
            int ret = SSL_connect(c->ssl);

            if (ret != 1) {
                int           e   = SSL_get_error(c->ssl, ret);
                long          vr  = SSL_get_verify_result(c->ssl);
                unsigned long ec;

                OsLog(LOG_DEBUG, "TLS handshake failed: %s, verify=%ld (%s)%s%s\n",
                      ssl_err_name(e), vr, X509_verify_cert_error_string(vr),
                      (e == SSL_ERROR_SYSCALL) ? ", errno=" : "",
                      (e == SSL_ERROR_SYSCALL) ? strerror(errno) : "");

                while ((ec = ERR_get_error()) != 0) {
                    char ebuf[256];
                    ERR_error_string_n(ec, ebuf, sizeof(ebuf));
                    OsLog(LOG_DEBUG, "  openssl: %s\n", ebuf);
                }
                conn_close(c);
                return -1;
            }
            OsLog(LOG_DEBUG, "[TLS] connected: %s, cipher %s\n",
                  SSL_get_version(c->ssl), SSL_get_cipher_name(c->ssl));
        }

#if DOWNLOAD_TLS_VERIFY
        {
            long vr = SSL_get_verify_result(c->ssl);
            if (vr != X509_V_OK) {
                OsLog(LOG_DEBUG, "Certificate verification failed: %s\n",
                        X509_verify_cert_error_string(vr));
                conn_close(c);
                return -1;
            }
        }
#endif
        c->is_ssl = 1;
    }
    return 0;
}

static int conn_write(conn_t *c, const void *buf, size_t len)
{
    const char *p = buf;
    size_t off = 0;

    while (off < len) {
        int n;
        if (c->is_ssl) {
            n = SSL_write(c->ssl, p + off, (int)(len - off));
            if (n <= 0) {
                int e = SSL_get_error(c->ssl, n);
                if (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE) continue;
                return -1;
            }
        } else {
            ssize_t r = write(c->fd, p + off, len - off);
            if (r < 0) {
                if (errno == EINTR) continue;
                return -1;
            }
            n = (int)r;
        }
        off += (size_t)n;
    }
    return (int)off;
}

static int conn_read(conn_t *c, void *buf, size_t len)
{
    if (c->is_ssl) {
        for (;;) {
            int n = SSL_read(c->ssl, buf, (int)len);
            if (n > 0) return n;
            int e = SSL_get_error(c->ssl, n);
            if (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE) continue;
            if (e == SSL_ERROR_ZERO_RETURN) return 0;   /* peer closed cleanly */
            return -1;
        }
    }
    for (;;) {
        ssize_t n = read(c->fd, buf, len);
        if (n < 0 && errno == EINTR) continue;
        return (int)n;
    }
}

/* ------------------------------------------------------------------ */
/* Buffered reader                                                    */
/* ------------------------------------------------------------------ */
typedef struct {
    conn_t *c;
    char    buf[BODY_BUF];
    size_t  pos, len;
} breader_t;

static int br_fill(breader_t *br)
{
    if (br->pos < br->len) return 0;
    int n = conn_read(br->c, br->buf, sizeof(br->buf));
    if (n <= 0) return -1;
    br->pos = 0;
    br->len = (size_t)n;
    return 0;
}

static int br_getc(breader_t *br)
{
    if (br->pos >= br->len && br_fill(br) < 0) return -1;
    return (unsigned char)br->buf[br->pos++];
}

static int br_read(breader_t *br, void *dst, size_t n)
{
    char  *d   = dst;
    size_t got = 0;

    while (got < n) {
        if (br->pos >= br->len && br_fill(br) < 0) return -1;
        size_t avail = br->len - br->pos;
        size_t take  = (avail < n - got) ? avail : (n - got);
        memcpy(d + got, br->buf + br->pos, take);
        br->pos += take;
        got     += take;
    }
    return 0;
}

/* Read a line ending with \n, strip trailing \r */
static int br_readline(breader_t *br, char *out, size_t cap)
{
    size_t i = 0;
    for (;;) {
        int ch = br_getc(br);
        if (ch < 0) return -1;
        if (ch == '\n') {
            if (i > 0 && out[i - 1] == '\r') i--;
            out[i] = '\0';
            return 0;
        }
        if (i + 1 < cap) out[i++] = (char)ch;
    }
}

/* ------------------------------------------------------------------ */
/* HTTP response header parsing                                       */
/* ------------------------------------------------------------------ */
typedef struct {
    int       status;
    long long content_length;      /* -1 means unknown */
    int       chunked;
    int       has_content_range;
    long long range_start;
    long long range_end;
    long long range_total;         /* -1 means unknown */
} http_resp_t;

static int read_response_head(breader_t *br, http_resp_t *r)
{
    char line[HDR_BUF];

    memset(r, 0, sizeof(*r));
    r->content_length = -1;
    r->range_total    = -1;

    if (br_readline(br, line, sizeof(line)) < 0) {
        OsLog(LOG_DEBUG, "Failed to read status line\n");
        return -1;
    }
    if (strncmp(line, "HTTP/", 5) != 0) {
        OsLog(LOG_DEBUG, "Invalid response: %s\n", line);
        return -1;
    }
    char *sp = strchr(line, ' ');
    if (!sp) return -1;
    r->status = atoi(sp + 1);

    for (;;) {
        if (br_readline(br, line, sizeof(line)) < 0) return -1;
        if (line[0] == '\0') break;            /* end of headers */

        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = '\0';

        char *name = line;
        char *val  = colon + 1;
        while (*val == ' ' || *val == '\t') val++;

        if (strcasecmp(name, "Content-Length") == 0) {
            r->content_length = strtoll(val, NULL, 10);
        } else if (strcasecmp(name, "Transfer-Encoding") == 0) {
            if (strcasestr(val, "chunked")) r->chunked = 1;
        } else if (strcasecmp(name, "Content-Range") == 0) {
            long long a, b, t;
            if (sscanf(val, "bytes %lld-%lld/%lld", &a, &b, &t) == 3) {
                r->has_content_range = 1;
                r->range_start = a;
                r->range_end   = b;
                r->range_total = t;
            } else if (sscanf(val, "bytes %lld-%lld/*", &a, &b) == 2) {
                r->has_content_range = 1;
                r->range_start = a;
                r->range_end   = b;
                r->range_total = -1;
            } else if (sscanf(val, "bytes */%lld", &t) == 1) {
                /* 416 响应：只有总长度，没有区间 */
                r->has_content_range = 1;
                r->range_start = -1;
                r->range_end   = -1;
                r->range_total = t;
            }
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Response body -> file (using standard C file operations)           */
/* ------------------------------------------------------------------ */
static int body_fixed(breader_t *br, FILE *fp, long long offset,
                      long long length, long long *written)
{
    char buf[BODY_BUF];
    long long got = 0;

    while (got < length) {
        size_t take = (size_t)((length - got) < (long long)sizeof(buf)
                               ? (length - got) : (long long)sizeof(buf));
        if (br_read(br, buf, take) < 0) return -1;

        if (fseek(fp, (long)(offset + got), SEEK_SET) != 0) {
            OsLog(LOG_DEBUG,"fseek");
            return -1;
        }
        if (fwrite(buf, 1, take, fp) != take) {
            OsLog(LOG_DEBUG,"fwrite");
            return -1;
        }
        got += (long long)take;
    }
    *written = got;
    return 0;
}

static int body_chunked(breader_t *br, FILE *fp, long long offset, long long *written)
{
    char buf[BODY_BUF];
    char line[512];
    long long total = 0;

    for (;;) {
        if (br_readline(br, line, sizeof(line)) < 0) return -1;

        long long sz = strtoll(line, NULL, 16);   /* supports chunk extensions "1a;x=y" */
        if (sz < 0) return -1;

        if (sz == 0) {                            /* read trailers until empty line */
            for (;;) {
                if (br_readline(br, line, sizeof(line)) < 0) break;
                if (line[0] == '\0') break;
            }
            break;
        }

        long long remaining = sz;
        while (remaining > 0) {
            size_t take = (size_t)((remaining < (long long)sizeof(buf))
                                   ? remaining : (long long)sizeof(buf));
            if (br_read(br, buf, take) < 0) return -1;

            if (fseek(fp, (long)(offset + total), SEEK_SET) != 0) {
                OsLog(LOG_DEBUG,"fseek");
                return -1;
            }
            if (fwrite(buf, 1, take, fp) != take) {
                OsLog(LOG_DEBUG,"fwrite");
                return -1;
            }
            total     += (long long)take;
            remaining -= (long long)take;
        }
        if (br_readline(br, line, sizeof(line)) < 0) return -1;  /* CRLF */
    }
    *written = total;
    return 0;
}

static int body_until_eof(breader_t *br, FILE *fp, long long offset, long long *written)
{
    long long total = 0;

    for (;;) {
        if (br->pos >= br->len && br_fill(br) < 0) break;   /* EOF */
        size_t avail = br->len - br->pos;

        if (fseek(fp, (long)(offset + total), SEEK_SET) != 0) {
            OsLog(LOG_DEBUG,"fseek");
            return -1;
        }
        if (fwrite(br->buf + br->pos, 1, avail, fp) != avail) {
            OsLog(LOG_DEBUG,"fwrite");
            return -1;
        }
        br->pos = br->len;
        total  += (long long)avail;
    }
    *written = total;
    return 0;
}

static int stream_body(breader_t *br, const http_resp_t *r,
                       FILE *fp, long long offset, long long *written)
{
    *written = 0;
    if (r->chunked)
        return body_chunked(br, fp, offset, written);
    if (r->content_length >= 0)
        return body_fixed(br, fp, offset, r->content_length, written);
    return body_until_eof(br, fp, offset, written);
}

/* 当前文件大小（字节），失败返回 -1 */
static long long file_size(FILE *fp)
{
    long cur  = ftell(fp);
    long size;

    if (cur < 0) return -1;
    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    size = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    return (long long)size;
}

/* 把要发送的 HTTP 请求逐行打进日志 */
static void log_request(const char *req, int len)
{
    char line[256];
    int  i, n = 0;

    OsLog(LOG_DEBUG, "[send] %d bytes:\n", len);

    for (i = 0; i < len; i++) {
        if (req[i] == '\r') continue;
        if (req[i] == '\n') {
            line[n] = '\0';
            if (n > 0) OsLog(LOG_DEBUG, "  > %s\n", line);
            n = 0;
            continue;
        }
        if (n < (int)sizeof(line) - 1) line[n++] = req[i];
    }

    if (n > 0) {
        line[n] = '\0';
        OsLog(LOG_DEBUG, "  > %s\n", line);
    }
}

/* ------------------------------------------------------------------ */
/* Issue a GET request with Range header                              */
/* ------------------------------------------------------------------ */
static int http_get_range(const url_t *u, long long start, long long end,
                          conn_t *c, breader_t *br, http_resp_t *r)
{
    if (conn_open(c, u) < 0) return -1;

    /* Host header */
    char hosthdr[320];
    int  default_port = (strcmp(u->scheme, "https") == 0) ? 443 : 80;
    int  has_colon    = (strchr(u->host, ':') != NULL);

    if (u->port == default_port)
        snprintf(hosthdr, sizeof(hosthdr), has_colon ? "[%s]" : "%s", u->host);
    else
        snprintf(hosthdr, sizeof(hosthdr), has_colon ? "[%s]:%d" : "%s:%d",
                 u->host, u->port);

    char req[HDR_BUF];
    int  n = snprintf(req, sizeof(req),
                      "GET %s HTTP/1.1\r\n"
                      "Host: %s\r\n"
                      "User-Agent: chunk-downloader/1.0\r\n"
                      "Accept: */*\r\n"
                      "Accept-Encoding: identity\r\n"
                      "Range: bytes=%lld-%lld\r\n"
                      "Connection: close\r\n"
                      "\r\n",
                      u->path, hosthdr, start, end);

    if (n < 0 || (size_t)n >= sizeof(req)) {
        OsLog(LOG_DEBUG, "Request header too long\n");
        conn_close(c);
        return -1;
    }

    log_request(req, n);

    if (conn_write(c, req, (size_t)n) != n) {
        OsLog(LOG_DEBUG, "Failed to send request\n");
        conn_close(c);
        return -1;
    }

    memset(br, 0, sizeof(*br));
    br->c = c;

    if (read_response_head(br, r) < 0) {
        conn_close(c);
        return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Main chunked download routine                                      */
/* ------------------------------------------------------------------ */
/* 进度回调包装：cb 为空时直接跳过 */
static void notify_progress(download_progress_cb cb, long long downloaded, long long total)
{
    if (cb) cb(downloaded, total);
}

static int download_file(const char *url, const char *outpath, download_progress_cb cb)
{
    url_t u;
    if (parse_url(url, &u) < 0) return -1;

    FILE *fp = fopen(outpath, "wb");
    if (!fp) {
        OsLog(LOG_DEBUG,"fopen output file");
        return -1;
    }

    long long offset    = 0;   /* bytes already downloaded */
    long long total     = -1;  /* total file size (unknown = -1) */
    int       restarted = 0;   /* 416 之后是否已经重头下过一次 */
    int       rc        = -1;

    for (;;) {
        notify_progress(cb, offset, total);

        if (total >= 0 && offset >= total) { rc = 0; break; }

        long long end = offset + CHUNK_SIZE - 1;
        if (total >= 0 && end > total - 1) end = total - 1;

        conn_t      c;
        breader_t   br;
        http_resp_t r;

        if (http_get_range(&u, offset, end, &c, &br, &r) < 0) {
            OsLog(LOG_DEBUG, "Request failed (offset=%lld)\n", offset);
            break;
        }

        OsLog(LOG_DEBUG, "[*] Range bytes=%lld-%lld -> HTTP %d\n", offset, end, r.status);

        if (r.status == 200) {
            /* 200 并不代表“整个文件”，很多服务器对 Range 请求也回 200，
             * 但内容只是从请求位置开始的一包/最后一段数据。这里按优先级判断落点：
             *   1) 带了 Content-Range       -> 按里面的区间写（等价 206）
             *   2) offset==0 或长度==total  -> 整个文件，从 0 覆盖写
             *   3) 其它                     -> 最后一包数据，写到 offset 处，绝不能覆盖前面的内容
             */
            long long clen       = r.content_length;
            long long w          = 0;
            long long write_at   = offset;
            long long expected   = -1;
            int       whole_file = 0;

            if (r.has_content_range && r.range_start >= 0) {
                write_at = r.range_start;
                if (r.range_total >= 0) total = r.range_total;
                if (r.range_end >= r.range_start)
                    expected = r.range_end - r.range_start + 1;

                OsLog(LOG_DEBUG, "[*] HTTP 200 with Content-Range: %lld-%lld/%lld\n",
                      r.range_start, r.range_end, r.range_total);
            } else if (offset == 0 || (total >= 0 && clen == total)) {
                whole_file = 1;
                write_at   = 0;
            }

            if (whole_file) {
                if (offset != 0) {
                    OsLog(LOG_DEBUG, "[!] HTTP 200 returns whole file, rewrite from 0\n");
                    fflush(fp);
                    if (ftruncate(fileno(fp), 0) != 0) {
                        OsLog(LOG_DEBUG, "[!] truncate failed: %s\n", strerror(errno));
                        conn_close(&c);
                        break;
                    }
                }
            } else {
                OsLog(LOG_DEBUG, "[*] HTTP 200 returns last packet, append at %lld\n", write_at);
            }

            if (stream_body(&br, &r, fp, write_at, &w) < 0) {
                OsLog(LOG_DEBUG, "Error receiving data\n");
                conn_close(&c);
                break;
            }
            conn_close(&c);

            offset = write_at + w;

            OsLog(LOG_DEBUG, "[*] HTTP 200 body: %lld bytes at %lld (expect %lld), total=%lld\n",
                  w, write_at, expected, total);

            if (whole_file) {
                total = (clen >= 0) ? clen : w;

                if (total >= 0 && offset < total) {
                    /* 服务器给了长度却没收到这么多：连接提前断，交给上层重试 */
                    OsLog(LOG_DEBUG, "[!] Incomplete body (%lld/%lld)\n", offset, total);
                    break;
                }
                rc = 0;
                break;
            }

            if (expected > 0 && w < expected)
                OsLog(LOG_DEBUG, "[!] Short body: %lld/%lld\n", w, expected);

            if (total < 0) {
                /* 总长度未知：把这一包当成文件结尾（服务器只回最后一包的情况） */
                total = offset;
                OsLog(LOG_DEBUG, "[*] HTTP 200 last packet accepted as EOF (%lld bytes)\n", total);
                rc = 0;
                break;
            }

            if (offset > total) {
                OsLog(LOG_DEBUG, "[!] Received more data than expected (%lld/%lld)\n",
                      offset, total);
                break;
            }

            if (offset == total) {
                rc = 0;
                break;
            }

            if (w == 0) {
                OsLog(LOG_DEBUG, "[!] No progress in this request, stopping\n");
                break;
            }

            /* 还没到文件尾，继续要下一段 */
            continue;

        } else if (r.status == 206) {
            /* Partial content */
            long long real_start = offset;
            long long expected   = -1;

            if (r.has_content_range) {
                real_start = r.range_start;
                if (r.range_total >= 0) total = r.range_total;
                if (r.range_end >= r.range_start)
                    expected = r.range_end - r.range_start + 1;
            }

            long long w = 0;
            if (stream_body(&br, &r, fp, real_start, &w) < 0) {
                OsLog(LOG_DEBUG, "Error receiving data\n");
                conn_close(&c);
                break;
            }
            conn_close(&c);

            OsLog(LOG_DEBUG, "[*] HTTP 206 body: %lld bytes (expect %lld), %lld/%lld\n",
                  w, expected, real_start + w, total);

            if (w == 0) {
                OsLog(LOG_DEBUG, "[!] No data received in this request, stopping to avoid infinite loop\n");
                break;
            }

            if (expected > 0 && w < expected)
                OsLog(LOG_DEBUG, "[!] Short range body: %lld/%lld\n", w, expected);

            offset = real_start + w;

            if (total < 0) {
                OsLog(LOG_DEBUG, "[!] Server did not return total length, cannot continue chunked download\n");
                rc = (offset > 0) ? 0 : -1;
                break;
            }

        } else if (r.status == 416) {
            /* 416 Range Not Satisfiable：请求起点已到/超过文件末尾。
             * 这里先比对“已下载长度”和 total，一致就说明已经下载完成。 */
            conn_close(&c);

            if (r.range_total >= 0)
                total = r.range_total;    /* 服务器在 Content-Range 里给出了准确总长 */

            long long local = file_size(fp);

            OsLog(LOG_DEBUG, "[*] HTTP 416: local=%lld, offset=%lld, total=%lld\n",
                  local, offset, total);

            if (total >= 0 && local == total) {
                OsLog(LOG_DEBUG, "[+] HTTP 416: downloaded %lld bytes == total, complete\n", local);
                rc = 0;
                break;
            }

            if (total >= 0 && local > total) {
                /* 本地多出残留数据，截断到 total 后同样算完整 */
                fflush(fp);
                if (ftruncate(fileno(fp), (off_t)total) == 0) {
                    OsLog(LOG_DEBUG, "[+] HTTP 416: truncated to %lld == total, complete\n", total);
                    rc = 0;
                } else {
                    OsLog(LOG_DEBUG, "[!] truncate to %lld failed: %s\n",
                          total, strerror(errno));
                }
                break;
            }

            if (total < 0) {
                /* 服务器没给总长度：本地已有数据就认为已经下完 */
                rc = (local > 0) ? 0 : -1;
                if (rc == 0)
                    OsLog(LOG_DEBUG, "[+] HTTP 416: no total, local %lld bytes accepted\n", local);
                else
                    OsLog(LOG_DEBUG, "[!] HTTP 416: no total and empty local file\n");
                break;
            }

            /* local < total：确实还没下完，服务器却拒绝该范围，只能重头再来一次 */
            if (!restarted) {
                restarted = 1;
                OsLog(LOG_DEBUG, "[!] HTTP 416 but incomplete (local=%lld, total=%lld), restart from 0\n",
                      local, total);
                fflush(fp);
                if (ftruncate(fileno(fp), 0) == 0) {
                    offset = 0;
                    continue;
                }
                OsLog(LOG_DEBUG, "[!] truncate failed: %s\n", strerror(errno));
            }
            break;

        } else {
            OsLog(LOG_DEBUG, "HTTP error: %d\n", r.status);
            conn_close(&c);
            break;
        }
    }

    /* 收尾：统一以“本地文件大小 == 远端总长”判定成功，
     * 不再依赖中间过程的 offset 计数 —— 短包、重复请求、200 最后一包等情况
     * 只要最终字节数对上就算下载成功。 */
    fflush(fp);

    long long saved = file_size(fp);

    if (total >= 0 && saved > total) {
        OsLog(LOG_DEBUG, "[!] Local file larger than total (%lld/%lld), truncating\n",
              saved, total);
        if (ftruncate(fileno(fp), (off_t)total) == 0) saved = total;
    }

    if (total >= 0) {
        if (saved == total) {
            OsLog(LOG_DEBUG, "[+] size check ok: local=%lld, total=%lld\n", saved, total);
            rc = 0;
        } else {
            OsLog(LOG_DEBUG, "[-] size check failed: local=%lld, total=%lld\n", saved, total);
            rc = -1;
        }
    } else {
        /* 服务器始终没给出总长度：只能按“是否收到数据”判断 */
        rc = (saved > 0) ? 0 : -1;
        OsLog(LOG_DEBUG, "[*] total unknown, saved %lld bytes\n", saved);
    }

    fclose(fp);

    /* 最终进度：把实际文件大小和已知总长报给上层 */
    notify_progress(cb, saved, total);

    if (rc == 0)
        OsLog(LOG_DEBUG, "[*] saved %lld bytes to %s\n", saved, outpath);

    return rc;
}

/* 带进度回调的对外接口 */
int download_file_with_progress(const char *url, const char *outpath,
                                download_progress_cb cb)
{
    return download_file(url, outpath, cb);
}

/* ------------------------------------------------------------------ */
/* 进度回调示例：按百分比打印日志 */
static void download_progress_log(long long downloaded, long long total)
{
    if (total > 0) {
        int pct = (int)((downloaded * 100) / total);

        if (pct > 100) pct = 100;
        if (pct < 0)   pct = 0;

        OsLog(LOG_DEBUG, "[progress] %lld/%lld bytes, %d%%\n", downloaded, total, pct);
    } else {
        OsLog(LOG_DEBUG, "[progress] %lld bytes, total unknown\n", downloaded);
    }
}

int download_test()
{
    /* 备用地址（HTTPS，IP 直连）：https://1.7.140.58/pos/TMS/ApkDownload/DownloadAPKZip/4957F688-50D2-4163-8D69-522DD1CB7823_ZHURNMJK2I_appfile1.0.0.2 */
    char *url = "http://dspread-tms-se-test-dspread.s3.ap-east-1.amazonaws.com/2e32bd6efbc20ef09851f132bab92062.apk";
    int rc = download_file_with_progress(url, "/tmp/file.apk", download_progress_log);

    if (rc == 0) OsLog(LOG_DEBUG, "[+] Download complete\n");
    else         OsLog(LOG_DEBUG, "[-] Download failed\n");
    return rc == 0 ? 0 : 1;
}