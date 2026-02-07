#include <lang/constant.h>

Vltl_lang_literal vltl_lang_constant_zero_literal = {
    .type = &vltl_lang_type_long, .fields = { 0 }
};
Vltl_lang_literal vltl_lang_constant_one_literal = {
    .type = &vltl_lang_type_long, .fields = { (void *) 1 }
};

Vltl_lang_constant vltl_lang_constant_zero = {
    .name = "zero", .type = &vltl_lang_type_long, .literal = &vltl_lang_constant_zero_literal
};
Vltl_lang_constant vltl_lang_constant_one = {
    .name = "one", .type = &vltl_lang_type_long, .literal = &vltl_lang_constant_one_literal
};
