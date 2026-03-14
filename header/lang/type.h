#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
// assumptions:
// kind for type changes rarely as it represents what a type actually is
// size changes somewhat as different math occurs.
// instance changes heavily as going from pointer to indirection back to pointer to indirection happens often.
typedef enum vltl_lang_type_integral_kind {
    VLTL_LANG_TYPE_INTEGRAL_KIND_SIGNED,
    VLTL_LANG_TYPE_INTEGRAL_KIND_UNSIGNED,
    //VLTL_LANG_TYPE_INTEGRAL_KIND_FLOAT,
    //VLTL_LANG_TYPE_INTEGRAL_KIND_SATOMIC,
    //VLTL_LANG_TYPE_INTEGRAL_KIND_UATOMIC,
    VLTL_LANG_TYPE_INTEGRAL_KIND_USERDEFINED
} Vltl_lang_type_integral_kind;

typedef enum vltl_lang_type_integral_size {
    VLTL_LANG_TYPE_INTEGRAL_SIZE_8,
    VLTL_LANG_TYPE_INTEGRAL_SIZE_16,
    VLTL_LANG_TYPE_INTEGRAL_SIZE_32,
    VLTL_LANG_TYPE_INTEGRAL_SIZE_64,
    VLTL_LANG_TYPE_INTEGRAL_SIZE_USERDEFINED
} Vltl_lang_type_integral_size;

// VLTL_LANG_TYPE_INTEGRAL_KIND_SCALAR used only for literals and constants
// VLTL_LANG_TYPE_INTEGRAL_KIND_ARRAY is special kind of scalar value because arrays are weird =_=
// VLTL_LANG_TYPE_INTEGRAL_KIND_POINTER is used when a pointer has been stored or address-of operator used
// VLTL_LANG_TYPE_INTEGRAL_KIND_INDIRECTION used when getting the value of some variable as part of an expression, dereferencing a pointer, or indexing into an array
typedef enum vltl_lang_type_integral_instance {
    VLTL_LANG_TYPE_INTEGRAL_KIND_SCALAR,
    VLTL_LANG_TYPE_INTEGRAL_KIND_ARRAY,
    VLTL_LANG_TYPE_INTEGRAL_KIND_POINTER,
    VLTL_LANG_TYPE_INTEGRAL_KIND_INDIRECTION
} Vltl_lang_type_integral_instance;

typedef struct vltl_lang_type_integral {
    Vltl_lang_type_integral_kind kind;
    Vltl_lang_type_integral_size size;
    Vltl_lang_type_integral_instance instance;
} Vltl_lang_type_integral;
*/

typedef enum vltl_lang_type_integral {
    VLTL_LANG_TYPE_INTEGRAL_UNSET,
    VLTL_LANG_TYPE_INTEGRAL_INVALID,

    // VLTL_LANG_TYPE_INTEGRAL_TBD means that the result of an ast/sast operation is not known at compile time!
    // Not sure if I would ever use this!
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

/*
// VLTL_LANG_TYPE_IDENTIFIER_ALIAS is alias for integral type (like i16, i16)
// VLTL_LANG_TYPE_IDENTIFIER_USERDEFINED used for any user-defined type with its own fields
// VLTL_LANG_TYPE_IDENTIFIER_EXTERN use for any externally defined type
typedef enum vltl_lang_type_kind {
    VLTL_LANG_TYPE_IDENTIFIER_ALIAS,
    VLTL_LANG_TYPE_IDENTIFIER_USERDEFINED,
    VLTL_LANG_TYPE_IDENTIFIER_EXTERN
} Vltl_lang_type_kind;

// VLTL_LANG_TYPE_INTEGRAL_KIND_SCALAR is used only for literals and constants
// VLTL_LANG_TYPE_INTEGRAL_KIND_ARRAY used for special kind of scalar value because arrays are weird =_=
// VLTL_LANG_TYPE_INTEGRAL_KIND_POINTER used when a pointer has been stored or address-of operator used
// VLTL_LANG_TYPE_INTEGRAL_KIND_INDIRECTION used when getting the value of some variable as part of an expression, dereferencing a pointer, or indexing into an array
typedef enum vltl_lang_type_instance {
    VLTL_LANG_TYPE_KIND_SCALAR,
    VLTL_LANG_TYPE_KIND_ARRAY,
    VLTL_LANG_TYPE_KIND_POINTER,
    VLTL_LANG_TYPE_KIND_INDIRECTION
} Vltl_lang_type_instance;

typedef struct vltl_lang_type_intern {
    const char *name;
    Vltl_lang_type_kind kind;

    union {
        Vltl_lang_type_integral_size integral_size; // set for aliased types
        size_t total_size; // set for userdefined types
    };
    union {
        const struct vltl_lang_type_intern *alias_of; // set for aliased types
        const struct vltl_lang_type_intern *fields[8]; // set for userdefined types
    };
} Vltl_lang_type_intern;

typedef enum vltl_lang_type_attribute {
    VLTL_LANG_TYPE_ATTRIBUTE_KIND_UNSET,
    VLTL_LANG_TYPE_ATTRIBUTE_KIND_INVALID,

    VLTL_LANG_TYPE_ATTRIBUTE_KIND_SIGNED,
    VLTL_LANG_TYPE_ATTRIBUTE_KIND_UNSIGNED,
    //VLTL_LANG_TYPE_ATTRIBUTE_KIND_ATOMIC
} Vltl_lang_type_attribute;

typedef struct vltl_lang_type {
    const Vltl_lang_type_intern *intern;
    Vltl_lang_type_attribute attributes[8];
    Vltl_lang_type_instance instance;

    // Only used when instance is array (in order to support multi-dimensional array).
    // Note that for a 2d array defined as `local my_array[2][3]` would be interpreted as forming an array with two entry, three entry, and one entry fields or .part_of = { 2, 3, 1 }.
    // Thus, my_array[1][1] would actually refer to a one element length cell of my_array rather than a scalar.
    uint16_t *(part_of[15]);

    // Indirection of 0 for scalar or dereferencing variable (without using indirection operator), -1 for pointer, -2 for pointer to pointer, 1 for dereferencing pointer (using indirection operator) or indexing into an array, 2 for indexing into an array while also indexing into an array.
    int16_t indirection_level;
} Vltl_lang_type;
*/

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
    struct vltl_lang_type *accepted_fields[8];
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

extern Vltl_lang_type vltl_lang_type_nullstr;

#ifdef __cplusplus
}
#endif
