#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_operation_kind {
    VLTL_LANG_OPERATION_KIND_UNSET,
    VLTL_LANG_OPERATION_KIND_INVALID,

    VLTL_LANG_OPERATION_KIND_DEF,
    VLTL_LANG_OPERATION_KIND_EQUALS,
    VLTL_LANG_OPERATION_KIND_EQUALSEQUALS,
    VLTL_LANG_OPERATION_KIND_ADD
} Vltl_lang_operation_kind;

typedef struct vltl_lang_operation {
    const Vltl_lang_type *type;
    const struct vltl_lang_type *accepted_fields[9];
} Vltl_lang_operation;

#ifdef __cplusplus
}
#endif
