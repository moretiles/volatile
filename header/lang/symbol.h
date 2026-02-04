#pragma once

#include <lang/type.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_symbol_scope {
    VLTL_LANG_SYMBOL_SCOPE_UNSET,
    VLTL_LANG_SYMBOL_SCOPE_INVALID,

    VLTL_LANG_SYMBOL_SCOPE_LOCAL,
    VLTL_LANG_SYMBOL_SCOPE_GLOBAL,
    VLTL_LANG_SYMBOL_SCOPE_INTRINSIC
} Vltl_lang_symbol_scope;

typedef struct vltl_lang_symbol {
    enum vltl_lang_symbol_scope scope;
    const char *name;
    const Vltl_lang_type *type;
    Vltl_lang_type_attribute attributes[9];
} Vltl_lang_symbol;

bool vltl_lang_symbol_valid(const Vltl_lang_symbol symbol);

#ifdef __cplusplus
}
#endif
