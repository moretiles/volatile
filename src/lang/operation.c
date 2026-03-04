#include <lang/operation.h>

Vltl_lang_operation vltl_lang_operation_equals = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_EQUALS,
    .name = "=",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_add = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_ADD,
    .name = "+",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_sub = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_SUB,
    .name = "-",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_mul = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_MUL,
    .name = "*",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_div = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_DIV,
    .name = "/",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_comma = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_COMMA,
    .name = ",",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_typeas = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_TYPEAS,
    .name = ":",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_grouping_open = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_GROUPING_OPEN,
    .name = "(",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_grouping_close = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_GROUPING_CLOSE,
    .name = ")",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_global = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_GLOBAL,
    .name = "global",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_constant = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_CONSTANT,
    .name = "constant",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_local = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_LOCAL,
    .name = "local",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_function = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_FUNCTION,
    .name = "function",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_body_open = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_BODY_OPEN,
    .name = "{",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_body_close = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_BODY_CLOSE,
    .name = "}",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
Vltl_lang_operation vltl_lang_operation_return = {
    .operation_kind = VLTL_LANG_OPERATION_KIND_RETURN,
    .name = "return",
    .type = &vltl_lang_type_long,
    .accepted_fields = { 0 }
};
