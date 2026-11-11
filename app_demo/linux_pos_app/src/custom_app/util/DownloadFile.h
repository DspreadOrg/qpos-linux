#ifndef _DOWNLOAD_FILE_H
#define _DOWNLOAD_FILE_H

/* Download progress callback: downloaded = number of bytes downloaded, total = total file length (-1 means unknown) */
typedef void (*download_progress_cb)(long long downloaded, long long total);

/* Download API with progress callback; passing NULL for cb indicates no progress required */
int download_file_with_progress(const char *url, const char *outpath,
                                download_progress_cb cb);

int download_test();

#endif
