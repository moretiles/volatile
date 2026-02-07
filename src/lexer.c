#include <lang/type.h>
#include <lang/token.h>
#include <lexer.h>
#include <ds/varena.h>
#include <global.h>
#include <lang/literal.h>
#include <lang/constant.h>
#include <lang/global.h>
#include <lang/local.h>

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
    const char *line,
    Vltl_lang_token token
) {
    if(!vltl_lang_token_kind_valid(token.kind)) {
        return EINVAL;
    }

    dest->line = line;
    dest->token = token;

    return 0;
}

int vltl_lexer_line_convert(Vltl_lexer_line *dest, const char *src) {
    int ret = 0;
    if(dest == NULL || src == NULL) {
        return EINVAL;
    }

    dest->text = src;

    dest->filename = "test.vltl";
    dest->function_name = "main";
    dest->linenumber = 0;
    memset(dest->tokens, 0, VLTL_LEXER_LINE_TOKENS_MAX * sizeof(Vltl_lexer_token));

    Vltl_lang_token_kind presumed_token_kind = { 0 };
    size_t start_of_current_line, end_of_current_line = 0, offset_into_line_buffer = 0;
    size_t current_token_index = 0;
    bool done = false;
    while(!done) {
        if(current_token_index >= VLTL_LEXER_LINE_TOKENS_MAX) {
            return EXFULL;
        }
        offset_into_line_buffer += end_of_current_line;

        ret = vltl_lexer_token_chomp(
                  &start_of_current_line, &end_of_current_line, &presumed_token_kind, &(src[offset_into_line_buffer])
              );
        if(ret == ENODATA) {
            done = true;
            continue;
        } else if(ret == 0) {
            // keep going
        } else {
            return ret;
        }

        char *memory_for_this_token = varena_alloc(
                                          &vltl_global_allocator, end_of_current_line - start_of_current_line + 1
                                      );
        if(memory_for_this_token == NULL) {
            return ENOMEM;
        }
        memcpy(
            memory_for_this_token,
            &(src[offset_into_line_buffer + start_of_current_line]),
            end_of_current_line - start_of_current_line
        );
        memory_for_this_token[offset_into_line_buffer + end_of_current_line - start_of_current_line] = 0;
        dest->tokens[current_token_index].line = memory_for_this_token;

        ret = vltl_lexer_token_tokenize(
                  &(dest->tokens[current_token_index++]), memory_for_this_token, presumed_token_kind
              );
        if(ret) {
            return ret;
        }
    }

    dest->token_count = current_token_index;
    return 0;
}

int vltl_lexer_token_chomp(
    size_t *start_of_next_token, size_t *end_of_next_token,
    Vltl_lang_token_kind *presumed_token_kind, const char *line
) {
    size_t start_of_token = 0, end_of_token = 0;
    if(start_of_next_token == NULL || end_of_next_token == NULL || presumed_token_kind == NULL || line == NULL) {
        return EINVAL;
    }

    // find start of next token
    bool done = false;
    while(!done) {
        switch(line[start_of_token + 0]) {
        case 0:
        case '\n':
            end_of_token = start_of_token + 0;
            return ENODATA;
            break;
        case ' ':
            start_of_token++;
            continue;
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
        case '\'':
        case '\"':
            *presumed_token_kind = VLTL_LANG_TOKEN_KIND_LITERAL;
            done = true;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
            *presumed_token_kind = VLTL_LANG_TOKEN_KIND_UNKNOWN;
            done = true;
            break;
        default:
            *presumed_token_kind = VLTL_LANG_TOKEN_KIND_OPERATION;
            done = true;
            break;
        }
    }
    *start_of_next_token = start_of_token;
    end_of_token = start_of_token;

    // find end of next token
    done = false;
    while(!done) {
        if(*presumed_token_kind == VLTL_LANG_TOKEN_KIND_LITERAL) {
            switch(line[end_of_token]) {
            case '"':
            case '\'':
            case '.':
                // not supported yet
                return EINVAL;
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
                end_of_token++;
                break;
            default:
                done = true;
                break;
            }
        } else if(*presumed_token_kind == VLTL_LANG_TOKEN_KIND_OPERATION) {
            switch(line[end_of_token]) {
            case 0:
            case '\n':
            case ' ':
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
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
                done = true;
                break;
            default:
                end_of_token++;
                break;
            }
        } else if(*presumed_token_kind == VLTL_LANG_TOKEN_KIND_UNKNOWN) {
            switch(line[end_of_token]) {
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
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
                end_of_token++;
                break;
            default:
                done = true;
                break;
            }
        } else {
            return EINVAL;
        }
    }
    *end_of_next_token = end_of_token;

    return 0;
}

int vltl_lexer_token_tokenize(Vltl_lexer_token *dest, const char *src, const Vltl_lang_token_kind token_kind) {
    if(token_kind == VLTL_LANG_TOKEN_KIND_LITERAL) {
        void *value = NULL;
        sscanf(src, "%li", (long *) &(value));

        dest->token.kind = VLTL_LANG_TOKEN_KIND_LITERAL;
        dest->token.literal = (Vltl_lang_literal) {
            .name = src,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .fields = { value }
        };

        return 0;
    }

    if(token_kind == VLTL_LANG_TOKEN_KIND_OPERATION) {
        Vltl_lang_operation *operation = NULL;
        int ret = nkht_get(vltl_global_table_operations, src, &operation);
        if(ret) {
            return EINVAL;
        }

        dest->token.kind = VLTL_LANG_TOKEN_KIND_OPERATION;
        dest->token.operation = operation;
        return 0;
    }

    {
        Vltl_lang_type_attribute *attribute = NULL;
        int ret = nkht_get(vltl_global_table_attributes, src, &attribute);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_ATTRIBUTE;
            dest->token.attribute = attribute;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_type *type = NULL;
        int ret = nkht_get(vltl_global_table_types, src, &type);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_TYPE;
            dest->token.type = type;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_local *local = NULL;
        int ret = nkht_get(vltl_global_table_locals, src, &local);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_LOCAL;
            dest->token.local = local;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_global *global = NULL;
        int ret = nkht_get(vltl_global_table_globals, src, &global);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_GLOBAL;
            dest->token.global = global;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_constant *constant = NULL;
        int ret = nkht_get(vltl_global_table_constants, src, &constant);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_CONSTANT;
            dest->token.constant = constant;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            return ENOTRECOVERABLE;
        }
    }

    dest->token.kind = VLTL_LANG_TOKEN_KIND_UNKNOWN;
    dest->token.unknown = src;

    return 0;
}
