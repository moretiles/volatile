#pragma once

#include <lang/type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_operation_kind {
    VLTL_LANG_OPERATION_KIND_UNSET,
    VLTL_LANG_OPERATION_KIND_INVALID,

    VLTL_LANG_OPERATION_KIND_DEF,
    VLTL_LANG_OPERATION_KIND_EQUALS,
    VLTL_LANG_OPERATION_KIND_EQUALSEQUALS,
    VLTL_LANG_OPERATION_KIND_ADD,
    VLTL_LANG_OPERATION_KIND_SUB,
    VLTL_LANG_OPERATION_KIND_GLOBAL
} Vltl_lang_operation_kind;

typedef struct vltl_lang_operation {
    Vltl_lang_operation_kind operation_kind;

    const char *name;
    const Vltl_lang_type *type;
    const struct vltl_lang_type *accepted_fields[9];
} Vltl_lang_operation;

extern struct vltl_lang_operation vltl_lang_operation_equals;
extern struct vltl_lang_operation vltl_lang_operation_add;
extern struct vltl_lang_operation vltl_lang_operation_sub;
extern struct vltl_lang_operation vltl_lang_operation_global;

#ifdef __cplusplus
}
#endif
