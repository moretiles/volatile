#pragma once

#include <lang/literal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vltl_lang_global {
    const char *name;

    const Vltl_lang_type *type;
    Vltl_lang_type_attribute *attributes[9];
    Vltl_lang_literal *literal;
} Vltl_lang_global;

// temporary globals
extern struct vltl_lang_literal vltl_lang_global_a_literal;
extern struct vltl_lang_literal vltl_lang_global_b_literal;

extern struct vltl_lang_global vltl_lang_global_a;
extern struct vltl_lang_global vltl_lang_global_b;

#ifdef __cplusplus
}
#endif
