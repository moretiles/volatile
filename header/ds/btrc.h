#pragma once

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
int btrc_strncpy(size_t *src_len, char *__restrict dest, const char *__restrict src, size_t n);
#else
int btrc_strncpy(size_t *src_len, char *restrict dest, const char *restrict src, size_t n);
#endif
#define BTRC_SNPRINTF(return_value, dest_len, str, n, format, ...) \
do { \
    if(return_value == NULL || str == NULL || format == NULL) { \
        *return_value = EINVAL; \
        continue; \
    } \
    if(n == 0) { \
        *return_value = EXFULL; \
        continue; \
    } \
      \
    int __tmp_ret_val = snprintf(str, n, format, __VA_ARGS__); \
    if(__tmp_ret_val < 0) { \
        *return_value = ENOTRECOVERABLE; \
        continue; \
    } \
      \
    if((size_t) __tmp_ret_val >= n) { \
        *dest_len = n; \
        *return_value = EXFULL; \
        continue; \
    } \
      \
    *dest_len = __tmp_ret_val; \
    *return_value = 0; \
} while(0);

#ifdef __cplusplus
}
#endif
