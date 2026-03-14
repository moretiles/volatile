#pragma once

#include <lang/token.h>
#include <ast.h>
#include <sast.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_ERROR_BUF_CAP (999)

#ifdef __cplusplus
extern thread_local char vltl_error_buf[VLTL_ERROR_BUF_CAP];
#else
extern _Thread_local char vltl_error_buf[VLTL_ERROR_BUF_CAP];
#endif

void vltl_error(const char *string);
void vltl_error_line(const Vltl_lexer_line bad_line);
void vltl_error_token(const Vltl_lang_token bad_token);
void vltl_error_ast(const Vltl_ast_operation bad_ast);
void vltl_error_sast(const Vltl_sast_operation bad_sast);
void vltl_error_asm(const char *bad_asm);

#ifdef __cplusplus
}
#endif
