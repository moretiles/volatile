#pragma once

#include <lang/token.h>
#include <ast.h>
#include <sast.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vltl_trace {
    Vltl_lexer_line *as_line;
    Vltl_lang_token *as_token;
    Vltl_ast_operation *as_ast;
    Vltl_sast_operation *as_sast;
} Vltl_trace;

#ifdef __cplusplus
}
#endif
