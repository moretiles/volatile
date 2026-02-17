#pragma once

#include <ds/vqueue.h>
#include <lang/token.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_LEXER_LINE_TOKENS_MAX (20LU)

typedef struct vltl_lexer_token {
    const char *line;
    Vltl_lang_token token;
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
    const char *line,
    Vltl_lang_token token
);
int vltl_lexer_line_convert(Vltl_lexer_line *dest, const char *src);
int vltl_lexer_token_chomp(
    size_t *start_of_next_token, size_t *end_of_next_token,
    Vltl_lang_token_kind *presumed_token_kind, const char *line
);
int vltl_lexer_token_tokenize(Vltl_lexer_token *dest, const char *src, size_t src_len, const Vltl_lang_token_kind token_kind);

#ifdef __cplusplus
}
#endif
