#pragma once

#include <lang/type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_operation_kind {
    VLTL_LANG_OPERATION_KIND_UNSET,
    VLTL_LANG_OPERATION_KIND_INVALID,

    VLTL_LANG_OPERATION_KIND_EQUALS,
    VLTL_LANG_OPERATION_KIND_EQUALSEQUALS,
    VLTL_LANG_OPERATION_KIND_ADD,
    VLTL_LANG_OPERATION_KIND_SUB,
    VLTL_LANG_OPERATION_KIND_GLOBAL,
    VLTL_LANG_OPERATION_KIND_CONSTANT,
    VLTL_LANG_OPERATION_KIND_LOCAL,
    VLTL_LANG_OPERATION_KIND_FUNCTION,
    VLTL_LANG_OPERATION_KIND_BODY_OPEN,
    VLTL_LANG_OPERATION_KIND_BODY_CLOSE,
    VLTL_LANG_OPERATION_KIND_RETURN
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
extern struct vltl_lang_operation vltl_lang_operation_constant;
extern struct vltl_lang_operation vltl_lang_operation_local;
extern struct vltl_lang_operation vltl_lang_operation_function;
extern struct vltl_lang_operation vltl_lang_operation_body_open;
extern struct vltl_lang_operation vltl_lang_operation_body_close;
extern struct vltl_lang_operation vltl_lang_operation_return;

#ifdef __cplusplus
}
#endif
