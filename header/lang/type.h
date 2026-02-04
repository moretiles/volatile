#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_type_integral {
    VLTL_LANG_TYPE_INTEGRAL_UNSET,
    VLTL_LANG_TYPE_INTEGRAL_INVALID,

    // VLTL_LANG_TYPE_INTEGRAL_TBD demonstrate that the result of a ast/sast operation is not known at compile time
    VLTL_LANG_TYPE_INTEGRAL_TBD,
    VLTL_LANG_TYPE_INTEGRAL_BOOL,
    VLTL_LANG_TYPE_INTEGRAL_INT8,
    VLTL_LANG_TYPE_INTEGRAL_INT16,
    VLTL_LANG_TYPE_INTEGRAL_INT32,
    VLTL_LANG_TYPE_INTEGRAL_INT64,
    VLTL_LANG_TYPE_INTEGRAL_USERDEFINED
} Vltl_lang_type_integral;

typedef enum vltl_lang_type_attribute {
    VLTL_LANG_TYPE_ATTRIBUTE_UNSET,
    VLTL_LANG_TYPE_ATTRIBUTE_INVALID,

    VLTL_LANG_TYPE_ATTRIBUTE_SIGNED,
    VLTL_LANG_TYPE_ATTRIBUTE_UNSIGNED
} Vltl_lang_type_attribute;

typedef struct vltl_lang_type {
    const char *name;
    Vltl_lang_type_integral integral_type;

    // only relevant when (integral_type == VLTL_LANG_TYPE_INTEGRAL_USERDEFINED)
    struct vltl_lang_type *accepted_fields[9];
} Vltl_lang_type;

// default types
extern Vltl_lang_type vltl_lang_type_long;

#ifdef __cplusplus
}
#endif
