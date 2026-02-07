#pragma once

#include <lang/operation.h>
#include <lang/literal.h>
#include <lang/type.h>
#include <lang/local.h>
#include <lang/global.h>
#include <lang/constant.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_token_kind {
    VLTL_LANG_TOKEN_KIND_UNSET,
    VLTL_LANG_TOKEN_KIND_INVALID,

    VLTL_LANG_TOKEN_KIND_UNKNOWN,
    VLTL_LANG_TOKEN_KIND_LITERAL,
    VLTL_LANG_TOKEN_KIND_CONSTANT,
    VLTL_LANG_TOKEN_KIND_GLOBAL,
    VLTL_LANG_TOKEN_KIND_LOCAL,
    VLTL_LANG_TOKEN_KIND_ATTRIBUTE,
    VLTL_LANG_TOKEN_KIND_OPERATION,
    VLTL_LANG_TOKEN_KIND_TYPE
} Vltl_lang_token_kind;

typedef struct vltl_lang_token {
    Vltl_lang_token_kind kind;
    union {
        const char *unknown;
        Vltl_lang_literal literal;
        const Vltl_lang_type *type;
        Vltl_lang_operation *operation;
        const Vltl_lang_type_attribute *attribute;
        Vltl_lang_constant *constant;
        Vltl_lang_global *global;
        Vltl_lang_local *local;
    };
} Vltl_lang_token;

bool vltl_lang_token_kind_valid(const Vltl_lang_token_kind kind);
int vltl_lang_token_stringify(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_lang_token src);

#ifdef __cplusplus
}
#endif
