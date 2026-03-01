#pragma once

#include <lang/literal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_LANG_CONSTANT_ATTRIBUTE_CAP (9LU)

typedef struct vltl_lang_constant {
    const char *name;

    const Vltl_lang_type *type;
    Vltl_lang_type_attribute *attributes[VLTL_LANG_CONSTANT_ATTRIBUTE_CAP];
    Vltl_lang_literal *literal;
} Vltl_lang_constant;

#ifdef __cplusplus
}
#endif
