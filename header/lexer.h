#pragma once

#include <lang/token.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_LEXER_LINE_TOKENS_MAX (20)

typedef struct vltl_lexer_token {
    size_t offset;
    size_t length;
    Vltl_lang_token_kind kind;
} Vltl_lexer_token;

typedef struct vltl_lexer_line {
    const char *text;

    const char *filename;
    const char *function_name;
    size_t linenumber;

    Vltl_lexer_token tokens[VLTL_LEXER_LINE_TOKENS_MAX];
    size_t token_count;
} Vltl_lexer_line;

bool vltl_lexer_line_valid(const Vltl_lexer_line line);
int vltl_lexer_token_init(
    Vltl_lexer_token *dest,
    size_t offset,
    size_t length,
    Vltl_lang_token_kind kind
);
int vltl_lexer_line_convert(Vltl_lexer_line *dest, const char *src);

#ifdef __cplusplus
}
#endif
