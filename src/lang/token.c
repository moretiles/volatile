#include <lang/token.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int vltl_lang_token_stringify(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_lang_token src
) {
    int dest_len_helper2 = 0;
    if(dest == NULL || dest_cap == 0 || dest_len == NULL) {
        return EINVAL;
    }

    switch(src.kind) {
    case VLTL_LANG_TOKEN_KIND_LITERAL:
        dest_len_helper2 = snprintf(dest, dest_cap, "%ld", (int64_t) src.literal.fields[0]);
        if(dest_len_helper2 < 0) {
            return ENOMEM;
        }

        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_UNKNOWN:
    case VLTL_LANG_TOKEN_KIND_CONSTANT:
    case VLTL_LANG_TOKEN_KIND_GLOBAL:
    case VLTL_LANG_TOKEN_KIND_LOCAL:
    case VLTL_LANG_TOKEN_KIND_ATTRIBUTE:
    case VLTL_LANG_TOKEN_KIND_OPERATION:
    case VLTL_LANG_TOKEN_KIND_TYPE:
    default:
        if(dest_cap < 4) {
            return ENOMEM;
        }
        memcpy(dest, "???", 3);
        dest[3] = 0;

        *dest_len = 4;
        break;
    }

    return 0;
}

bool vltl_lang_token_kind_valid(const Vltl_lang_token_kind kind) {
    switch(kind) {
    case VLTL_LANG_TOKEN_KIND_UNKNOWN:
    case VLTL_LANG_TOKEN_KIND_LITERAL:
    case VLTL_LANG_TOKEN_KIND_CONSTANT:
    case VLTL_LANG_TOKEN_KIND_GLOBAL:
    case VLTL_LANG_TOKEN_KIND_LOCAL:
    case VLTL_LANG_TOKEN_KIND_ATTRIBUTE:
    case VLTL_LANG_TOKEN_KIND_OPERATION:
    case VLTL_LANG_TOKEN_KIND_TYPE:
        break;
    default:
        return false;
        break;
    }

    return true;
}
