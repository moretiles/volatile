#pragma once

#include <lang/operation.h>
#include <lang/literal.h>
#include <lang/symbol.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_token_kind {
    VLTL_LANG_TOKEN_KIND_UNSET,
    VLTL_LANG_TOKEN_KIND_INVALID,

    VLTL_LANG_TOKEN_KIND_LITERAL,
    VLTL_LANG_TOKEN_KIND_SYMBOL,
    VLTL_LANG_TOKEN_KIND_OPERATION
} Vltl_lang_token_kind;

typedef struct vltl_lang_token {
    Vltl_lang_token_kind kind;
    union {
        Vltl_lang_literal literal;
        Vltl_lang_symbol symbol;
        Vltl_lang_operation operation;
    };
} Vltl_lang_token;

bool vltl_lang_token_kind_valid(const Vltl_lang_token_kind kind);
int vltl_lang_token_stringify(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_lang_token src);

#ifdef __cplusplus
}
#endif
