#include <ds/btrc.h>
#include <ds/iestack.h>
#include <ds/varena.h>
#include <global.h>
#include <lang/constant.h>
#include <lang/global.h>
#include <lang/literal.h>
#include <lang/local.h>
#include <lang/token.h>
#include <lang/type.h>
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
        IESTACK_PUSHF(
            &vltl_global_errors, EINVAL,
            "Argument pointer is null : dest = %p, src = %p",
            (void *) dest, (void *) src
        );
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
            IESTACK_PUSHF(
                &vltl_global_errors, EXFULL,
                "The lexer has processed %lu tokens! Unable to continue!", VLTL_LEXER_LINE_TOKENS_MAX
            );
            return EXFULL;
        }
        offset_into_line_buffer += end_of_current_line;

        ret = vltl_lexer_token_chomp(
                  &start_of_current_line, &end_of_current_line,
                  &presumed_token_kind, &(src[offset_into_line_buffer])
              );
        if(ret == ENODATA) {
            done = true;
            continue;
        } else if(ret == 0) {
            // keep going
        } else {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure!");
            return ret;
        }

        char *memory_for_this_token = varena_alloc(
                                          &vltl_global_allocator, end_of_current_line - start_of_current_line + 1
                                      );
        if(memory_for_this_token == NULL) {
            IESTACK_PUSH(&vltl_global_errors, ENOMEM, "Unable to allocate memory for copy of token substring!");
            return ENOMEM;
        }

        size_t len_of_current_line = 0;
        ret = btrc_strncpy(
                  &len_of_current_line,
                  memory_for_this_token, &(src[offset_into_line_buffer + start_of_current_line]),
                  end_of_current_line - start_of_current_line
              );
        if(ret) {
            return ret;
        }
        dest->tokens[current_token_index].line = memory_for_this_token;

        ret = vltl_lexer_token_tokenize(
                  &(dest->tokens[current_token_index++]), memory_for_this_token, end_of_current_line - start_of_current_line, presumed_token_kind
              );
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure!");
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
        IESTACK_PUSHF(
            &vltl_global_errors, EINVAL,
            "Arguments are NULL"
            " : "
            "start_of_next_token = %p, end_of_next_token = %p, presumed_token_kind = %p, line = %p\n",
            (void *) start_of_next_token, (void *) end_of_next_token, (void *) presumed_token_kind, (void *) line
        );
        return EINVAL;
    }

    // find start of next token
    bool done = false;
    while(!done) {
        switch(line[start_of_token + 0]) {
        case 0:
        case '\n':
            end_of_token = start_of_token + 0;
            // don't push error because enodata is a valid state
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
                IESTACK_PUSH(&vltl_global_errors, EINVAL, "Unsupported literal!");
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
            IESTACK_PUSH(&vltl_global_errors, EINVAL, "No idea what kind of token this might be!");
            return EINVAL;
        }
    }
    *end_of_next_token = end_of_token;

    return 0;
}

int vltl_lexer_token_tokenize(Vltl_lexer_token *dest, const char *src, size_t src_len, const Vltl_lang_token_kind token_kind) {
    const size_t tmp_cap = 999;
    char tmp[tmp_cap];

    if(dest == NULL || src == NULL) {
        IESTACK_PUSHF(
            &vltl_global_errors, EINVAL,
            "Arguments are invalid : dest = %p, src = %p!",
            (void *) dest, (void *) src
        );
        return EINVAL;
    }

    if(src_len == 0) {
        IESTACK_PUSH(&vltl_global_errors, EINVAL, "Length of src string is 0!");
        return EINVAL;
    }

    if(!vltl_lang_token_kind_valid(token_kind)) {
        IESTACK_PUSH(&vltl_global_errors, EINVAL, "Invalid token kind!");
        return EINVAL;
    }

    if((src_len - 1) > tmp_cap) {
        IESTACK_PUSH(&vltl_global_errors, EINVAL, "The src string is far too long!");
        return EINVAL;
    }

    // Use memcpy here to copy substring with token from string containing it and null terminate
    memcpy(tmp, src, src_len);
    tmp[src_len] = 0;

    if(token_kind == VLTL_LANG_TOKEN_KIND_LITERAL) {
        void *value = NULL;
        sscanf(tmp, "%li", (long *) &(value));

        dest->token.kind = VLTL_LANG_TOKEN_KIND_LITERAL;
        dest->token.literal = (Vltl_lang_literal) {
            .name = tmp,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .fields = { value }
        };

        return 0;
    }

    {
        Vltl_lang_operation *operation = NULL;
        int ret = nkht_get(vltl_global_table_operations, tmp, &operation);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_OPERATION;
            dest->token.operation = operation;

            return 0;
        } else if(token_kind == VLTL_LANG_TOKEN_KIND_OPERATION) {
            IESTACK_PUSHF(
                &vltl_global_errors, EINVAL,
                "Unable to find operation named %s when doing lookup!",
                tmp
            );
            return EINVAL;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            IESTACK_PUSHF(
                &vltl_global_errors, EINVAL,
                "Unable to find operation named %s when doing lookup!",
                tmp
            );
            return EINVAL;
        }
    }

    {
        Vltl_lang_type_attribute *attribute = NULL;
        int ret = nkht_get(vltl_global_table_attributes, tmp, &attribute);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_ATTRIBUTE;
            dest->token.attribute = attribute;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "Unexpected failure!");
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_type *type = NULL;
        int ret = nkht_get(vltl_global_table_types, tmp, &type);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_TYPE;
            dest->token.type = type;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "Unexpected failure!");
            return ENOTRECOVERABLE;
        }
    }

    {
        if(vltl_global_context.function != NULL) {
            Vltl_lang_local *local = NULL;
            int ret = vltl_lang_function_local_get(&local, vltl_global_context.function, tmp);
            if(ret == 0) {
                dest->token.kind = VLTL_LANG_TOKEN_KIND_LOCAL;
                dest->token.local = local;

                return 0;
            } else if(ret == ENODATA) {
                // keep going
            } else {
                IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "Unexpected failure!");
                return ENOTRECOVERABLE;
            }
        }
    }

    {
        Vltl_lang_global *global = NULL;
        int ret = nkht_get(vltl_global_table_globals, tmp, &global);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_GLOBAL;
            dest->token.global = global;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "Unexpected failure!");
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_constant *constant = NULL;
        int ret = nkht_get(vltl_global_table_constants, tmp, &constant);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_CONSTANT;
            dest->token.constant = constant;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "Unexpected failure!");
            return ENOTRECOVERABLE;
        }
    }

    {
        Vltl_lang_function *function = NULL;
        int ret = nkht_get(vltl_global_table_functions, tmp, &function);
        if(ret == 0) {
            dest->token.kind = VLTL_LANG_TOKEN_KIND_FUNCTION;
            dest->token.function = function;

            return 0;
        } else if(ret == ENODATA) {
            // keep going
        } else {
            IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "Unexpected failure!");
            return ENOTRECOVERABLE;
        }
    }

    dest->token.kind = VLTL_LANG_TOKEN_KIND_UNKNOWN;
    dest->token.unknown = src;

    return 0;
}
