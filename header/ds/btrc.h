#pragma once

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
extern thread_local int btrc_strncpy_tmp;
#else
extern _Thread_local int btrc_strncpy_tmp;
#endif

#ifdef __cplusplus
int btrc_strncpy(size_t *src_len, char *__restrict dest, const char *__restrict src, size_t n);
#else
int btrc_strncpy(size_t *src_len, char *restrict dest, const char *restrict src, size_t n);
#endif
#define BTRC_SNPRINTF(return_value, dest_len, str, n, format, ...) \
do { \
    if(return_value == NULL) { \
        *return_value = EINVAL; \
        continue; \
    } \
    if(str == NULL) { \
        *return_value = EINVAL; \
        continue; \
    } \
    if(format == NULL) { \
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
#define BTRC_SNPRINTF2(dest_len, str, n, format, ...) __extension__({ \
	do { \
	    if(str == NULL) { \
	        btrc_strncpy_tmp = EINVAL; \
	        continue; \
	    } \
	    if(format == NULL) { \
	        btrc_strncpy_tmp = EINVAL; \
	        continue; \
	    } \
	    if(n == 0) { \
	        btrc_strncpy_tmp = EXFULL; \
	        continue; \
	    } \
	      \
	    btrc_strncpy_tmp = snprintf(str, n, format, __VA_ARGS__); \
	    if(btrc_strncpy_tmp < 0) { \
	        btrc_strncpy_tmp = ENOTRECOVERABLE; \
	        continue; \
	    } else if((size_t) btrc_strncpy_tmp >= n) { \
	        *dest_len = n; \
	        btrc_strncpy_tmp = EXFULL; \
	        continue; \
	    } \
	      \
	    *dest_len = (size_t) btrc_strncpy_tmp; \
	    btrc_strncpy_tmp = 0; \
	} while(0); \
    btrc_strncpy_tmp; \
})

#ifdef __cplusplus
}
#endif
