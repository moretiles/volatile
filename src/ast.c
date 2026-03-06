#include <ds/btrc.h>
#include <ds/iestack.h>
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
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT;
        break;
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
    case VLTL_AST_OPERATION_PRECEDENCE_16:
    case VLTL_AST_OPERATION_PRECEDENCE_17:
    case VLTL_AST_OPERATION_PRECEDENCE_18:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT;
        break;
    case VLTL_AST_OPERATION_PRECEDENCE_19:
    case VLTL_AST_OPERATION_PRECEDENCE_20:
        *dest = VLTL_AST_OPERATION_PRECEDENCE_ORDER_RIGHT_TO_LEFT;
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
    case VLTL_AST_OPERATION_KIND_BODY_OPEN:
    case VLTL_AST_OPERATION_KIND_BODY_CLOSE:
    case VLTL_AST_OPERATION_KIND_GROUPING_CLOSE:
    case VLTL_AST_OPERATION_KIND_GROUPING_OPEN:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_0;
        break;
    case VLTL_AST_OPERATION_KIND_CALL:
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
    case VLTL_AST_OPERATION_KIND_TEST_EQUALS:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_7;
        break;
    case VLTL_AST_OPERATION_KIND_TYPEAS:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_13;
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_14;
        break;
    case VLTL_AST_OPERATION_KIND_COMMA:
    case VLTL_AST_OPERATION_KIND_CSV:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_15;
        break;
    case VLTL_AST_OPERATION_KIND_RETURN:
    case VLTL_AST_OPERATION_KIND_CONSTANT:
    case VLTL_AST_OPERATION_KIND_GLOBAL:
    case VLTL_AST_OPERATION_KIND_LOCAL:
    case VLTL_AST_OPERATION_KIND_FUNCTION:
    case VLTL_AST_OPERATION_KIND_IF:
    case VLTL_AST_OPERATION_KIND_ELIF:
    case VLTL_AST_OPERATION_KIND_ELSE:
    case VLTL_AST_OPERATION_KIND_WHILE:
        determined_precedence = VLTL_AST_OPERATION_PRECEDENCE_19;
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
    case VLTL_AST_OPERATION_KIND_GROUPING_OPEN:
        src_string = "(";
        break;
    case VLTL_AST_OPERATION_KIND_GROUPING_CLOSE:
        src_string = ")";
        break;
    case VLTL_AST_OPERATION_KIND_CALL:
        src_string = "CALL";
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
    case VLTL_AST_OPERATION_KIND_TEST_EQUALS:
        src_string = "==";
        break;
    case VLTL_AST_OPERATION_KIND_TYPEAS:
        src_string = ":";
        break;
    case VLTL_AST_OPERATION_KIND_COMMA:
        src_string = ",";
        break;
    case VLTL_AST_OPERATION_KIND_CSV:
        src_string = "csv";
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        src_string = "=";
        break;
    case VLTL_AST_OPERATION_KIND_LOCAL:
        src_string = "local";
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        src_string = "global";
        break;
    case VLTL_AST_OPERATION_KIND_CONSTANT:
        src_string = "constant";
        break;
    case VLTL_AST_OPERATION_KIND_EVAL:
        src_string = "eval";
        break;
    case VLTL_AST_OPERATION_KIND_RETURN:
        src_string = "return";
        break;
    case VLTL_AST_OPERATION_KIND_FUNCTION:
        src_string = "function";
        break;
    case VLTL_AST_OPERATION_KIND_IF:
        src_string = "if";
        break;
    case VLTL_AST_OPERATION_KIND_ELIF:
        src_string = "elif";
        break;
    case VLTL_AST_OPERATION_KIND_ELSE:
        src_string = "else";
        break;
    case VLTL_AST_OPERATION_KIND_WHILE:
        src_string = "while";
        break;
    case VLTL_AST_OPERATION_KIND_BODY_OPEN:
        src_string = "{";
        break;
    case VLTL_AST_OPERATION_KIND_BODY_CLOSE:
        src_string = "}";
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
    int dest_len_helper = 0;
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

    BTRC_SNPRINTF(&ret, &dest_len_helper,
                  dest, dest_cap, "%s\\n%s\\n%s",
                  buf_for_kind, buf_for_evaluates_to,
                  src.result_type != NULL ? src.result_type->name : "NULL"
                 );
    if(ret) {
        return ret;
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

    if(dest == NULL || dest_len == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : dest = %p, dest_len = %p!",
            (void *) dest, (void *) dest_len
        );
        return ret;
    }

    if(dest_cap == 0) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "Whoops dest_cap is 0!");
        return ret;
    }

    BTRC_SNPRINTF(&ret, &dest_len_helper2,
                  &(dest[dest_offset]), dest_cap - dest_offset,
                  "node_%lu [label=\"", *monotonic_index
                 );
    //if(ret) { return ret; }
    VLTL_EXPECT(ret, "sprintf failed!");
    dest_offset += (size_t) dest_len_helper2;
    dest_cap -= (size_t) dest_len_helper2;

    VLTL_EXPECT(
        vltl_ast_operation_detokenize(&(dest[dest_offset]), dest_cap, &dest_len_helper, operation),
        "detokenization failed!"
    );
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    VLTL_EXPECT(btrc_strncpy(&dest_len_helper, &(dest[dest_offset]), "\"];\n", dest_cap), "strncpy failed");
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    if(initial_value_of_monotonic_index == parent_index) {
        // this is the root node, don't draw vector
    } else {
        BTRC_SNPRINTF(&ret, &dest_len_helper2, &(dest[dest_offset]), dest_cap, "node_%lu -> node_%lu;\n", parent_index, *monotonic_index);
        VLTL_EXPECT(ret, "sprintf failed!");

        dest_offset += (size_t) dest_len_helper2;
        dest_cap -= (size_t) dest_len_helper2;
    }

    *monotonic_index += 1;
    for(size_t i = 0; i < VLTL_AST_OPERATION_ARGUMENTS_MAX; i++) {
        if(operation.arguments[i] == NULL) {
            break;
        }

        VLTL_EXPECT(
            vltl_ast_tree_detokenize_recurse(
                &(dest[dest_offset]),
                dest_cap,
                &dest_len_helper,
                *(operation.arguments[i]),
                monotonic_index,
                initial_value_of_monotonic_index
            ),
            "recursive detokenization failed!"
        );
        dest_offset += dest_len_helper;
        dest_cap -= dest_len_helper;
    }

    *dest_len = dest_offset;
    return 0;
}

int vltl_ast_tree_detokenize(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_ast_tree src) {
    size_t dest_offset = 0, dest_len_helper = 0, monotonic_index = 0;

    VLTL_EXPECT(
        btrc_strncpy(&dest_len_helper, &(dest[dest_offset]), "digraph ast_tree {\n", dest_cap),
        "Unexpected failure copying string!"
    );
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    size_t initial_value_of_monotonic_index = 0;
    VLTL_EXPECT(
        vltl_ast_tree_detokenize_recurse(
            &(dest[dest_offset]),
            dest_cap - dest_offset,
            &dest_len_helper,
            *(src.root),
            &monotonic_index,
            initial_value_of_monotonic_index
        ),
        "Unexpected failure calling vltl_ast_tree_detokenize_recurse!"
    );
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    VLTL_EXPECT(
        btrc_strncpy(&dest_len_helper, &(dest[dest_offset]), "}\n", dest_cap),
        "Unexpected failure copying string!"
    );
    dest_offset += dest_len_helper;
    dest_cap -= dest_len_helper;

    *dest_len = dest_offset + 1;
    return 0;
}

bool vltl_ast_operation_kind_valid(const Vltl_ast_operation_kind operation_kind) {
    switch(operation_kind) {
    case VLTL_AST_OPERATION_KIND_GROUPING_OPEN:
    case VLTL_AST_OPERATION_KIND_GROUPING_CLOSE:
    case VLTL_AST_OPERATION_KIND_CALL:
    case VLTL_AST_OPERATION_KIND_ADD:
    case VLTL_AST_OPERATION_KIND_SUB:
    case VLTL_AST_OPERATION_KIND_MUL:
    case VLTL_AST_OPERATION_KIND_DIV:
    case VLTL_AST_OPERATION_KIND_TEST_EQUALS:
    case VLTL_AST_OPERATION_KIND_COMMA:
    case VLTL_AST_OPERATION_KIND_CSV:
    case VLTL_AST_OPERATION_KIND_TYPEAS:
    case VLTL_AST_OPERATION_KIND_EVAL:
    case VLTL_AST_OPERATION_KIND_EQUALS:
    case VLTL_AST_OPERATION_KIND_LOCAL:
    case VLTL_AST_OPERATION_KIND_GLOBAL:
    case VLTL_AST_OPERATION_KIND_CONSTANT:
    case VLTL_AST_OPERATION_KIND_RETURN:
    case VLTL_AST_OPERATION_KIND_FUNCTION:
    case VLTL_AST_OPERATION_KIND_IF:
    case VLTL_AST_OPERATION_KIND_ELIF:
    case VLTL_AST_OPERATION_KIND_ELSE:
    case VLTL_AST_OPERATION_KIND_WHILE:
    case VLTL_AST_OPERATION_KIND_BODY_OPEN:
    case VLTL_AST_OPERATION_KIND_BODY_CLOSE:
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

size_t vltl_ast_operation_expected_argc(const Vltl_ast_operation operation) {
    switch(operation.kind) {
    case VLTL_AST_OPERATION_KIND_GROUPING_OPEN:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_GROUPING_CLOSE:
        return 0;
        break;
    case VLTL_AST_OPERATION_KIND_CALL:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_ADD:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_SUB:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_MUL:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_DIV:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_TEST_EQUALS:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_TYPEAS:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_COMMA:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_CSV:
        return vltl_ast_operation_argc(operation);
        break;
    case VLTL_AST_OPERATION_KIND_LOCAL:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_CONSTANT:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_RETURN:
        return 1;
        break;
    case VLTL_AST_OPERATION_KIND_EVAL:
        return 0;
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_FUNCTION:
        return 3;
        break;
    case VLTL_AST_OPERATION_KIND_IF:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_ELIF:
        return 3;
        break;
    case VLTL_AST_OPERATION_KIND_ELSE:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_WHILE:
        return 2;
        break;
    case VLTL_AST_OPERATION_KIND_BODY_OPEN:
        return 0;
        break;
    case VLTL_AST_OPERATION_KIND_BODY_CLOSE:
        return 0;
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
    int ret = 0;
    if(tree == NULL || new_child == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : tree is %p, new_child is %p!",
            (void *) tree, (void *) new_child
        );
        return EINVAL;
    }

    if((parent != NULL && !vltl_ast_operation_valid(*parent))) {
        VLTL_RETURN(EINVAL, "Parent exists and is invalid!");
    }

    if(!vltl_ast_operation_valid(*new_child)) {
        VLTL_RETURN(EINVAL, "new_child exists and is invalid!");
    }

    // rearrange nodes themselves
    {
        if(parent == NULL) {
            if(tree->root != NULL) {
                VLTL_RETURN(EINVAL, "Parent is NULL in bad location!");
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
    int ret = 0;
    if(tree == NULL || new_parent == NULL || adopt_this == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : tree = %p, new_parent = %p, adopt_this = %p!",
            (void *) tree, (void *) new_parent, (void *) adopt_this
        );
        return ret;
    }

    if(!vltl_ast_operation_valid(*new_parent)) {
        VLTL_RETURN(EINVAL, "It looks like new_parent is invalid!");
    }

    if(!vltl_ast_operation_valid(*adopt_this)) {
        VLTL_RETURN(EINVAL, "It looks like adopt_this is invalid!");
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
                    VLTL_RETURN(EXFULL, "The parent cannot hold this additional children!");
                }
                *parent_pointer_to_adopt_this = new_parent;
            }
        }

        // update parent reference
        {
            new_parent->parent = adopt_this->parent;
            new_parent->belongs_to = adopt_this->belongs_to;
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
    if(tree == NULL || pushed == NULL) {
        IESTACK_RETURNF(
            &vltl_global_errors, EINVAL,
            "Arguments are NULL : tree = %p, pushed = %p!",
            (void *) tree, (void *) pushed
        );
    }

    if(!vltl_ast_operation_valid(*pushed)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "Pushed operation is invalid!");
        return ret;
    }

    if(tree->root == NULL) {
        VLTL_EXPECT(vltl_ast_operation_insert(tree, NULL, pushed, 0), "Unable to insert node into tree!");
        tree->last = pushed;
        return 0;
    }

    if(tree->root == NULL || tree->last == NULL) {
        VLTL_RETURN(ENOTRECOVERABLE, "dest tree has become invalid!");
    }

    Vltl_ast_operation_precedence pushed_precedence = {0}, tree_last_precedence = {0};
    VLTL_EXPECT(
        vltl_ast_operation_precedence_determine(&pushed_precedence, *pushed),
        "Bad precedence for pushed!"
    );
    VLTL_EXPECT(
        vltl_ast_operation_precedence_determine(&tree_last_precedence, *(tree->last)),
        "Bad precedence for tree->last!"
    );
    if(
        pushed_precedence <= tree_last_precedence &&
        vltl_ast_operation_argc(*tree->last) < vltl_ast_operation_expected_argc(*(tree->last))
    ) {
        // handle empty parenthesis ()
        if(
            tree->last->kind == VLTL_AST_OPERATION_KIND_GROUPING_OPEN &&
            vltl_ast_operation_argc(*(tree->last)) == 0 &&
            pushed->kind == VLTL_AST_OPERATION_KIND_GROUPING_CLOSE
        ) {
            Vltl_ast_operation *empty_csv_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_ast_operation));
            if(!empty_csv_operation) {
                return ENOMEM;
            }

            const Vltl_ast_operation_kind operation_kind = VLTL_AST_OPERATION_KIND_CSV;
            const Vltl_lang_type *result_type = &vltl_lang_type_long;
            VLTL_EXPECT(
                vltl_ast_operation_init(empty_csv_operation, operation_kind, NULL, result_type),
                "Unable to initalize empty CSV operation"
            );
            VLTL_EXPECT(
                vltl_ast_operation_insert(
                    tree, tree->last, empty_csv_operation,
                    vltl_ast_operation_argc(*(tree->last))
                ),
                "Unable to insert node into tree!"
            );
        }

        VLTL_EXPECT(
            vltl_ast_operation_insert(tree, tree->last, pushed, vltl_ast_operation_argc(*(tree->last))),
            "Unable to insert node into tree!"
        );
        tree->last = pushed;
        return 0;
    }

    if(
        vltl_ast_operation_argc(*pushed) == vltl_ast_operation_expected_argc(*(pushed)) ||
        pushed->kind == VLTL_AST_OPERATION_KIND_GROUPING_OPEN
    ) {
        bool done = false;
        possible_target = tree->last;
        while(!done && possible_target != NULL) {
            Vltl_ast_operation_precedence possible_target_precedence = { 0 };
            VLTL_EXPECT(
                vltl_ast_operation_precedence_determine(&possible_target_precedence, *possible_target),
                "Bad precedence for possible_target!"
            );

            if(
                pushed_precedence <= possible_target_precedence &&
                vltl_ast_operation_argc(*possible_target) < vltl_ast_operation_expected_argc(*(possible_target))
            ) {
                VLTL_EXPECT(
                    vltl_ast_operation_insert(
                        tree, possible_target, pushed,
                        vltl_ast_operation_argc(*possible_target)
                    ),
                    "Unable to insert node into tree!"
                );
                tree->last = pushed;
                return 0;
            }

            possible_target = possible_target->parent;
        }
    }

    bool need_to_displace = false;
    for(possible_target = tree->last;
            possible_target != NULL && possible_target->parent != NULL;
            possible_target = possible_target->parent
       ) {
        Vltl_ast_operation_precedence parent_precedence = {0};
        ret = vltl_ast_operation_precedence_determine(&pushed_precedence, *pushed);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Bad precedence for pushed!");
            return ret;
        }
        ret = vltl_ast_operation_precedence_determine(&parent_precedence, *(possible_target->parent));
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Bad precedence for possible_target->parent!");
            return ret;
        }

        Vltl_ast_operation_precedence precedence = { 0 };
        Vltl_ast_operation_precedence_order precedence_order = { 0 };
        bool parent_lower_than_pushed = { 0 };
        ret = vltl_ast_operation_precedence_determine(&precedence, *possible_target);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Bad precedence for possible_target!");
            return ret;
        }
        ret = vltl_ast_operation_precedence_order_determine(&precedence_order, precedence);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unable to determine precedence_order!");
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
            ret = ENOTRECOVERABLE;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected precedence_order... something is very wrong!");
            return ret;
            break;
        }

        const bool parent_is_hungry_grouping_open = (
                possible_target->parent->kind == VLTL_AST_OPERATION_KIND_GROUPING_OPEN &&
                vltl_ast_operation_argc(*possible_target->parent) < vltl_ast_operation_expected_argc(*(possible_target->parent))
            );

        if(parent_lower_than_pushed && !parent_is_hungry_grouping_open) {
            // always need to travel up because cannot insert here
        } else {
            need_to_displace = true;

            if(!parent_lower_than_pushed || parent_is_hungry_grouping_open) {
                target = possible_target;
                break;
            }
        }
    }

    if(target == NULL) {
        VLTL_EXPECT(vltl_ast_operation_adopt(tree, pushed, tree->root), "Unable to adopt pushed!");
    } else if(need_to_displace) {
        VLTL_EXPECT(vltl_ast_operation_adopt(tree, pushed, target), "Unable to adopt pushed!");
    } else {
        VLTL_RETURN(ENOTRECOVERABLE, "Unexpected failure!");
    }

    return 0;
}

int vltl_ast_tree_convert(Vltl_ast_tree *dest, Vltl_lexer_line *src) {
    Vltl_ast_operation *push_this = NULL;
    Vltl_ast_operation_kind operation_kind = VLTL_AST_OPERATION_KIND_UNSET;
    Vltl_lang_token *evaluates_to = NULL;
    const Vltl_lang_type *result_type = NULL;
    int ret = 0;
    if(dest == NULL || src == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : dest = %p, src = %p!",
            (void *) dest, (void *) src
        );
        return ret;
    }

    if(!vltl_lexer_line_valid(*src)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "Lexer line is invalid!");
        return ret;
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
        case VLTL_LANG_TOKEN_KIND_FUNCTION:
            operation_kind = VLTL_AST_OPERATION_KIND_CALL;
            evaluates_to = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_token));
            if(evaluates_to == NULL) {
                return ENOMEM;
            }

            evaluates_to->kind = VLTL_LANG_TOKEN_KIND_FUNCTION;
            evaluates_to->function = src->tokens[i].token.function;
            result_type = &vltl_lang_type_long;
            ret = vltl_ast_operation_init(push_this, operation_kind, evaluates_to, result_type);
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
            case VLTL_LANG_OPERATION_KIND_CONSTANT:
                operation_kind = VLTL_AST_OPERATION_KIND_CONSTANT;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_LOCAL:
                operation_kind = VLTL_AST_OPERATION_KIND_LOCAL;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_FUNCTION:
                operation_kind = VLTL_AST_OPERATION_KIND_FUNCTION;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_IF:
                operation_kind = VLTL_AST_OPERATION_KIND_IF;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_ELIF:
                operation_kind = VLTL_AST_OPERATION_KIND_ELIF;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_ELSE:
                operation_kind = VLTL_AST_OPERATION_KIND_ELSE;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_WHILE:
                operation_kind = VLTL_AST_OPERATION_KIND_WHILE;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_BODY_OPEN:
                operation_kind = VLTL_AST_OPERATION_KIND_BODY_OPEN;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_BODY_CLOSE:
                operation_kind = VLTL_AST_OPERATION_KIND_BODY_CLOSE;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_GROUPING_OPEN:
                operation_kind = VLTL_AST_OPERATION_KIND_GROUPING_OPEN;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_GROUPING_CLOSE:
                operation_kind = VLTL_AST_OPERATION_KIND_GROUPING_CLOSE;

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
            case VLTL_LANG_OPERATION_KIND_MUL:
                operation_kind = VLTL_AST_OPERATION_KIND_MUL;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_DIV:
                operation_kind = VLTL_AST_OPERATION_KIND_DIV;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_TEST_EQUALS:
                operation_kind = VLTL_AST_OPERATION_KIND_TEST_EQUALS;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_TYPEAS:
                operation_kind = VLTL_AST_OPERATION_KIND_TYPEAS;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            case VLTL_LANG_OPERATION_KIND_COMMA:
                operation_kind = VLTL_AST_OPERATION_KIND_COMMA;

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
            case VLTL_LANG_OPERATION_KIND_RETURN:
                operation_kind = VLTL_AST_OPERATION_KIND_RETURN;

                result_type = &vltl_lang_type_long;
                ret = vltl_ast_operation_init(push_this, operation_kind, NULL, result_type);
                if(ret) {
                    return ret;
                }
                break;
            default:
                ret = EINVAL;
                IESTACK_PUSH(&vltl_global_errors, EINVAL, "Unknown VLTL_LANG_OPERATION_KIND!");
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
            IESTACK_RETURN(&vltl_global_errors, EINVAL, "Unknown VLTL_LANG_TOKEN_KIND!");
            break;
        }

        ret = vltl_ast_tree_insert(dest, push_this);
        if(ret != 0) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unable to insert node into tree!");
            return ret;
        }
    }

    return 0;
}
