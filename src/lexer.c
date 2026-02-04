#include <lang/type.h>
#include <lang/token.h>
#include <lexer.h>

#include <string.h>
#include <errno.h>

bool vltl_lexer_line_valid(const Vltl_lexer_line line) {
    if(line.filename == NULL || line.function_name == NULL || line.text == NULL) {
        return false;
    }

    return true;
}

int vltl_lexer_token_init(
    Vltl_lexer_token *dest,
    size_t offset,
    size_t length,
    Vltl_lang_token_kind kind
) {
    if(!vltl_lang_token_kind_valid(kind)) {
        return EINVAL;
    }

    dest->offset = offset;
    dest->length = length;
    dest->kind = kind;

    return 0;
}

int vltl_lexer_line_convert(Vltl_lexer_line *dest, const char *src) {
    if(dest == NULL || src == NULL) {
        return EINVAL;
    }

    dest->text = src;

    dest->filename = "test.vltl";
    dest->function_name = "main";
    dest->linenumber = 0;
    memset(dest->tokens, 0, VLTL_LEXER_LINE_TOKENS_MAX * sizeof(Vltl_lexer_token));

    bool done = false;
    Vltl_lang_token_kind token_kind = VLTL_LANG_TOKEN_KIND_UNSET;
    size_t token_len = 0, token_offset = 0;
    size_t current_token_index = 0;
    while(!done && current_token_index < VLTL_LEXER_LINE_TOKENS_MAX) {
        switch(src[token_offset]) {
        case 0:
        case '\n':
            done = true;
            continue;
            break;
        case ' ':
            token_offset++;
            continue;
            break;
        case '-':
        case '+':
        case '*':
        case '/':
            token_kind = VLTL_LANG_TOKEN_KIND_OPERATION;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            token_kind = VLTL_LANG_TOKEN_KIND_LITERAL;
            break;
        default:
            token_kind = VLTL_LANG_TOKEN_KIND_SYMBOL;
            break;
        }

        const char *first_space = strchr(&(src[token_offset]), ' ');
        const char *first_newline = strchr(&(src[token_offset]), '\n');

        if(first_space == NULL && first_newline == NULL) {
            token_len = strlen(&(src[token_offset]));
            vltl_lexer_token_init(&(dest->tokens[current_token_index]), token_offset, token_len, token_kind);
            done = true;
        } else if (first_space != NULL && first_newline == NULL) {
            token_len = first_space - &(src[token_offset]);
            vltl_lexer_token_init(&(dest->tokens[current_token_index]), token_offset, token_len, token_kind);
        } else if (first_space == NULL && first_newline != NULL) {
            token_len = first_newline - &(src[token_offset]);
            vltl_lexer_token_init(&(dest->tokens[current_token_index]), token_offset, token_len, token_kind);
        } else if (first_space < first_newline) {
            token_len = first_space - &(src[token_offset]);
            vltl_lexer_token_init(&(dest->tokens[current_token_index]), token_offset, token_len, token_kind);
        } else if (first_newline < first_space) {
            token_len = first_newline - &(src[token_offset]);
            vltl_lexer_token_init(&(dest->tokens[current_token_index]), token_offset, token_len, token_kind);
        } else {
            return ENOTRECOVERABLE;
            break;
        }

        token_offset += token_len;
        current_token_index++;
    }

    dest->token_count = current_token_index;
    if(current_token_index >= VLTL_LEXER_LINE_TOKENS_MAX) {
        return EXFULL;
    }

    return 0;
}
