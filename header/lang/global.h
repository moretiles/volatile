#pragma once

#include <lang/literal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_LANG_GLOBAL_ATTRIBUTES_CAP (9LU)

typedef struct vltl_lang_global {
    const char *name;

    const Vltl_lang_type *type;
    Vltl_lang_type_attribute *attributes[VLTL_LANG_GLOBAL_ATTRIBUTES_CAP];
    Vltl_lang_literal *literal;
} Vltl_lang_global;

#ifdef __cplusplus
}
#endif
