#pragma once

#include <lang/token.h>
#include <ast.h>
#include <sast.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vltl_trace {
    const char *as_code;
    Vltl_lang_token *as_token;
    Vltl_ast_operation *as_ast;
    Vltl_sast_operation *as_sast;
    const char *as_asm;
} Vltl_trace;

#ifdef __cplusplus
}
#endif
