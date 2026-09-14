#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include "appinc.h"

/* ============================================================
 * utility function
 * ============================================================ */

static void print_openssl_error(const char *msg)
{
    OsLog(LOG_DEBUG, "[ERROR] %s\n", msg);
    ERR_print_errors_fp(stderr);
}

/* Load the public key from the PEM certificate */
static EVP_PKEY *load_pubkey_from_cert(const char *cert_path)
{
    FILE *fp = fopen(cert_path, "r");
    if (!fp) { OsLog(LOG_DEBUG,"fopen cert"); return NULL; }

    X509 *cert = PEM_read_X509(fp, NULL, NULL, NULL);
    fclose(fp);
    if (!cert) {
        print_openssl_error("PEM_read_X509 failed");
        return NULL;
    }

    EVP_PKEY *pkey = X509_get_pubkey(cert);
    X509_free(cert);
    if (!pkey) print_openssl_error("X509_get_pubkey failed");
    return pkey;
}

/* Load the public key from a PEM file (standalone public key file,
 not a certificate) */
static EVP_PKEY *load_pubkey(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) { OsLog(LOG_DEBUG,"fopen pubkey"); return NULL; }
    EVP_PKEY *pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);
    if (!pkey) print_openssl_error("PEM_read_PUBKEY failed");
    return pkey;
}

#include <openssl/ui.h>
#include <openssl/opensslv.h>

/* ============================================================
* Password callback: Used to load the encrypted private key
* If you don't want to enter it interactively, you can just return a hardcoded password
 * ============================================================ */
typedef struct {
    const char *password;   /* If it's NULL, prompt the user to enter from the terminal */
    int        len;         /* Password length. -1 means use strlen */
} pem_password_t;

static int rsa_pem_password_cb(char *buf, int size, int rwflag, void *userdata)
{
    (void)rwflag;
    pem_password_t *pw = (pem_password_t *)userdata;
    if (!pw || !pw->password) {
        /* No password information available, unable to load */
        return 0;
    }
    int len = (pw->len >= 0) ? pw->len : (int)strlen(pw->password);
    if (len > size) len = size;
    memcpy(buf, pw->password, (size_t)len);
    return len;
}

/* ============================================================
* API 1: Load private key from PEM file (password-free)
*   file: Path to the PEM private key file
*   Returns EVP_PKEY*, NULL on failure
 * ============================================================ */
static EVP_PKEY *load_privkey(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp) {
        OsLog(LOG_DEBUG,"fopen privkey");
        return NULL;
    }

    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);

    if (!pkey) {
        OsLog(LOG_DEBUG,"PEM_read_PrivateKey failed (please use load_privkey_with_password)");
    }
    return pkey;
}

/* ============================================================
* Interface 2: Loads private key from PEM file (with password, used for encrypting the private key)
*   file    : Path to the PEM private key file
*   password: Password string (NULL indicates no password)
 * ============================================================ */
static EVP_PKEY *load_privkey_with_password(const char *file,
                                            const char *password)
{
    FILE *fp = fopen(file, "r");
    if (!fp) {
        OsLog(LOG_DEBUG,"fopen privkey");
        return NULL;
    }

    pem_password_t pw = { password, -1 };
    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, NULL, rsa_pem_password_cb, &pw);
    fclose(fp);

    if (!pkey) {
        OsLog(LOG_DEBUG,"PEM_read_PrivateKey with password failed ");
    }
    return pkey;
}

/* ============================================================
* API 3: Load private key from a PEM string in memory
*   pem_data: PEM string (starting with -----BEGIN ...)
*   len: String length (0 means automatically calculate strlen)
*   password: Password (NULL means no password)
 * ============================================================ */
static EVP_PKEY *load_privkey_from_mem(const char *pem_data,
                                       size_t len,
                                       const char *password)
{
    if (!pem_data) return NULL;

    BIO *bio = BIO_new_mem_buf(pem_data, (len == 0) ? -1 : (int)len);
    if (!bio) {
        OsLog(LOG_DEBUG,"BIO_new_mem_buf failed");
        return NULL;
    }

    EVP_PKEY *pkey = NULL;
    if (password && password[0]) {
        pem_password_t pw = { password, -1 };
        pkey = PEM_read_bio_PrivateKey(bio, NULL, rsa_pem_password_cb, &pw);
    } else {
        pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    }

    BIO_free(bio);

    if (!pkey) {
        OsLog(LOG_DEBUG,"PEM_read_bio_PrivateKey failed");
    }
    return pkey;
}

/* ============================================================
* API 4: Loads private key from PEM file, compatible with PKCS#1/PKCS#8 encryption
*   Attempts passwordless login first; if that fails, tries the given password
 * ============================================================ */
static EVP_PKEY *load_privkey_auto(const char *file, const char *password)
{
    EVP_PKEY *pkey = load_privkey(file);
    if (pkey) return pkey;

    /* Clear the error queue to avoid interfering with subsequent attempts */
    ERR_clear_error();

    if (password && password[0]) {
        pkey = load_privkey_with_password(file, password);
    }
    return pkey;
}

/* Check if the PEM file is an encrypted private key
 * Returns: 1 = Encrypted, 0 = Unencrypted, -1 = Unable to determine */
static int is_pem_privkey_encrypted(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp) return -1;

    char line[256];
    int encrypted = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "ENCRYPTED")) {
            encrypted = 1;
            break;
        }
        if (strstr(line, "-----BEGIN") && strstr(line, "PRIVATE KEY-----")) {
           /* Hit an unencrypted PEM header */
        }
    }
    fclose(fp);
    return encrypted;
}

/* Get the private key type name, such as "RSA" or "EC" */
static const char *get_privkey_type(EVP_PKEY *pkey)
{
    if (!pkey) return "NULL";

    switch (EVP_PKEY_base_id(pkey)) {
        case EVP_PKEY_RSA:    return "RSA";
        case EVP_PKEY_RSA_PSS:return "RSA-PSS";
        case EVP_PKEY_DSA:    return "DSA";
        case EVP_PKEY_DH:     return "DH";
        case EVP_PKEY_EC:     return "EC";
        default:              return "Unknown";
    }
}
/* Get the number of bits in the private key */
static int get_privkey_bits(EVP_PKEY *pkey)
{
    if (!pkey) return 0;
    return EVP_PKEY_bits(pkey);
}

/* ============================================================
* RSA public key encryption
*   padding_mode: RSA_PKCS1_PADDING or RSA_PKCS1_OAEP_PADDING
 * ============================================================ */
static int rsa_encrypt(EVP_PKEY *pub_key,
                       const unsigned char *plain, size_t plain_len,
                       int padding_mode,
                       unsigned char **out, size_t *out_len)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pub_key, NULL);
    if (!ctx) { print_openssl_error("EVP_PKEY_CTX_new"); return -1; }

    int ret = -1;

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        print_openssl_error("EVP_PKEY_encrypt_init");
        goto out;
    }

    /* Set the padding mode */
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding_mode) <= 0) {
        print_openssl_error("set_rsa_padding");
        goto out;
    }

    /* The OAEP mode also requires setting the digest algorithm */
    if (padding_mode == RSA_PKCS1_OAEP_PADDING) {
        if (EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256()) <= 0) goto out;
        if (EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha256()) <= 0) goto out;
    }

    /* First call: Get the output buffer size */
    size_t buf_len = 0;
    if (EVP_PKEY_encrypt(ctx, NULL, &buf_len, plain, plain_len) <= 0) {
        print_openssl_error("EVP_PKEY_encrypt (size query)");
        goto out;
    }

    unsigned char *buf = malloc(buf_len);
    if (!buf) goto out;

    /* Second call: actual encryption */
    if (EVP_PKEY_encrypt(ctx, buf, &buf_len, plain, plain_len) <= 0) {
        print_openssl_error("EVP_PKEY_encrypt");
        free(buf);
        goto out;
    }

    *out = buf;
    *out_len = buf_len;
    ret = 0;

out:
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

/* ============================================================
* RSA private key decryption
 * ============================================================ */
static int rsa_decrypt(EVP_PKEY *priv_key,
                       const unsigned char *cipher, size_t cipher_len,
                       int padding_mode,
                       unsigned char **out, size_t *out_len)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(priv_key, NULL);
    if (!ctx) { print_openssl_error("EVP_PKEY_CTX_new"); return -1; }

    int ret = -1;

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        print_openssl_error("EVP_PKEY_decrypt_init");
        goto out;
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding_mode) <= 0) {
        print_openssl_error("set_rsa_padding");
        goto out;
    }

    if (padding_mode == RSA_PKCS1_OAEP_PADDING) {
        if (EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256()) <= 0) goto out;
        if (EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha256()) <= 0) goto out;
    }

    size_t buf_len = 0;
    if (EVP_PKEY_decrypt(ctx, NULL, &buf_len, cipher, cipher_len) <= 0) {
        print_openssl_error("EVP_PKEY_decrypt (size query)");
        goto out;
    }

    unsigned char *buf = malloc(buf_len);
    if (!buf) goto out;

    if (EVP_PKEY_decrypt(ctx, buf, &buf_len, cipher, cipher_len) <= 0) {
        print_openssl_error("EVP_PKEY_decrypt");
        free(buf);
        goto out;
    }

    *out = buf;
    *out_len = buf_len;
    ret = 0;

out:
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

/* ============================================================
 * 
 * ============================================================ */
int rsa_Test()
{
    char AppPath[256] = {0};

     char pub_cert[256] = {0};// "server.crt.pem";
     char priv_key[256] = {0};//"server.key.pem";
     const char *password = NULL;  /* Optional password */
    const char *plaintext = "Hello RSA, this is a secret message!";

    Business_getAppPath(AppPath,sizeof(AppPath));

    sprintf(pub_cert,"%s/res/server.crt.pem",AppPath);
    sprintf(priv_key,"%s/res/server.key.pem",AppPath);

    OsLog(LOG_DEBUG,"=== RSA ENC/DEC DEMO ===\n");
    OsLog(LOG_DEBUG,"PUBKEY: %s\n", pub_cert);
    OsLog(LOG_DEBUG,"PRIVKEY: %s\n", priv_key);
    OsLog(LOG_DEBUG,"DATA: %s\n\n", plaintext);

    /* 1. load pub key */
    EVP_PKEY *pub = load_pubkey_from_cert(pub_cert);
    if (!pub) return 1;

    /* load private key */
    EVP_PKEY *priv = NULL;
    if (is_pem_privkey_encrypted(priv_key) == 1) {
        OsLog(LOG_DEBUG,"Encrypted private key detected, password required\n");
        if (!password) {
            OsLog(LOG_DEBUG, "Error: The encryption private key must provide a password parameter\n");
            EVP_PKEY_free(pub);
            return 1;
        }
        priv = load_privkey_with_password(priv_key, password);
    } else {
        priv = load_privkey(priv_key);
    }
    if (!priv) { EVP_PKEY_free(pub); return 1; }

    printf("private key type: %s, bit: %d\n\n",
           get_privkey_type(priv), get_privkey_bits(priv));

    OsLog(LOG_DEBUG,"[key info]\n");
    OsLog(LOG_DEBUG,"  key type: %s\n", get_privkey_type(pub));
    OsLog(LOG_DEBUG,"  key length: %d bit\n\n", EVP_PKEY_bits(pub));

    /* ============ Mode 1：PKCS#1 v1.5 ============ */
    OsLog(LOG_DEBUG,"--- Mode 1:PKCS#1 v1.5 panding ---\n");
    {
        unsigned char *cipher = NULL, *decrypted = NULL;
        size_t cipher_len = 0, decrypted_len = 0;

        if (rsa_encrypt(pub,
                        (const unsigned char *)plaintext, strlen(plaintext),
                        RSA_PKCS1_PADDING,
                        &cipher, &cipher_len) != 0) {
            goto cleanup;
        }
        OsLog(LOG_DEBUG,"  After encryption: %zu byte (First 16 bytes: ", cipher_len);
        for (size_t i = 0; i < 16 && i < cipher_len; i++)
            OsLog(LOG_DEBUG,"%02X", cipher[i]);
        OsLog(LOG_DEBUG,"...)\n");

        if (rsa_decrypt(priv, cipher, cipher_len,
                        RSA_PKCS1_PADDING,
                        &decrypted, &decrypted_len) != 0) {
            free(cipher);
            goto cleanup;
        }
        OsLog(LOG_DEBUG,"  After decryption: %.*s\n\n", (int)decrypted_len, decrypted);

        free(cipher);
        free(decrypted);
    }

    /* ============ Mode 2：OAEP(SHA256) ============ */
    OsLog(LOG_DEBUG,"--- Mode 2:RSA-OAEP(SHA256) padding ---\n");
    {
        unsigned char *cipher = NULL, *decrypted = NULL;
        size_t cipher_len = 0, decrypted_len = 0;

        if (rsa_encrypt(pub,
                        (const unsigned char *)plaintext, strlen(plaintext),
                        RSA_PKCS1_OAEP_PADDING,
                        &cipher, &cipher_len) != 0) {
            goto cleanup;
        }
        OsLog(LOG_DEBUG,"  After encryption: %zu byte (First 16 bytes:", cipher_len);
        for (size_t i = 0; i < 16 && i < cipher_len; i++)
            OsLog(LOG_DEBUG,"%02X", cipher[i]);
        OsLog(LOG_DEBUG,"...)\n");

        if (rsa_decrypt(priv, cipher, cipher_len,
                        RSA_PKCS1_OAEP_PADDING,
                        &decrypted, &decrypted_len) != 0) {
            free(cipher);
            goto cleanup;
        }
        OsLog(LOG_DEBUG,"  After decryption: %.*s\n\n", (int)decrypted_len, decrypted);

        free(cipher);
        free(decrypted);
    }

    OsLog(LOG_DEBUG,"--- Maximum length for a single encryption ---\n");
    {
        int bits = EVP_PKEY_bits(pub);
        int bytes = bits / 8;
        OsLog(LOG_DEBUG,"  key bits: %d\n", bits);
        OsLog(LOG_DEBUG,"  PKCS#1 v1.5  Maximum plaintext: %d bytes\n", bytes - 11);
        OsLog(LOG_DEBUG,"  OAEP(SHA256) Maximum plaintext: %d bytes\n", bytes - 2 * 32 - 2);
        OsLog(LOG_DEBUG,"  OAEP(SHA1)   Maximum plaintext: %d bytes\n", bytes - 2 * 20 - 2);
    }

cleanup:
    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);
    OsLog(LOG_DEBUG,"\n=== Done ===\n");

    char htc_pubkey_pem[256] = {0};
    sprintf(htc_pubkey_pem,"%s/res/uat_rsapubkey.pem",AppPath);
    OsLog(LOG_DEBUG,"pem path: %s",htc_pubkey_pem);
    EVP_PKEY *htc_pub = load_pubkey(htc_pubkey_pem);
    if (!htc_pub) return 1;
        /* ============ Mode 1：PKCS#1 v1.5 ============ */
    OsLog(LOG_DEBUG,"--- Mode 1:PKCS#1 v1.5 panding ---\n");
    {
        unsigned char *cipher = NULL, *decrypted = NULL;
        size_t cipher_len = 0, decrypted_len = 0;

        if (rsa_encrypt(htc_pub,
                        (const unsigned char *)plaintext, strlen(plaintext),
                        RSA_PKCS1_PADDING,
                        &cipher, &cipher_len) != 0) {
            if(cipher)
                free(cipher);                
            EVP_PKEY_free(htc_pub);
            return 0;
        }
        OsLog(LOG_DEBUG,"  After encryption: %zu byte (First 16 bytes: ", cipher_len);
        for (size_t i = 0; i < 16 && i < cipher_len; i++)
            OsLog(LOG_DEBUG,"%02X", cipher[i]);

        // log_debug_hex("enc data:",cipher,cipher_len);    
        OsLog(LOG_DEBUG,"...)\n");
        free(cipher);
    }
    EVP_PKEY_free(htc_pub);
    return 0;
}

int test()
{
    char AppPath[256] = {0};
    const char *plaintext = "Hello RSA, this is a secret message!";
    Business_getAppPath(AppPath,sizeof(AppPath));
	char htc_pubkey_pem[256] = {0};
    sprintf(htc_pubkey_pem,"%s/res/uat_rsapubkey.pem",AppPath);
    OsLog(LOG_DEBUG,"pem path: %s",htc_pubkey_pem);
    EVP_PKEY *htc_pub = load_pubkey(htc_pubkey_pem);
    if (!htc_pub) return 1;
        /* ============ Mode 1：PKCS#1 v1.5 ============ */
    OsLog(LOG_DEBUG,"--- Mode 1:PKCS#1 v1.5 panding ---\n");
    {
        unsigned char *cipher = NULL, *decrypted = NULL;
        size_t cipher_len = 0, decrypted_len = 0;
 
        if (rsa_encrypt(htc_pub,
                        (const unsigned char *)plaintext, strlen(plaintext),
                        RSA_PKCS1_PADDING,
                        &cipher, &cipher_len) != 0) {
            if(cipher)
                free(cipher);                
            EVP_PKEY_free(htc_pub);
            return 0;
        }
        OsLog(LOG_DEBUG,"  After encryption: %zu byte (First 16 bytes: ", cipher_len);
        for (size_t i = 0; i < 16 && i < cipher_len; i++)
            OsLog(LOG_DEBUG,"%02X", cipher[i]);
        OsLog(LOG_DEBUG,"...)\n");
        free(cipher);
    }
    EVP_PKEY_free(htc_pub);
	return 0;
}