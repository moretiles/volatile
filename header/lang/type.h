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

    // Literal values of N bits
    VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR8, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_SCALAR8,
    VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR16, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_SCALAR16,
    VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR32, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_SCALAR32,
    VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_SCALAR64,

    // Pointer to N-bit value
    VLTL_LANG_TYPE_INTEGRAL_INT_POINTER8, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_POINTER8,
    VLTL_LANG_TYPE_INTEGRAL_INT_POINTER16, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_POINTER16,
    VLTL_LANG_TYPE_INTEGRAL_INT_POINTER32, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_POINTER32,
    VLTL_LANG_TYPE_INTEGRAL_INT_POINTER64, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_POINTER64,

    // Indirection to N-bit value
    VLTL_LANG_TYPE_INTEGRAL_INT_INDIRECTION8, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_INDIRECTION8,
    VLTL_LANG_TYPE_INTEGRAL_INT_INDIRECTION16, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_INDIRECTION16,
    VLTL_LANG_TYPE_INTEGRAL_INT_INDIRECTION32, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_INDIRECTION32,
    VLTL_LANG_TYPE_INTEGRAL_INT_INDIRECTION64, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_INDIRECTION64,

    // Array of N-bit values
    VLTL_LANG_TYPE_INTEGRAL_INT_ARRAY8, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_ARRAY8,
    VLTL_LANG_TYPE_INTEGRAL_INT_ARRAY16, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_ARRAY16,
    VLTL_LANG_TYPE_INTEGRAL_INT_ARRAY32, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_ARRAY32,
    VLTL_LANG_TYPE_INTEGRAL_INT_ARRAY64, VLTL_LANG_TYPE_INTEGRAL_UNSIGNED_ARRAY64,

    // Holding the type itself still must be done with a pointer, slightly different abstraction
    VLTL_LANG_TYPE_INTEGRAL_USERDEFINED_ITSELF,
    VLTL_LANG_TYPE_INTEGRAL_USERDEFINED_POINTER,
    VLTL_LANG_TYPE_INTEGRAL_USERDEFINED_INDIRECTION,
    VLTL_LANG_TYPE_INTEGRAL_USERDEFINED_ARRAY
} Vltl_lang_type_integral;

typedef enum vltl_lang_type_integral_kind {
    VLTL_LANG_TYPE_INTEGRAL_KIND_SCALAR,
    VLTL_LANG_TYPE_INTEGRAL_KIND_POINTER,
    VLTL_LANG_TYPE_INTEGRAL_KIND_INDIRECTION,
    VLTL_LANG_TYPE_INTEGRAL_KIND_ARRAY
} Vltl_lang_type_integral_kind;

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
