#include <ds/btrc.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

int btrc_strncpy(size_t *src_len, char *restrict dest, const char *restrict src, size_t n) {
    if(src_len == NULL || dest == NULL || src == NULL) {
        return EINVAL;
    }
    if(n == 0) {
        return EXFULL;
    }

    *src_len = strlen(src);
    if((*src_len + 1) < n) {
        n = *src_len + 1;
    }
    memcpy(dest, src, n);
    dest[*src_len] = 0;
    return 0;
}
