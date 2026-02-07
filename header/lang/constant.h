#pragma once

#include <lang/literal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vltl_lang_constant {
    const char *name;

    const Vltl_lang_type *type;
    Vltl_lang_type_attribute *attributes[9];
    Vltl_lang_literal *literal;
} Vltl_lang_constant;

// temporary constant
extern struct vltl_lang_literal vltl_lang_constant_zero_literal;
extern struct vltl_lang_literal vltl_lang_constant_one_literal;

extern struct vltl_lang_constant vltl_lang_constant_zero;
extern struct vltl_lang_constant vltl_lang_constant_one;

#ifdef __cplusplus
}
#endif
