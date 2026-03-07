#include <lang/type.h>

bool vltl_lang_type_integral_valid(const Vltl_lang_type_integral type_integral) {
    switch(type_integral) {
    case VLTL_LANG_TYPE_INTEGRAL_TBD:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR8:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR16:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR32:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64:
        break;
    default:
        return false;
    }

    return true;
}

Vltl_lang_type_attribute vltl_lang_type_attribute_signed = {
    .attribute_kind = VLTL_LANG_TYPE_ATTRIBUTE_KIND_SIGNED,
    .name = "signed"
};
Vltl_lang_type_attribute vltl_lang_type_attribute_unsigned = {
    .attribute_kind = VLTL_LANG_TYPE_ATTRIBUTE_KIND_UNSIGNED,
    .name = "unsigned"
};

Vltl_lang_type vltl_lang_type_long = {
    .name = "long", .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64, .accepted_fields = { 0 }
};
Vltl_lang_type vltl_lang_type_int = {
    .name = "int", .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR32, .accepted_fields = { 0 }
};
Vltl_lang_type vltl_lang_type_short = {
    .name = "short", .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR16, .accepted_fields = { 0 }
};
Vltl_lang_type vltl_lang_type_char = {
    .name = "char", .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR8, .accepted_fields = { 0 }
};
