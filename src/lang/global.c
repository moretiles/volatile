#include <lang/global.h>

Vltl_lang_literal vltl_lang_global_a_literal = {
    .name = "2", .type = &vltl_lang_type_long, .attributes = { 0 }, .fields = { (void *) 2 }
};
Vltl_lang_literal vltl_lang_global_b_literal = {
    .name = "3", .type = &vltl_lang_type_long, .attributes = { 0 }, .fields = { (void *) 3 }
};

Vltl_lang_global vltl_lang_global_a = {
    .name = "a", .type = &vltl_lang_type_long, .literal = &vltl_lang_global_a_literal
};
Vltl_lang_global vltl_lang_global_b = {
    .name = "b", .type = &vltl_lang_type_long, .literal = &vltl_lang_global_b_literal
};
