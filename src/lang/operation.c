#include <lang/operation.h>

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
