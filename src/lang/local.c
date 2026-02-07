#include <lang/local.h>

Vltl_lang_literal vltl_lang_local_c_literal = {
    .type = &vltl_lang_type_long, .fields = { 0 }
};
Vltl_lang_literal vltl_lang_local_d_literal = {
    .type = &vltl_lang_type_long, .fields = { 0 }
};

Vltl_lang_local vltl_lang_local_c = {
    .name = "c", .type = &vltl_lang_type_long, .literal = &vltl_lang_local_c_literal
};
Vltl_lang_local vltl_lang_local_d = {
    .name = "d", .type = &vltl_lang_type_long, .literal = &vltl_lang_local_d_literal
};
