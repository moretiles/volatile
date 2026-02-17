#include <ds/iestack.h>
#include <sast.h>
#include <convert.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>

int vltl_sast_operation_copy(Vltl_sast_operation *dest, const Vltl_sast_operation *src) {
    if(dest == NULL || src == NULL) {
        return EINVAL;
    }

    memcpy(dest, src, sizeof(Vltl_sast_operation));
    for(size_t i = 0; i < vltl_sast_operation_argc(*dest); i++) {
        dest->arguments[i]->parent = dest;
    }

    return 0;
}

bool vltl_sast_operation_kind_valid(const Vltl_sast_operation_kind operation_kind) {
    switch(operation_kind) {
    case VLTL_SAST_OPERATION_KIND_EVAL:
    case VLTL_SAST_OPERATION_KIND_LOAD:
    case VLTL_SAST_OPERATION_KIND_ADD:
    case VLTL_SAST_OPERATION_KIND_SUB:
    case VLTL_SAST_OPERATION_KIND_STORE:
    case VLTL_SAST_OPERATION_KIND_GLOBAL:
    case VLTL_SAST_OPERATION_KIND_CONSTANT:
    case VLTL_SAST_OPERATION_KIND_LOCAL:
    case VLTL_SAST_OPERATION_KIND_FUNCTION:
    case VLTL_SAST_OPERATION_KIND_BODY_OPEN:
    case VLTL_SAST_OPERATION_KIND_BODY_CLOSE:
    case VLTL_SAST_OPERATION_KIND_RETURN:
        return true;
        break;
    default:
        return false;
        break;
    }

    return true;
}

bool vltl_sast_operation_valid(const Vltl_sast_operation operation) {
    return vltl_sast_operation_kind_valid(operation.kind);
}

bool vltl_sast_operation_incomplete(const Vltl_sast_operation operation) {
    return (operation.evaluates_to.kind == VLTL_ASM_OPERAND_KIND_TBD) ? true : false;
}

int vltl_sast_operation_insert(
    Vltl_sast_tree *tree,
    Vltl_sast_operation *parent,
    Vltl_sast_operation *new_child,
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

    if((parent != NULL && !vltl_sast_operation_valid(*parent))) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "Parent exists and is invalid!");
        return ret;
    }

    if(!vltl_sast_operation_valid(*new_child)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "new_child exists and is invalid!");
        return ret;
    }

    // rearrange nodes themselves
    {
        if(parent == NULL) {
            if(tree->root != NULL) {
                ret = EINVAL;
                IESTACK_PUSH(&vltl_global_errors, ret, "Parent is NULL in bad location!");
                return ret;
            }

            tree->root = new_child;
        } else {
            parent->arguments[new_child_index] = new_child;
        }
        new_child->parent = parent;
        new_child->belongs_to = tree;
    }

    // vltl_sast_operation specific logic
    {
    }

    return 0;
}

int vltl_sast_operation_adopt(
    Vltl_sast_tree *tree,
    Vltl_sast_operation *new_parent,
    Vltl_sast_operation *adopt_this
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

    if(!vltl_sast_operation_valid(*new_parent)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "It looks like new_parent is invalid!");
        return ret;
    }

    if(!vltl_sast_operation_valid(*adopt_this)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "It looks like adopt_this is invalid!");
        return ret;
    }

    // rearrange nodes themselves
    {
        // append adopt_this to the arguments of new_parent.
        {
            const size_t first_empty_pos = vltl_sast_operation_argc(*new_parent);
            new_parent->arguments[first_empty_pos] = adopt_this;
        }

        // adopt_this->parent[index of adopt_this in parent arguments] = new_parent
        {
            Vltl_sast_operation **parent_pointer_to_adopt_this = NULL;
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

    // vltl_sast_operation specific logic
    {
    }

    return 0;
}

size_t vltl_sast_operation_kind_argc(const Vltl_sast_operation_kind operation_kind) {
    switch(operation_kind) {
    case VLTL_SAST_OPERATION_KIND_EVAL:
    case VLTL_SAST_OPERATION_KIND_BODY_OPEN:
    case VLTL_SAST_OPERATION_KIND_BODY_CLOSE:
        return 0;
        break;
    case VLTL_SAST_OPERATION_KIND_GLOBAL:
    case VLTL_SAST_OPERATION_KIND_CONSTANT:
    case VLTL_SAST_OPERATION_KIND_LOCAL:
    case VLTL_SAST_OPERATION_KIND_RETURN:
        return 1;
        break;
    case VLTL_SAST_OPERATION_KIND_FUNCTION:
    case VLTL_SAST_OPERATION_KIND_LOAD:
    case VLTL_SAST_OPERATION_KIND_STORE:
    case VLTL_SAST_OPERATION_KIND_ADD:
    case VLTL_SAST_OPERATION_KIND_SUB:
        return 2;
        break;
    default:
        return 0;
        break;
    }

    return 0;
}

size_t vltl_sast_operation_argc(const Vltl_sast_operation operation) {
    if(!vltl_sast_operation_valid(operation)) {
        return 0;
    }

    size_t i = 0;
    for(i = 0; i < VLTL_SAST_OPERATION_ARGUMENTS_MAX; i++) {
        if(operation.arguments[i] == NULL) {
            break;
        }
    }

    return i;
}

int vltl_sast_operation_insert_operand(
    Vltl_sast_tree *tree,
    Vltl_sast_operation **created_child_ptr,
    Vltl_sast_operation *parent,
    const Vltl_asm_operand operand,
    size_t new_child_index
) {
    Vltl_sast_operation *created_here = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    *created_child_ptr = created_here;

    int ret = 0;
    if(tree == NULL || created_child_ptr == NULL || parent == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : tree = %p, created_child_ptr = %p, parent = %p!",
            (void *) tree, (void *) created_child_ptr, (void *) parent
        );
        return ret;
    }

    if(!vltl_asm_operand_valid(operand)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "operand is invalid!");
        return ret;
    }

    Vltl_asm_operand empty_destination = { 0 };
    ret = vltl_sast_operation_init(created_here, VLTL_SAST_OPERATION_KIND_EVAL, operand, empty_destination);
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure initializing created_here!");
        return ret;
    }

    size_t parent_argc = vltl_sast_operation_argc(*parent);
    if(parent_argc >= vltl_sast_operation_kind_argc(parent->kind)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "Cannot insert any additional arguments for parent!");
        return ret;
    }
    return vltl_sast_operation_insert(tree, parent, created_here, new_child_index);
}

int vltl_sast_operation_kind_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_sast_operation_kind src
) {
    const char *src_string = NULL;

    if(dest == NULL || dest_cap == 0) {
        return EINVAL;
    }

    switch(src) {
    case VLTL_SAST_OPERATION_KIND_EVAL:
        src_string = "EVAL";
        break;
    case VLTL_SAST_OPERATION_KIND_LOAD:
        src_string = "LOAD";
        break;
    case VLTL_SAST_OPERATION_KIND_STORE:
        src_string = "STORE";
        break;
    case VLTL_SAST_OPERATION_KIND_ADD:
        src_string = "ADD";
        break;
    case VLTL_SAST_OPERATION_KIND_SUB:
        src_string = "SUB";
        break;
    case VLTL_SAST_OPERATION_KIND_RETURN:
        src_string = "RETURN";
        break;
    case VLTL_SAST_OPERATION_KIND_GLOBAL:
        src_string = "GLOBAL";
        break;
    case VLTL_SAST_OPERATION_KIND_CONSTANT:
        src_string = "CONSTANT";
        break;
    case VLTL_SAST_OPERATION_KIND_LOCAL:
        src_string = "LOCAL";
        break;
    case VLTL_SAST_OPERATION_KIND_FUNCTION:
        src_string = "FUNCTION";
        break;
    case VLTL_SAST_OPERATION_KIND_BODY_OPEN:
        src_string = "BODY_OPEN";
        break;
    case VLTL_SAST_OPERATION_KIND_BODY_CLOSE:
        src_string = "BODY_CLOSE";
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

int vltl_sast_operation_detokenize(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_sast_operation src) {
    int ret;
    const size_t buf_cap = 999;
    size_t buf_len = 0;
    char buf_for_kind[buf_cap];
    char buf_for_evaluates_to[buf_cap];
    char buf_for_arguments_and_destination[buf_cap];
    int dest_len_helper = 0;
    if(dest == NULL || dest_cap == 0) {
        return EINVAL;
    }

    ret = vltl_sast_operation_kind_detokenize(buf_for_kind, buf_cap, &buf_len, src.kind);
    if(ret != 0) {
        return ret;
    }

    if(src.evaluates_to.kind == VLTL_ASM_OPERAND_KIND_UNSET) {
        memcpy(buf_for_evaluates_to, "???", 3 + 1);
    } else {
        ret = vltl_asm_operand_detokenize(buf_for_evaluates_to, buf_cap, &buf_len, src.evaluates_to);
        if(ret != 0) {
            return ret;
        }
    }

    // Produce expression detailing the arguments and destination of the src sast_operation
    {
        const size_t buf_for_arguments_and_destination_cap = buf_cap;
        size_t buf_for_arguments_and_destination_len = 0;
        size_t buf_for_arguments_and_destination_len_helper = 0;
        int buf_for_arguments_and_destination_len_helper2 = 0;

        buf_for_arguments_and_destination_len_helper2 = snprintf(
                &(buf_for_arguments_and_destination[buf_for_arguments_and_destination_len]),
                buf_for_arguments_and_destination_cap - buf_for_arguments_and_destination_len,
                "("
            );
        if(buf_for_arguments_and_destination_len_helper2 < 0) {
            return ENOMEM;
        }
        buf_for_arguments_and_destination_len += (size_t) buf_for_arguments_and_destination_len_helper2;

        for(size_t i = 0; i < vltl_sast_operation_argc(src); i++) {
            ret = vltl_asm_operand_detokenize(
                      &(buf_for_arguments_and_destination[buf_for_arguments_and_destination_len]),
                      buf_for_arguments_and_destination_cap - buf_for_arguments_and_destination_len,
                      &buf_for_arguments_and_destination_len_helper,
                      src.arguments[i]->evaluates_to
                  );
            if(ret != 0) {
                return ret;
            }
            buf_for_arguments_and_destination_len += buf_for_arguments_and_destination_len_helper;

            if(i + 1 < vltl_sast_operation_argc(src)) {
                buf_for_arguments_and_destination_len_helper2 = snprintf(
                        &(buf_for_arguments_and_destination[buf_for_arguments_and_destination_len]),
                        buf_for_arguments_and_destination_cap - buf_for_arguments_and_destination_len,
                        ", "
                    );
                if(buf_for_arguments_and_destination_len_helper2 < 0) {
                    return ENOMEM;
                }
                buf_for_arguments_and_destination_len += (size_t) buf_for_arguments_and_destination_len_helper2;
            }
        }

        buf_for_arguments_and_destination_len_helper2 = snprintf(
                &(buf_for_arguments_and_destination[buf_for_arguments_and_destination_len]),
                buf_for_arguments_and_destination_cap - buf_for_arguments_and_destination_len,
                ") => "
            );
        if(buf_for_arguments_and_destination_len_helper2 < 0) {
            return ENOMEM;
        }
        buf_for_arguments_and_destination_len += (size_t) buf_for_arguments_and_destination_len_helper2;

        if(src.destination.kind == VLTL_ASM_OPERAND_KIND_UNSET) {
            memcpy(&(buf_for_arguments_and_destination[buf_for_arguments_and_destination_len]), "???", 3 + 1);
        } else {
            ret = vltl_asm_operand_detokenize(
                      &(buf_for_arguments_and_destination[buf_for_arguments_and_destination_len]),
                      buf_for_arguments_and_destination_cap - buf_for_arguments_and_destination_len,
                      &buf_for_arguments_and_destination_len_helper,
                      src.destination
                  );
            if(ret != 0) {
                return ret;
            }
            buf_for_arguments_and_destination_len += buf_for_arguments_and_destination_len_helper;
        }
    }

    dest_len_helper = snprintf(
                          dest, dest_cap, "%s\\n%s\\n%s", buf_for_kind, buf_for_evaluates_to, buf_for_arguments_and_destination
                      );
    if(dest_len_helper < 0) {
        return ENOTRECOVERABLE;
    }

    *dest_len = (size_t) dest_len_helper;
    return 0;
}

static int vltl_sast_tree_detokenize_recurse(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_sast_operation operation,
    size_t *monotonic_index,
    size_t parent_index
) {
    int ret = 0;
    size_t dest_offset = 0, dest_len_helper = 0, initial_value_of_monotonic_index = *monotonic_index;
    int dest_len_helper2 = 0;
    if(dest == NULL || dest_cap == 0 || dest_len == NULL) {
        return EINVAL;
    }

    // Don't try to detokenize or recurse.
    if(
        operation.kind == VLTL_SAST_OPERATION_KIND_UNSET ||
        operation.kind == VLTL_SAST_OPERATION_KIND_EVAL
    ) {
        *dest_len = 0;
        return 0;
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

    ret = vltl_sast_operation_detokenize(
              &(dest[dest_offset]), dest_cap - dest_offset, &dest_len_helper, operation
          );
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
    for(size_t i = 0; i < vltl_sast_operation_argc(operation); i++) {
        ret = vltl_sast_tree_detokenize_recurse(
                  &(dest[dest_offset]),
                  dest_cap,
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

int vltl_sast_tree_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_sast_tree src
) {
    size_t dest_offset = 0, dest_len_helper = 0, monotonic_index = 0;
    int dest_len_helper2 = 0;
    int ret = 0;

    if(dest == NULL || dest_cap == 0 || dest_len == NULL || src.root == NULL) {
        return EINVAL;
    }

    dest_len_helper2 = snprintf(&(dest[dest_offset]), dest_cap, "digraph sast_tree {\n");
    if(dest_len_helper2 < 0) {
        return ENOTRECOVERABLE;
    }
    dest_offset += (size_t) dest_len_helper2;
    dest_cap -= (size_t) dest_len_helper2;

    size_t initial_value_of_monotonic_index = 0;
    ret = vltl_sast_tree_detokenize_recurse(
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

int vltl_sast_operation_init(
    Vltl_sast_operation *operation,
    Vltl_sast_operation_kind kind,
    Vltl_asm_operand evaluates_to,
    Vltl_asm_operand destination
) {
    if(
        operation == NULL ||
        !vltl_sast_operation_kind_valid(kind)
    ) {
        return EINVAL;
    }

    *operation = (Vltl_sast_operation) {
        0
    };
    operation->kind = kind;
    operation->evaluates_to = evaluates_to;
    operation->destination = destination;

    return 0;
}

int vltl_sast_tree_connect_recurse(Vltl_sast_tree *tree, Vltl_sast_operation *operation) {
    int ret = 0;
    if(tree == NULL || operation == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : tree = %p, operation = %p!",
            (void *) tree, (void *) operation
        );
        return EINVAL;
    }

    const bool is_eval = operation->kind == VLTL_SAST_OPERATION_KIND_EVAL;
    const bool is_incomplete = vltl_sast_operation_incomplete(*operation);
    const bool argc_is_0 = vltl_sast_operation_argc(*operation) == 0;
    const bool kind_argc_is_0 = vltl_sast_operation_kind_argc(operation->kind) == 0;
    if(!is_incomplete && (is_eval || argc_is_0 || kind_argc_is_0)) {
        return 0;
    } else if (is_incomplete) {
        // Set destination for operation if not already set by popping unused register from global registers
        const bool parent_not_null = operation->parent != NULL;
        const bool is_first_argument = parent_not_null && operation->parent->arguments[0] == operation;
        if(!is_first_argument) {
            ret = EINVAL;
            IESTACK_PUSH(&vltl_global_errors, ret, "An operation that is not the first (lchild) is incomplete!");
            return ret;
        }

        Vltl_global_register *reserve_this = NULL;
        ret = vltl_global_registers_clear();
        if(ret != 0) {
            ret = ENOTRECOVERABLE;
            IESTACK_PUSH(
                &vltl_global_errors, ret, "Unexpected failure when calling vltl_global_registers_clear!"
            );
            return ret;
        }

        // build a complete understanding of what registers are available
        Vltl_sast_operation *prev_parent = operation, *current_parent = operation->parent;
        for(
            ;
            current_parent != NULL;
            prev_parent = current_parent, current_parent = current_parent->parent
        ) {
            bool inner_done = false;
            for(size_t i = 0; !inner_done && i < vltl_sast_operation_argc(*current_parent); i++) {
                Vltl_sast_operation *current_sibling = current_parent->arguments[i];

                // stop once you reach the child that is in prev_parent to avoid double counting it
                if(current_sibling == prev_parent) {
                    inner_done = true;
                    continue;
                }

                if(current_sibling->destination.kind != VLTL_ASM_OPERAND_KIND_REGISTER) {
                    continue;
                }

                ret = vltl_convert_asm_operand_to_global_register(
                          &reserve_this, current_sibling->destination
                      );
                if(ret) {
                    IESTACK_PUSH(
                        &vltl_global_errors, ret, "Unexpected failure when converting operand to register!"
                    );
                    return ret;
                }
                ret = vltl_global_registers_inuse(reserve_this);
                if(ret != 0) {
                    IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure marking register as in-use!");
                    return ret;
                }
            }
        }

        ret = vltl_global_registers_use(&reserve_this);
        if(ret != 0) {
            ret = EXFULL;
            IESTACK_PUSH(&vltl_global_errors, ret, "No free registers available!");
            return ret;
        }
        Vltl_asm_operand register_operand = { 0 };
        vltl_convert_global_register_to_asm_operand(&register_operand, reserve_this);
        if(ret != 0) {
            ret = ENOTRECOVERABLE;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when converting register to operand!");
            return ret;
        }

        operation->evaluates_to = register_operand;
        operation->destination = register_operand;
    } else {
        const size_t initial_argc = vltl_sast_operation_argc(*operation);
        const size_t full_argc = vltl_sast_operation_kind_argc(operation->kind);

        for(size_t i = 0; i < vltl_sast_operation_argc(*operation); i++) {
            ret = vltl_sast_tree_connect_recurse(tree, operation->arguments[i]);
            if(ret) {
                IESTACK_PUSH(
                    &vltl_global_errors, ret, "Unexpected failure when calling vltl_sast_tree_connect_recurse!"
                );
                return ret;
            }
        }

        // operation-specific check on children
        const size_t operation_argc = vltl_sast_operation_argc(*operation);
        const size_t expected_operation_argc = vltl_sast_operation_kind_argc(operation->kind);
        bool all_children_can_be_evaluated_as_immediate_values = false;
        if(initial_argc != full_argc) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Invalid number of argument for operation!");
            return EINVAL;
        }

        switch(operation->kind) {
        case VLTL_SAST_OPERATION_KIND_ADD:
            ;
            if(operation_argc != expected_operation_argc || operation_argc != 2) {
                ret = ENOTRECOVERABLE;
                IESTACK_PUSH(&vltl_global_errors, ret, "Add operation does not have two arguments!");
                return ret;
            }

            all_children_can_be_evaluated_as_immediate_values = true;
            for(size_t i = 0; i < vltl_sast_operation_kind_argc(operation->kind); i++) {
                if(operation->arguments[i]->evaluates_to.kind != VLTL_ASM_OPERAND_KIND_IMMEDIATE) {
                    all_children_can_be_evaluated_as_immediate_values = false;
                    break;
                }
            }

            if(all_children_can_be_evaluated_as_immediate_values) {
                operation->evaluates_to = (Vltl_asm_operand) {
                    .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
                    .as_immediate = (Vltl_asm_operand_immediate) {
                        .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                        .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
                        .value = 0
                    }
                };

                const uint64_t add_this = operation->arguments[0]->evaluates_to.as_immediate.value;
                const uint64_t also_add_this = operation->arguments[1]->evaluates_to.as_immediate.value;
                operation->evaluates_to.as_immediate.value += add_this;
                operation->evaluates_to.as_immediate.value += also_add_this;
            }

            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_SUB:
            ;
            if(operation_argc != expected_operation_argc || operation_argc != 2) {
                ret = ENOTRECOVERABLE;
                IESTACK_PUSH(&vltl_global_errors, ret, "Add operation does not have two arguments!");
                return ret;
            }

            all_children_can_be_evaluated_as_immediate_values = true;
            for(size_t i = 0; i < vltl_sast_operation_kind_argc(operation->kind); i++) {
                if(operation->arguments[i]->evaluates_to.kind != VLTL_ASM_OPERAND_KIND_IMMEDIATE) {
                    all_children_can_be_evaluated_as_immediate_values = false;
                    break;
                }
            }

            if(all_children_can_be_evaluated_as_immediate_values) {
                operation->evaluates_to = (Vltl_asm_operand) {
                    .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
                    .as_immediate = (Vltl_asm_operand_immediate) {
                        .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                        .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
                        .value = 0
                    }
                };

                const uint64_t add_this = operation->arguments[0]->evaluates_to.as_immediate.value;
                const uint64_t subtract_this = operation->arguments[1]->evaluates_to.as_immediate.value;
                operation->evaluates_to.as_immediate.value += add_this;
                operation->evaluates_to.as_immediate.value -= subtract_this;
            }

            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_STORE:
            if(operation->lchild == NULL || operation->rchild == NULL) {
                IESTACK_PUSH(&vltl_global_errors, ret, "One or more arguments of operation are NULL!");
                return EINVAL;
            }

            operation->destination = operation->lchild->evaluates_to;
            operation->evaluates_to = operation->rchild->evaluates_to;
            break;
        case VLTL_SAST_OPERATION_KIND_LOAD:
            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_GLOBAL:
            operation->evaluates_to = operation->arguments[0]->evaluates_to;
            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_CONSTANT:
            operation->evaluates_to = operation->arguments[0]->evaluates_to;
            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_LOCAL:
            operation->evaluates_to = operation->arguments[0]->evaluates_to;
            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_FUNCTION:
            operation->evaluates_to = operation->arguments[0]->evaluates_to;
            operation->destination = operation->arguments[0]->destination;
            break;
        case VLTL_SAST_OPERATION_KIND_RETURN:
            operation->evaluates_to = operation->arguments[0]->evaluates_to;
            operation->destination = operation->arguments[0]->destination;
            break;
        default:
            break;
        }
    }

    return 0;
}

int vltl_sast_tree_connect(Vltl_sast_tree *tree) {
    int ret = 0;
    if(tree == NULL || tree->root == NULL) {
        return EINVAL;
    }

    ret = vltl_sast_tree_connect_recurse(tree, tree->root);
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vltl_sast_tree_connect_recurse!");
    }
    return ret;
}

int vltl_sast_operation_convert_amd64_eval(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *eval_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(eval_operation == NULL) {
        return ENOMEM;
    }

    switch(src->evaluates_to->kind) {
    case VLTL_LANG_TOKEN_KIND_GLOBAL:
        *eval_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = NULL,
            .evaluates_to = (Vltl_asm_operand) {
                .kind = VLTL_ASM_OPERAND_KIND_MEMORY,
                .as_memory = (Vltl_asm_operand_memory) {
                    .memory_kind = VLTL_ASM_OPERAND_MEMORY_KIND_GLOBAL,
                    .name = src->evaluates_to->global->name,
                    .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                    .value = 0
                }
            }
        };
        break;
    case VLTL_LANG_TOKEN_KIND_LOCAL:
        *eval_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = NULL,
            .evaluates_to = (Vltl_asm_operand) {
                .kind = VLTL_ASM_OPERAND_KIND_MEMORY,
                .as_memory = (Vltl_asm_operand_memory) {
                    .memory_kind = VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL,
                    .name = src->evaluates_to->local->name,
                    .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                    .value = 0
                }
            }
        };
        break;
    case VLTL_LANG_TOKEN_KIND_UNKNOWN:
        *eval_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = NULL,
            .evaluates_to = (Vltl_asm_operand) {
                .kind = VLTL_ASM_OPERAND_KIND_UNKNOWN,
                .as_unknown = src->evaluates_to->unknown
            }
        };
        break;
    default:
        return EINVAL;
        break;
    }

    *equivalent = eval_operation;

    // don't push anything
    (void) insert_below_next;
    return 0;
}

int vltl_sast_operation_convert_amd64_load(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    int ret = 0;
    Vltl_asm_operand evaluates_to = { 0 };
    Vltl_sast_operation *root = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    Vltl_sast_operation *source_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    Vltl_sast_operation *destination_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(source_operation == NULL || destination_operation == NULL) {
        return ENOMEM;
    }

    if( src->evaluates_to == NULL) {
        return EINVAL;
    }

    switch(src->evaluates_to->kind) {
    case VLTL_LANG_TOKEN_KIND_LITERAL:
        evaluates_to = (Vltl_asm_operand) {
            .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
            .as_immediate = {
                .integral_type = src->evaluates_to->literal.type->integral_type,
                .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
                .value = (uint64_t) src->evaluates_to->literal.fields[0]
            }
        };
        if(ret != 0) {
            return ret;
        }
        *root = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_LOAD,
            .arguments = { destination_operation, source_operation },
            .evaluates_to = evaluates_to
        };

        *destination_operation = vltl_sast_operation_tbd;
        destination_operation->parent = root;

        *source_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = root,
            .evaluates_to = evaluates_to
        };

        *equivalent = root;
        break;
    case VLTL_LANG_TOKEN_KIND_GLOBAL:
        evaluates_to = (Vltl_asm_operand) {
            .kind = VLTL_ASM_OPERAND_KIND_MEMORY,
            .as_memory = {
                .memory_kind = VLTL_ASM_OPERAND_MEMORY_KIND_GLOBAL,
                .name = src->evaluates_to->global->name,
                .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                .value = 0
            }
        };
        if(ret != 0) {
            return ret;
        }
        *root = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_LOAD,
            .arguments = { destination_operation, source_operation },
            .evaluates_to = evaluates_to
        };

        *destination_operation = vltl_sast_operation_tbd;
        destination_operation->parent = root;

        *source_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = root,
            .evaluates_to = evaluates_to
        };

        *equivalent = root;
        break;
    case VLTL_LANG_TOKEN_KIND_CONSTANT:
        evaluates_to = (Vltl_asm_operand) {
            .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
            .as_immediate = (Vltl_asm_operand_immediate) {
                .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
                .value = (uint64_t) src->evaluates_to->constant->literal->fields[0]
            }
        };
        if(ret != 0) {
            return ret;
        }
        *root = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_LOAD,
            .arguments = { destination_operation, source_operation },
            .evaluates_to = evaluates_to
        };

        *destination_operation = vltl_sast_operation_tbd;
        destination_operation->parent = root;

        *source_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = root,
            .evaluates_to = evaluates_to
        };

        *equivalent = root;
        break;
    case VLTL_LANG_TOKEN_KIND_LOCAL:
        evaluates_to = (Vltl_asm_operand) {
            .kind = VLTL_ASM_OPERAND_KIND_MEMORY,
            .as_memory = {
                .memory_kind = VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL,
                .name = src->evaluates_to->local->name,
                .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
                .value = 0
            }
        };
        if(ret != 0) {
            return ret;
        }
        *root = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_LOAD,
            .arguments = { destination_operation, source_operation },
            .evaluates_to = evaluates_to
        };

        *destination_operation = vltl_sast_operation_tbd;
        destination_operation->parent = root;

        *source_operation = (Vltl_sast_operation) {
            .kind = VLTL_SAST_OPERATION_KIND_EVAL,
            .parent = root,
            .evaluates_to = evaluates_to
        };

        *equivalent = root;
        break;
    default:
        return EINVAL;
        break;
    }

    // don't push to insert_below_next
    (void) insert_below_next;
    return 0;
}

int vltl_sast_operation_convert_amd64_store(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *store_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(store_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *store_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_STORE,
        .parent = NULL
    };

    *equivalent = store_operation;

    // push twice for destination and source
    vstack_push(insert_below_next, &store_operation);
    vstack_push(insert_below_next, &store_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_return(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *return_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    Vltl_sast_operation *store_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    Vltl_sast_operation *eval_rax_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(return_operation == NULL || store_operation == NULL || eval_rax_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *return_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_RETURN,
        .parent = NULL,
        .lchild = store_operation
    };
    *store_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_STORE,
        .parent = NULL,
        .lchild = eval_rax_operation
    };
    *eval_rax_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_EVAL,
        .parent = NULL,
        .evaluates_to = vltl_asm_operand_amd64_rax
    };

    *equivalent = return_operation;

    // push twice for destination and source
    vstack_push(insert_below_next, &store_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_add(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *add_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(add_operation == NULL) {
        return ENOMEM;
    }

    // don't use src for anything
    (void) src;
    *add_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_ADD,
        .parent = NULL
    };

    *equivalent = add_operation;

    // push twice for the lchild and rchild of the addition itself
    vstack_push(insert_below_next, &add_operation);
    vstack_push(insert_below_next, &add_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_sub(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *sub_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(sub_operation == NULL) {
        return ENOMEM;
    }

    // Don't use src for anything
    (void) *src;

    *sub_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_SUB,
        .parent = NULL
    };

    *equivalent = sub_operation;

    // push twice for the lchild and rchild of the subtraction itself
    vstack_push(insert_below_next, &sub_operation);
    vstack_push(insert_below_next, &sub_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_global(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *global_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(global_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *global_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_GLOBAL,
        .parent = NULL,
    };

    *equivalent = global_operation;

    vstack_push(insert_below_next, &global_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_local(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *local_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(local_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *local_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_LOCAL,
        .parent = NULL,
    };

    *equivalent = local_operation;

    vstack_push(insert_below_next, &local_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_constant(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *constant_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(constant_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *constant_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_CONSTANT,
        .parent = NULL,
    };

    *equivalent = constant_operation;

    vstack_push(insert_below_next, &constant_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_function(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *function_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(function_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *function_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_FUNCTION,
        .parent = NULL,
    };

    *equivalent = function_operation;

    vstack_push(insert_below_next, &function_operation);
    vstack_push(insert_below_next, &function_operation);
    return 0;
}

int vltl_sast_operation_convert_amd64_body_open(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *body_open_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(body_open_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *body_open_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_BODY_OPEN,
        .parent = NULL,
    };

    *equivalent = body_open_operation;

    (void) insert_below_next;
    return 0;
}

int vltl_sast_operation_convert_amd64_body_close(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    Vltl_sast_operation *body_close_operation = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_sast_operation));
    if(body_close_operation == NULL) {
        return ENOMEM;
    }

    (void) src;
    *body_close_operation = (Vltl_sast_operation) {
        .kind = VLTL_SAST_OPERATION_KIND_BODY_CLOSE,
        .parent = NULL,
    };

    *equivalent = body_close_operation;

    (void) insert_below_next;
    return 0;
}

int vltl_sast_operation_convert_amd64(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    switch(src->kind) {
    case VLTL_AST_OPERATION_KIND_EVAL:
        ;
        const bool parent_is_null = src->parent == NULL;
        const bool is_destination_of_parent = !parent_is_null && (src->parent->lchild == src);
        const bool parent_modifies_variable = (
                !parent_is_null &&
                (
                    (src->parent->kind == VLTL_AST_OPERATION_KIND_EQUALS) ||
                    (src->parent->kind == VLTL_AST_OPERATION_KIND_FUNCTION)
                )
                                              );

        if(is_destination_of_parent && parent_modifies_variable) {
            return vltl_sast_operation_convert_amd64_eval(equivalent, insert_below_next, src);
        } else {
            return vltl_sast_operation_convert_amd64_load(equivalent, insert_below_next, src);
        }
        break;
    case VLTL_AST_OPERATION_KIND_ADD:
        return vltl_sast_operation_convert_amd64_add(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_SUB:
        return vltl_sast_operation_convert_amd64_sub(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_EQUALS:
        return vltl_sast_operation_convert_amd64_store(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        return vltl_sast_operation_convert_amd64_global(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_CONSTANT:
        return vltl_sast_operation_convert_amd64_constant(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_LOCAL:
        return vltl_sast_operation_convert_amd64_local(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_FUNCTION:
        return vltl_sast_operation_convert_amd64_function(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_BODY_OPEN:
        return vltl_sast_operation_convert_amd64_body_open(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_BODY_CLOSE:
        return vltl_sast_operation_convert_amd64_body_close(equivalent, insert_below_next, src);
        break;
    case VLTL_AST_OPERATION_KIND_RETURN:
        return vltl_sast_operation_convert_amd64_return(equivalent, insert_below_next, src);
        break;
    default:
        // not implemented yet
        return EINVAL;
        break;
    }

    return 0;
}

int vltl_sast_operation_convert(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
) {
    int ret = 0;
    if(equivalent == NULL || insert_below_next == NULL || src == NULL || !vltl_ast_operation_valid(*src)) {
        return EINVAL;
    }

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        ret = vltl_sast_operation_convert_amd64(equivalent, insert_below_next, src);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when converting under AMD64!");
        }
        break;
    default:
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "Unknown ISA, unable to convert sast_operation to ast_operation!");
        break;
    }

    return ret;
}

static int vltl_sast_tree_convert_helper(
    Vltl_sast_tree *dest, Vltl_ast_tree *src,
    Vstack *ast_operations_to_visit, Vstack *sast_operations_to_insert_below
) {
    int ret = 0;
    Vltl_ast_operation *current_operation = NULL;
    Vltl_sast_operation *created = NULL;
    Vltl_sast_operation *insert_below = NULL;
    if(dest == NULL || src == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : dest = %p, src = %p!",
            (void *) dest, (void *) src
        );
        return ret;
    }

    ret = vstack_pop(ast_operations_to_visit, &current_operation);
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vstack_pop!");
        return ret;
    } else if(current_operation == NULL) {
        return 0;
    } else if(
        vltl_ast_operation_argc(*current_operation) != vltl_ast_operation_kind_argc(current_operation->kind)
    ) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "The number of arguments src has is invalid!");
        return ret;
    }

    size_t insert_below_argc = 0;
    ret = vstack_pop(sast_operations_to_insert_below, &insert_below);
    if(ret == 0) {
        insert_below_argc = vltl_sast_operation_argc(*insert_below);
        if(insert_below_argc >= vltl_sast_operation_kind_argc(insert_below->kind)) {
            ret = EXFULL;
            IESTACK_PUSH(
                &vltl_global_errors, ret,
                "The sast_operation dest cannot support any additonal arguments!"
            );
            return ret;
        }
    } else if(ret == ENODATA) {
        insert_below = NULL;
    } else {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vstack_pop!");
        return ret;
    }

    ret = vltl_sast_operation_convert(&created, sast_operations_to_insert_below, current_operation);
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vltl_sast_operation_convert!");
        return ret;
    }
    ret = vltl_sast_operation_insert(dest, insert_below, created, insert_below_argc);
    if(ret != 0) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vltl_sast_operation_insert!");
        return ret;
    }

    size_t ast_operations_to_visit_helper = vltl_ast_operation_argc(*current_operation);
    if(ast_operations_to_visit_helper == 0) {
        return 0;
    }

    // Iterate in reverse order so &(current_operation->arguments[0]) is on the top of ast_operations_to_visit.
    for(size_t i = ast_operations_to_visit_helper - 1; true; i--) {
        ret = vstack_push(ast_operations_to_visit, &(current_operation->arguments[i]));
        if(ret != 0) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vstack_push!");
            return ret;
        }

        if(i == 0) {
            break;
        }
    }
    return 0;
}

int vltl_sast_tree_convert(Vltl_sast_tree *dest, Vltl_ast_tree *src) {
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

    Vstack *ast_operations_to_visit = vstack_create(sizeof(Vltl_ast_operation *), 999);
    Vstack *sast_operations_to_insert_below = vstack_create(sizeof(Vltl_sast_operation *), 999);
    ret = vstack_push(ast_operations_to_visit, &(src->root));
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vstack_push!");
        return ret;
    }
    while(vstack_len(ast_operations_to_visit) > 0) {
        ret = vltl_sast_tree_convert_helper(
                  dest, src, ast_operations_to_visit, sast_operations_to_insert_below
              );
        if(ret) {
            IESTACK_PUSH(
                &vltl_global_errors, ret, "Unexpected failure when calling vltl_sast_tree_convert_helper!"
            );
            return ret;
        }
    }

    // optimize?

    ret = vltl_sast_tree_connect(dest);
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vltl_sast_tree_connect!");
        return ret;
    }

    // optimize again?

    return 0;
}

Vltl_sast_operation vltl_sast_operation_tbd = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_tbd,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rax = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rax,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rbx = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rbx,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rcx = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rcx,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rdx = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rdx,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rdi = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rdi,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rsi = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rsi,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rbp = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rbp,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_rsp = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_rsp,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r8 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r8,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r9 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r9,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r10 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r10,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r11 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r11,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r12 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r12,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r13 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r13,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r14 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r14,
    .destination = { 0 }
};

Vltl_sast_operation vltl_sast_operation_amd64_r15 = {
    .kind = VLTL_SAST_OPERATION_KIND_EVAL,
    .belongs_to = NULL,
    .parent = NULL,
    .arguments = { 0 },
    .evaluates_to = vltl_asm_operand_amd64_r15,
    .destination = { 0 }
};
