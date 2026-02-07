#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_lang_type_integral {
    VLTL_LANG_TYPE_INTEGRAL_UNSET,
    VLTL_LANG_TYPE_INTEGRAL_INVALID,

    // VLTL_LANG_TYPE_INTEGRAL_TBD demonstrates that the result of a ast/sast operation is not known at compile time!
    VLTL_LANG_TYPE_INTEGRAL_TBD,
    VLTL_LANG_TYPE_INTEGRAL_BOOL,
    VLTL_LANG_TYPE_INTEGRAL_INT8,
    VLTL_LANG_TYPE_INTEGRAL_INT16,
    VLTL_LANG_TYPE_INTEGRAL_INT32,
    VLTL_LANG_TYPE_INTEGRAL_INT64,

    // VLTL_LANG_TYPE_INTEGRAL_USERDEFINED being set means a user-defined type is appearing in some context it should not!
    VLTL_LANG_TYPE_INTEGRAL_USERDEFINED
} Vltl_lang_type_integral;

typedef enum vltl_lang_type_attribute_kind {
    VLTL_LANG_TYPE_ATTRIBUTE_KIND_UNSET,
    VLTL_LANG_TYPE_ATTRIBUTE_KIND_INVALID,

    VLTL_LANG_TYPE_ATTRIBUTE_KIND_SIGNED,
    VLTL_LANG_TYPE_ATTRIBUTE_KIND_UNSIGNED
} Vltl_lang_type_attribute_kind;

typedef struct vltl_lang_type_attribute {
    Vltl_lang_type_attribute_kind attribute_kind;
    const char *name;
} Vltl_lang_type_attribute;

typedef struct vltl_lang_type {
    const char *name;
    Vltl_lang_type_integral integral_type;

    // only relevant when (integral_type == VLTL_LANG_TYPE_INTEGRAL_USERDEFINED)
    struct vltl_lang_type *accepted_fields[9];
} Vltl_lang_type;

bool vltl_lang_type_integral_valid(const Vltl_lang_type_integral type_integral);

// attributes
extern Vltl_lang_type_attribute vltl_lang_type_attribute_signed;
extern Vltl_lang_type_attribute vltl_lang_type_attribute_unsigned;

// default types
extern Vltl_lang_type vltl_lang_type_long;
extern Vltl_lang_type vltl_lang_type_int;
extern Vltl_lang_type vltl_lang_type_short;
extern Vltl_lang_type vltl_lang_type_char;

#ifdef __cplusplus
}
#endif
