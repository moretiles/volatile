#include <ds/btrc.h>
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
    int ret = 0;
    int dest_len_helper2 = 0;
    if(dest == NULL || dest_cap == 0 || dest_len == NULL) {
        return EINVAL;
    }

    switch(src.kind) {
    case VLTL_LANG_TOKEN_KIND_LITERAL:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%ld", (int64_t) src.literal.fields[0]);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_CONSTANT:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.constant->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_GLOBAL:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.global->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_LOCAL:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.local->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_FUNCTION:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.function->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_ATTRIBUTE:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.attribute->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_OPERATION:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.operation->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_TYPE:
        BTRC_SNPRINTF(&ret, &dest_len_helper2, dest, dest_cap, "%s", (const char *) src.type->name);
        *dest_len = ((size_t) dest_len_helper2) + 1;
        break;
    case VLTL_LANG_TOKEN_KIND_UNKNOWN:
    default:
        ret = btrc_strncpy(dest_len, dest, "???", 3);
        break;
    }

    return ret;
}

bool vltl_lang_token_kind_valid(const Vltl_lang_token_kind kind) {
    switch(kind) {
    case VLTL_LANG_TOKEN_KIND_UNKNOWN:
    case VLTL_LANG_TOKEN_KIND_LITERAL:
    case VLTL_LANG_TOKEN_KIND_CONSTANT:
    case VLTL_LANG_TOKEN_KIND_FUNCTION:
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
