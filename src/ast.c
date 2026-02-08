#include <global.h>
#include <ast.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>

bool vltl_ast_operation_precedence_order_valid(const Vltl_ast_operation_precedence_order precedence_order) {
    switch(precedence_order) {
    case VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT:
    case VLTL_AST_OPERATION_PRECEDENCE_ORDER_RIGHT_TO_LEFT:
        return true;
        break;
    default:
        return false;
        break;
    }

    return false;
}

bool vltl_ast_operation_precedence_valid(const Vltl_ast_operation_precedence operation_precedence) {
    switch(operation_precedence) {
    case VLTL_AST_OPERATION_PRECEDENCE_0:
    case VLTL_AST_OPERATION_PRECEDENCE_1:
    case VLTL_AST_OPERATION_PRECEDENCE_2:
    case VLTL_AST_OPERATION_PRECEDENCE_3:
    case VLTL_AST_OPERATION_PRECEDENCE_4:
    case VLTL_AST_OPERATION_PRECEDENCE_5:
    case VLTL_AST_OPERATION_PRECEDENCE_6:
    case VLTL_AST_OPERATION_PRECEDENCE_7:
    case VLTL_AST_OPERATION_PRECEDENCE_8:
    case VLTL_AST_OPERATION_PRECEDENCE_9:
    case VLTL_AST_OPERATION_PRECEDENCE_10:
    case VLTL_AST_OPERATION_PRECEDENCE_11:
    case VLTL_AST_OPERATION_PRECEDENCE_12:
    case VLTL_AST_OPERATION_PRECEDENCE_13:
    case VLTL_AST_OPERATION_PRECEDENCE_14:
    case VLTL_AST_OPERATION_PRECEDENCE_15:
    case VLTL_AST_OPERATION_PRECEDENCE_16:
    case VLTL_AST_OPERATION_PRECEDENCE_17:
    case VLTL_AST_OPERATION_PRECEDENCE_18:
    case VLTL_AST_OPERATION_PRECEDENCE_19:
    case VLTL_AST_OPERATION_PRECEDENCE_20:
        return true;
        break;
    default:
        return false;
        break;
    }

    return false;
}

int vltl_ast_operation_precedence_order_determine(
    Vltl_ast_operation_precedence_order *dest, const Vltl_ast_operation_precedence src
) {
    if(dest == NULL || !vltl_ast_operation_precedence_valid(src)) {
        return EINVAL;
    }

    // In the interest of portability and producing an expected result the C operator precedence rules are used.
    // Important to note this operator precedence ordering only mirrors C between precedences 1 to 15 inclusive.
    // Thus, this operator precedence acts as a superset of what is found in the C programming language!
    //
    // See:
    // https://en.cppreference.com/w/c/language/operator_precedence.html
    switch(src) {
    case VLTL_AST_OPERATION_PRECEDENCE_0:
    case VLTL_AST_OPERATION_PRECEDENCE_1:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT;
        break;
    case VLTL_AST_OPERATION_PRECEDENCE_2:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_RIGHT_TO_LEFT;
        break;
    case VLTL_AST_OPERATION_PRECEDENCE_3:
    case VLTL_AST_OPERATION_PRECEDENCE_4:
    case VLTL_AST_OPERATION_PRECEDENCE_5:
    case VLTL_AST_OPERATION_PRECEDENCE_6:
    case VLTL_AST_OPERATION_PRECEDENCE_7:
    case VLTL_AST_OPERATION_PRECEDENCE_8:
    case VLTL_AST_OPERATION_PRECEDENCE_9:
    case VLTL_AST_OPERATION_PRECEDENCE_10:
    case VLTL_AST_OPERATION_PRECEDENCE_11:
    case VLTL_AST_OPERATION_PRECEDENCE_12:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT;
        break;
    case VLTL_AST_OPERATION_PRECEDENCE_13:
    case VLTL_AST_OPERATION_PRECEDENCE_14:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_RIGHT_TO_LEFT;
        break;
    case VLTL_AST_OPERATION_PRECEDENCE_15:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT;
        break;
    default:
        return EINVAL;
    }

    return 0;
}

int vltl_ast_operation_precedence_determine(Vltl_ast_operation_precedence *dest, const Vltl_ast_operation src) {
    Vltl_ast_operation_precedence determined_precedence = { 0 };
    if(dest == NULL || !vltl_ast_operation_valid(src)) {
        return EINVAL;
    }

    switch(src.kind) {
    case VLTL_AST_OPERATION_KIND_EVAL:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_0;
        break;
    case VLTL_AST_OPERATION_KIND_ENCLOSE:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_1;
        break;
    case VLTL_AST_OPERATION_KIND_MUL:
    case VLTL_AST_OPERATION_KIND_DIV:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_3;
        break;
    case VLTL_AST_OPERATION_KIND_ADD:
    case VLTL_AST_OPERATION_KIND_SUB:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_4;
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_14;
        break;
    case VLTL_AST_OPERATION_KIND_DEF:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_16;
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_16;
        break;
    default:
        return EINVAL;
        break;
    }

    *dest = determined_precedence;
    return 0;
}

int vltl_ast_operation_copy(Vltl_ast_operation *dest, const Vltl_ast_operation *src) {
    if(dest == NULL || src == NULL) {
        return EINVAL;
    }

    memcpy(dest, src, sizeof(Vltl_ast_operation));
    for(size_t i = 0; i < vltl_ast_operation_argc(*dest); i++) {
        dest->arguments[i]->parent = dest;
    }

    return 0;
}

int vltl_ast_operation_kind_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_ast_operation_kind src
) {
    const char *src_string = NULL;

    if(dest == NULL || dest_cap == 0) {
        return EINVAL;
    }

    switch(src) {
    case VLTL_AST_OPERATION_KIND_ENCLOSE:
        src_string = "()";
        break;
    case VLTL_AST_OPERATION_KIND_ADD:
        src_string = "+";
        break;
    case VLTL_AST_OPERATION_KIND_SUB:
        src_string = "-";
        break;
    case VLTL_AST_OPERATION_KIND_MUL:
        src_string = "*";
        break;
    case VLTL_AST_OPERATION_KIND_DIV:
        src_string = "/";
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        src_string = "=";
        break;
    case VLTL_AST_OPERATION_KIND_DEF:
        src_string = "def";
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        src_string = "global";
        break;
    case VLTL_AST_OPERATION_KIND_EVAL:
        src_string = "EVAL";
        break;
    default:
        src_string = "???";
        break;
    }

    if(src_string == NULL) {
        return EINVAL;
    }
    size_t src_string_len = strlen(src_string);
    if(dest_cap < (src_string_len + 1)) {
        return ENOMEM;
    }
    memcpy(dest, src_string, src_string_len);
    dest[src_string_len] = 0;

    *dest_len = src_string_len + 1;
    return 0;
}

int vltl_ast_operation_detokenize(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_ast_operation src) {
    int ret;
    const size_t buf_cap = 99;
    size_t buf_len = 0;
    char buf_for_kind[buf_cap];
    char buf_for_evaluates_to[buf_cap];
    int dest_len_helper;
    if(dest == NULL || dest_cap == 0) {
        return EINVAL;
    }

    ret = vltl_ast_operation_kind_detokenize(buf_for_kind, buf_cap, &buf_len, src.kind);
    if(ret != 0) {
        return ret;
    }

    if(src.evaluates_to == NULL) {
        memcpy(buf_for_evaluates_to, "???", 3 + 1);
    } else {
        ret = vltl_lang_token_stringify(buf_for_evaluates_to, buf_cap, &buf_len, *(src.evaluates_to));
        if(ret != 0) {
            return ret;
        }
    }

    dest_len_helper = snprintf(
                          dest, dest_cap, "%s\\n%s\\n%s",
                          buf_for_kind, buf_for_evaluates_to,
                          src.result_type != NULL ? src.result_type->name : "NULL"
                      );
    if(dest_len_helper < 0) {
        return ENOTRECOVERABLE;
    }

    *dest_len = (size_t) dest_len_helper;
    return 0;
}

static int vltl_ast_tree_detokenize_recurse(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_ast_operation operation,
    size_t *monotonic_index,
    size_t parent_index
) {
    int ret = 0;
    size_t dest_offset = 0, dest_len_helper = 0, initial_value_of_monotonic_index = *monotonic_index;
    int dest_len_helper2 = 0;
    if(dest == NULL || dest_cap == 0 || dest_len == NULL) {
        return EINVAL;
    }

    dest_len_helper2 = snprintf(
                           &(dest[dest_offset]), dest_cap - dest_offset,
                           "node_%lu [label=\"", *monotonic_index
                       );
    if(dest_len_helper2 < 0) {
        return ENOTRECOVERABLE;
    }
    dest_offset += (size_t) dest_len_helper2;
    dest_cap -= (size_t) dest_len_helper2;

    ret = vltl_ast_operation_detokenize(&(dest[dest_offset]), dest_cap - dest_offset, &dest_len_helper, operation);
    if(ret != 0) {
        return ret;
    }
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    dest_len_helper2 = snprintf(&(dest[dest_offset]), dest_cap - dest_offset, "\"];\n");
    if(dest_len_helper2 < 0) {
        return ENOTRECOVERABLE;
    }
    dest_offset += (size_t) dest_len_helper2;
    dest_cap -= (size_t) dest_len_helper2;

    if(initial_value_of_monotonic_index == parent_index) {
        // this is the root node, don't draw vector
    } else {
        dest_len_helper2 = snprintf(&(dest[dest_offset]), dest_cap, "node_%lu -> node_%lu;\n", parent_index, *monotonic_index);
        if(dest_len_helper2 < 0) {
            return ENOTRECOVERABLE;
        }
        dest_offset += (size_t) dest_len_helper2;
        dest_cap -= (size_t) dest_len_helper2;
    }

    *monotonic_index += 1;
    for(size_t i = 0; i < VLTL_AST_OPERATION_ARGUMENTS_MAX; i++) {
        if(operation.arguments[i] == NULL) {
            break;
        }

        ret = vltl_ast_tree_detokenize_recurse(
                  &(dest[dest_offset]),
                  dest_cap - dest_offset,
                  &dest_len_helper,
                  *(operation.arguments[i]),
                  monotonic_index,
                  initial_value_of_monotonic_index
              );
        if(ret != 0) {
            return ret;
        }
        dest_offset += dest_len_helper;
        dest_cap -= dest_len_helper;
    }

    *dest_len = dest_offset;
    return 0;
}

int vltl_ast_tree_detokenize(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_ast_tree src) {
    size_t dest_offset = 0, dest_len_helper = 0, monotonic_index = 0;
    int dest_len_helper2 = 0;
    int ret = 0;

    dest_len_helper2 = snprintf(&(dest[dest_offset]), dest_cap, "digraph ast_tree {\n");
    if(dest_len_helper2 < 0) {
        return ENOTRECOVERABLE;
    }
    dest_offset += (size_t) dest_len_helper2;
    dest_cap -= (size_t) dest_len_helper2;

    size_t initial_value_of_monotonic_index = 0;
    ret = vltl_ast_tree_detokenize_recurse(
              &(dest[dest_offset]),
              dest_cap - dest_offset,
              &dest_len_helper,
              *(src.root),
              &monotonic_index,
              initial_value_of_monotonic_index
          );
    if(ret != 0) {
        return ret;
    }
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    dest_len_helper2 = snprintf(&(dest[dest_offset]), dest_cap, "}\n");
    if(dest_len_helper2 < 0) {
        return ENOTRECOVERABLE;
    }
    dest_offset += (size_t) dest_len_helper2;
    dest_cap -= (size_t) dest_len_helper2;

    *dest_len = dest_offset + 1;
    return 0;
}

bool vltl_ast_operation_kind_valid(const Vltl_ast_operation_kind operation_kind) {
    switch(operation_kind) {
    case VLTL_AST_OPERATION_KIND_ENCLOSE:
    case VLTL_AST_OPERATION_KIND_ADD:
    case VLTL_AST_OPERATION_KIND_MUL:
    case VLTL_AST_OPERATION_KIND_DIV:
    case VLTL_AST_OPERATION_KIND_EVAL:
    case VLTL_AST_OPERATION_KIND_SUB:
    case VLTL_AST_OPERATION_KIND_EQUALS:
    case VLTL_AST_OPERATION_KIND_DEF:
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        break;
    default:
        return false;
        break;
    }

    return true;
}

bool vltl_ast_operation_valid(const Vltl_ast_operation operation) {
    if(!vltl_ast_operation_kind_valid(operation.kind) || operation.result_type == NULL) {
        return false;
    }

    return true;
}

size_t vltl_ast_operation_argc(const Vltl_ast_operation operation) {
    size_t i = 0;
    while(operation.arguments[i] != NULL && i < VLTL_AST_OPERATION_ARGUMENTS_MAX) {
        i++;
    }

    return i;
}

size_t vltl_ast_operation_kind_argc(const Vltl_ast_operation_kind operation_kind) {
    switch(operation_kind) {
    case VLTL_AST_OPERATION_KIND_ENCLOSE:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_ADD:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_MUL:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_DIV:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_DEF:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_EVAL:
        return 0;
        break;
    case VLTL_AST_OPERATION_KIND_SUB:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        return 2;
        break;
    default:
        return 0;
        break;
    }

    return 0;
}

int vltl_ast_operation_init(
    Vltl_ast_operation *operation,
    Vltl_ast_operation_kind kind,
    Vltl_lang_token *evaluates_to,
    const Vltl_lang_type *result_type
) {
    if(operation == NULL || !vltl_ast_operation_kind_valid(kind) || result_type == NULL) {
        return EINVAL;
    }

    memset(operation, 0, sizeof(Vltl_ast_operation));
    operation->kind = kind;
    operation->evaluates_to = evaluates_to;
    operation->result_type = result_type;

    return 0;
}

int vltl_ast_operation_insert(
    Vltl_ast_tree *tree,
    Vltl_ast_operation *parent,
    Vltl_ast_operation *new_child,
    size_t new_child_index
) {
    if(
        tree == NULL ||
        (parent != NULL && !vltl_ast_operation_valid(*parent)) ||
        new_child == NULL || !vltl_ast_operation_valid(*new_child)
    ) {
        return EINVAL;
    }

    // rearrange nodes themselves
    {
        if(parent == NULL) {
            if(tree->root != NULL) {
                return EINVAL;
            }

            tree->root = new_child;
        } else {
            parent->arguments[new_child_index] = new_child;
        }
        new_child->parent = parent;
        new_child->belongs_to = tree;
    }

    // vltl_ast_operation specific logic
    {
    }

    return 0;
}

int vltl_ast_operation_adopt(
    Vltl_ast_tree *tree,
    Vltl_ast_operation *new_parent,
    Vltl_ast_operation *adopt_this
) {
    if(
        tree == NULL ||
        new_parent == NULL || !vltl_ast_operation_valid(*new_parent) ||
        adopt_this == NULL || !vltl_ast_operation_valid(*adopt_this)
    ) {
        return EINVAL;
    }

    // rearrange nodes themselves
    {
        // append adopt_this to the arguments of new_parent.
        {
            const size_t first_empty_pos = vltl_ast_operation_argc(*new_parent);
            new_parent->arguments[first_empty_pos] = adopt_this;
        }

        // adopt_this->parent[index of adopt_this in parent arguments] = new_parent
        {
            Vltl_ast_operation **parent_pointer_to_adopt_this = NULL;
            if(adopt_this->parent == NULL) {
                tree->root = new_parent;
            } else {
                for(size_t i = 0; i < VLTL_AST_OPERATION_ARGUMENTS_MAX; i++) {
                    if(adopt_this->parent->arguments[i] == adopt_this) {
                        parent_pointer_to_adopt_this = &(adopt_this->parent->arguments[i]);
                        break;
                    }
                }
                if(parent_pointer_to_adopt_this == NULL) {
                    // The parent cannot hold this additional children.
                    return EXFULL;
                }
                *parent_pointer_to_adopt_this = new_parent;
            }
        }

        // update parent reference
        {
            new_parent->parent = adopt_this->parent;
            adopt_this->parent = new_parent;
        }

        tree->last = new_parent;
    }

    // vltl_ast_operation specific logic
    {
    }

    return 0;
}

int vltl_ast_tree_insert(Vltl_ast_tree *tree, Vltl_ast_operation *pushed) {
    Vltl_ast_operation *target = NULL, *possible_target;

    int ret = 0;
    if(tree == NULL || pushed == NULL || !vltl_ast_operation_valid(*pushed)) {
        return EINVAL;
    }

    if(tree->root == NULL) {
        ret = vltl_ast_operation_insert(tree, NULL, pushed, 0);
        if(ret != 0) {
            return ret;
        }

        tree->last = pushed;
        return 0;
    }

    if(tree->root == NULL || tree->last == NULL) {
        return ENOTRECOVERABLE;
    }

    Vltl_ast_operation_precedence pushed_precedence = {0}, tree_last_precedence = {0};
    ret = vltl_ast_operation_precedence_determine(&pushed_precedence, *pushed);
    if(ret) {
        return ret;
    }
    ret = vltl_ast_operation_precedence_determine(&tree_last_precedence, *(tree->last));
    if(ret) {
        return ret;
    }
    if(pushed_precedence <= tree_last_precedence) {
        ret = vltl_ast_operation_insert(
                  tree, tree->last, pushed,
                  vltl_ast_operation_argc(*(tree->last))
              );
        if(ret != 0) {
            return ret;
        }

        tree->last = pushed;
        return 0;
    }

    bool need_to_displace = false;
    for(possible_target = tree->last;
            possible_target != NULL && possible_target->parent != NULL;
            possible_target = possible_target->parent
       ) {
        Vltl_ast_operation_precedence parent_precedence = {0};
        ret = vltl_ast_operation_precedence_determine(&pushed_precedence, *pushed);
        if(ret) {
            return ret;
        }
        ret = vltl_ast_operation_precedence_determine(&parent_precedence, *(possible_target->parent));
        if(ret) {
            return ret;
        }

        Vltl_ast_operation_precedence precedence = { 0 };
        Vltl_ast_operation_precedence_order precedence_order = { 0 };
        bool parent_lower_than_pushed = { 0 };
        ret = vltl_ast_operation_precedence_determine(&precedence, *possible_target);
        if(ret != 0) {
            return ret;
        }
        ret = vltl_ast_operation_precedence_order_determine(&precedence_order, precedence);
        if(ret != 0) {
            return ret;
        }
        switch(precedence_order) {
        case VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT:
            parent_lower_than_pushed = pushed_precedence >= parent_precedence;
            break;
        case VLTL_AST_OPERATION_PRECEDENCE_ORDER_RIGHT_TO_LEFT:
            parent_lower_than_pushed = pushed_precedence > parent_precedence;
            break;
        default:
            return ENOTRECOVERABLE;
            break;
        }

        if(parent_lower_than_pushed) {
            // always need to travel up because cannot insert here
        } else {
            need_to_displace = true;

            if(!parent_lower_than_pushed) {
                target = possible_target;
                break;
            }
        }
    }

    if(target == NULL) {
        ret = vltl_ast_operation_adopt(tree, pushed, tree->root);
        if(ret != 0) {
            return ret;
        }
    } else if(need_to_displace) {
        ret = vltl_ast_operation_adopt(tree, pushed, target);
        if(ret != 0) {
            return ret;
        }
    } else {
        return EINVAL;
    }

    return 0;
}

int vltl_ast_tree_convert(Vltl_ast_tree *dest, Vltl_lexer_line *src) {
    Vltl_ast_operation *push_this = NULL;
    Vltl_ast_operation_kind operation_kind = VLTL_AST_OPERATION_KIND_UNSET;
    Vltl_lang_token *evaluates_to = NULL;
    const Vltl_lang_type *result_type = NULL;
    int ret = 0;
    if(dest == NULL || src == NULL || !vltl_lexer_line_valid(*src)) {
        return EINVAL;
    }

    memset(dest, 0, sizeof(Vltl_ast_tree));
    for(size_t i = 0; i < src->token_count; i++) {
        push_this = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_ast_operation));
        if(!push_this) {
            return ENOMEM;
        }

        switch(src->tokens[i].token.kind) {
        case VLTL_LANG_TOKEN_KIND_ATTRIBUTE:
        case VLTL_LANG_TOKEN_KIND_TYPE:
            // not implemented yet
            return ENOTRECOVERABLE;
            break;
        case VLTL_LANG_TOKEN_KIND_CONSTANT:
            operation_kind = VLTL_AST_OPERATION_KIND_EVAL;
            evaluates_to = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_token));
            if(evaluates_to == NULL) {
                return ENOMEM;
            }

            evaluates_to->kind = VLTL_LANG_TOKEN_KIND_CONSTANT;
            evaluates_to->constant = src->tokens[i].token.constant;
            result_type = &vltl_lang_type_long;
            ret = vltl_ast_operation_init(push_this, operation_kind, evaluates_to, result_type);
            break;
        case VLTL_LANG_TOKEN_KIND_LOCAL:
            operation_kind = VLTL_AST_OPERATION_KIND_EVAL;
            evaluates_to = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_token));
            if(evaluates_to == NULL) {
                return ENOMEM;
            }

            evaluates_to->kind = VLTL_LANG_TOKEN_KIND_LOCAL;
            evaluates_to->local = src->tokens[i].token.local;
            result_type = &vltl_lang_type_long;
            ret = vltl_ast_operation_init(push_this, operation_kind, evaluates_to, result_type);
            break;
        case VLTL_LANG_TOKEN_KIND_GLOBAL:
            operation_kind = VLTL_AST_OPERATION_KIND_EVAL;
            evaluates_to = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_token));
            if(evaluates_to == NULL) {
                return ENOMEM;
            }

            evaluates_to->kind = VLTL_LANG_TOKEN_KIND_GLOBAL;
            evaluates_to->global = src->tokens[i].token.global;
            result_type = &vltl_lang_type_long;
            ret = vltl_ast_operation_init(push_this, operation_kind, evaluates_to, result_type);
            break;
        case VLTL_LANG_TOKEN_KIND_LITERAL:
            operation_kind = VLTL_AST_OPERATION_KIND_EVAL;
            evaluates_to = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_token));
            if(evaluates_to == NULL) {
                return ENOMEM;
            }

            evaluates_to->kind = VLTL_LANG_TOKEN_KIND_LITERAL;
            evaluates_to->literal = src->tokens[i].token.literal;
            result_type = &vltl_lang_type_long;
            ret = vltl_ast_operation_init(push_this, operation_kind, evaluates_to, result_type);
            break;
        case VLTL_LANG_TOKEN_KIND_OPERATION:
            switch(src->tokens[i].token.operation->operation_kind) {
            case VLTL_LANG_OPERATION_KIND_GLOBAL:
                operation_kind = VLTL_AST_OPERATION_KIND_GLOBAL;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_ADD:
                operation_kind = VLTL_AST_OPERATION_KIND_ADD;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_SUB:
                operation_kind = VLTL_AST_OPERATION_KIND_SUB;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_EQUALS:
                operation_kind = VLTL_AST_OPERATION_KIND_EQUALS;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            default:
                return EINVAL;
                break;
            }

            break;
        case VLTL_LANG_TOKEN_KIND_UNKNOWN:
            operation_kind = VLTL_AST_OPERATION_KIND_EVAL;
            evaluates_to = &(src->tokens[i].token);
            result_type = &vltl_lang_type_long;

            ret = vltl_ast_operation_init(push_this, operation_kind, evaluates_to, result_type);
            break;
        default:
            return EINVAL;
            break;
        }

        ret = vltl_ast_tree_insert(dest, push_this);
        if(ret != 0) {
            return ret;
        }
    }

    return 0;
}
