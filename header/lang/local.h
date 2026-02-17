#pragma once

#include <ds/iestack.h>

#include <lang/literal.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vltl_lang_local {
    const char *name;

    const Vltl_lang_type *type;
    Vltl_lang_type_attribute *attributes[9];
    Vltl_lang_literal *literal;

    // Value of frame_offset used to indicate local lives at -frame_offset[%rbp]
    size_t frame_offset;
} Vltl_lang_local;

Vltl_lang_local *vltl_lang_local_create(
    const char *name, const Vltl_lang_type *type,
    Vltl_lang_type_attribute *attributes[9], Vltl_lang_literal *literal
);
int vltl_lang_local_init(
    Vltl_lang_local **dest, void *memory, const char *name, const Vltl_lang_type *type,
    Vltl_lang_type_attribute *attributes[9], Vltl_lang_literal *literal
);

#ifdef __cplusplus
}
#endif
