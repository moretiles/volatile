#include <reshape.h>
#include <lang/function.h>

int vltl_reshape_ast_tree_external(Vltl_ast_tree *tree, bool *done_now) {
    int ret = 0;

    IESTACK_SUPPOSE(tree->root->lchild, EINVAL, "No name for function!");
    IESTACK_SUPPOSE(tree->root->lchild->kind == VLTL_AST_OPERATION_KIND_FUNCTION, EINVAL, "External only works for functions right now!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild, EINVAL, "No name for function!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild->kind == VLTL_AST_OPERATION_KIND_EVAL, EINVAL, "No name for function!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_UNKNOWN, EINVAL, "No name for function!");
    const char *external_name = tree->root->lchild->lchild->evaluates_to->unknown;

    size_t ignore_src_len = 0;
    char *copy_of_external_name = varena_alloc(&vltl_global_allocator, 1 + strlen(external_name));
    btrc_strncpy(&ignore_src_len, copy_of_external_name, external_name, strlen(external_name));

    Vltl_lang_function *created_function = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_function));
    if(created_function == NULL) {
        ret = ENOMEM;
        IESTACK_RETURN2(vltl_global_errors, ret, "Could not allocate enough memory!");
    }

    ret = vltl_lang_function_init(created_function, copy_of_external_name);
    if(ret) {
        IESTACK_RETURN2(vltl_global_errors, ret, "Could not initialize lexer function!");
    }

    ret = nkht_set(vltl_global_table_functions, copy_of_external_name, &created_function);
    if(ret) {
        IESTACK_RETURN2(vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
    }

    *done_now = true;

    return 0;
}

int vltl_reshape_ast_tree_constant(Vltl_ast_tree *tree, bool *done_now) {
    Vltl_sast_tree sast_tree = { 0 };
    int ret = 0;

    IESTACK_SUPPOSE(tree->root->lchild, EINVAL, "Constant not defined as anything!");
    IESTACK_SUPPOSE(tree->root->lchild->kind == VLTL_AST_OPERATION_KIND_EQUALS, EINVAL, "Constant not assigned value using = (the assignment operator)!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild, EINVAL, "No type set for constant!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild->kind == VLTL_AST_OPERATION_KIND_TYPEAS, EINVAL, "No type set for constant!");

    const Vltl_ast_operation *type_as = tree->root->lchild->lchild;
    IESTACK_SUPPOSE(type_as->lchild, EINVAL, "Name used for constant is not unused!");
    IESTACK_SUPPOSE(type_as->lchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_UNKNOWN, EINVAL, "Name used for constant is not unused!");
    const char *constant_name = type_as->lchild->evaluates_to->unknown;
    IESTACK_SUPPOSE(type_as->rchild, EINVAL, "Existing type not provided for constant!");
    IESTACK_SUPPOSE(type_as->rchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_TYPE, EINVAL, "Existing type not provided for constant!");
    const Vltl_lang_type *constant_type = type_as->lchild->evaluates_to->type;

    tree->root = tree->root->lchild->rchild;
    tree->root->parent = NULL;
    ret = vltl_sast_tree_convert(&sast_tree, tree);
    IESTACK_SUPPOSE(sast_tree.root->evaluates_to.kind == VLTL_ASM_OPERAND_KIND_IMMEDIATE, EINVAL, "Value assigned to constant could not be evaluated here and now!");

    size_t ignore_src_len = 0;
    char *copy_of_constant_name = varena_alloc(&vltl_global_allocator, 1 + strlen(constant_name));
    btrc_strncpy(&ignore_src_len, copy_of_constant_name, constant_name, strlen(constant_name));
    Vltl_lang_constant *created_constant = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_constant));
    Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
    if(created_constant == NULL || created_literal == NULL) {
        ret = ENOMEM;
        IESTACK_RETURN2(vltl_global_errors, ret, "Could not allocate enough memory!");
    }
    *created_literal = (Vltl_lang_literal) {
        .name = NULL,
        .type = constant_type,
        .attributes = { 0 },
        .fields = { (void *) sast_tree.root->evaluates_to.as_immediate.value }
                  //.fields = { 0 }
    };
    *created_constant = (Vltl_lang_constant) {
        .name = copy_of_constant_name,
        .type = constant_type,
        .attributes = { 0 },
        .literal = created_literal
    };

    ret = nkht_set(vltl_global_table_constants, copy_of_constant_name, &created_constant);
    if(ret) {
        IESTACK_RETURN2(vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
    }

    *done_now = true;

    return 0;
}

int vltl_reshape_ast_tree_global(Vltl_ast_tree *tree, bool *done_now) {
    int ret = 0;
    Vltl_sast_tree sast_tree = { 0 };

    IESTACK_SUPPOSE(tree->root->lchild, EINVAL, "Global not defined as anything!");
    IESTACK_SUPPOSE(tree->root->lchild->kind == VLTL_AST_OPERATION_KIND_EQUALS, EINVAL, "Global not assigned value using = (the assignment operator)!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild, EINVAL, "No type set for global!");
    IESTACK_SUPPOSE(tree->root->lchild->lchild->kind == VLTL_AST_OPERATION_KIND_TYPEAS, EINVAL, "No type set for global!");

    const Vltl_ast_operation *type_as = tree->root->lchild->lchild;
    IESTACK_SUPPOSE(type_as->lchild, EINVAL, "Name used for global is not unused!");
    IESTACK_SUPPOSE(type_as->lchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_UNKNOWN, EINVAL, "Name used for global is not unused!");
    const char *global_name = type_as->lchild->evaluates_to->unknown;
    IESTACK_SUPPOSE(type_as->rchild, EINVAL, "Existing type not provided for global!");
    IESTACK_SUPPOSE(type_as->rchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_TYPE, EINVAL, "Existing type not provided for global!");
    const Vltl_lang_type *global_type = type_as->lchild->evaluates_to->type;

    tree->root = tree->root->lchild->rchild;
    tree->root->parent = NULL;
    ret = vltl_sast_tree_convert(&sast_tree, tree);
    IESTACK_SUPPOSE(sast_tree.root->evaluates_to.kind == VLTL_ASM_OPERAND_KIND_IMMEDIATE, EINVAL, "Value assigned to global could not be evaluated here and now!");

    size_t ignore_src_len = 0;
    char *copy_of_global_name = varena_alloc(&vltl_global_allocator, 1 + strlen(global_name));
    btrc_strncpy(&ignore_src_len, copy_of_global_name, global_name, strlen(global_name));

    Vltl_lang_global *created_global = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_global));
    Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
    if(created_global == NULL || created_literal == NULL) {
        ret = ENOMEM;
        IESTACK_RETURN2(vltl_global_errors, ret, "Could not allocate enough memory!");
    }
    *created_literal = (Vltl_lang_literal) {
        .name = NULL,
        .type = global_type,
        .attributes = { 0 },
        .fields = { (void *) sast_tree.root->evaluates_to.as_immediate.value }
    };
    *created_global = (Vltl_lang_global) {
        .name = copy_of_global_name,
        .type = global_type,
        .attributes = { 0 },
        .literal = created_literal
    };

    ret = nkht_set(vltl_global_table_globals, copy_of_global_name, &created_global);
    if(ret) {
        IESTACK_RETURN2(vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
    }

    *done_now = true;

    return 0;
}

int vltl_reshape_ast_tree_local(Vltl_ast_tree *tree, bool *done_now) {
    int ret = 0;
    Vltl_ast_operation *type_as  = NULL;
    const Vltl_lang_type *local_type = NULL;
    const char *local_name = NULL;

    // Make sure all is good
    {
        IESTACK_SUPPOSE(tree->root->lchild, EINVAL, "Local not defined as anything!");
        IESTACK_SUPPOSE(tree->root->lchild->kind == VLTL_AST_OPERATION_KIND_EQUALS, EINVAL, "Local not assigned value using = (the assignment operator)!");
        IESTACK_SUPPOSE(tree->root->lchild->lchild, EINVAL, "No type set for local!");
        IESTACK_SUPPOSE(tree->root->lchild->lchild->kind == VLTL_AST_OPERATION_KIND_TYPEAS, EINVAL, "No type set for local!");
        type_as = tree->root->lchild->lchild;
    }

    // Set local type
    {
        IESTACK_SUPPOSE(type_as->rchild, EINVAL, "Existing type not provided for local!");
        IESTACK_SUPPOSE(type_as->rchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_TYPE, EINVAL, "Existing type not provided for local!");
        local_type = type_as->lchild->evaluates_to->type;
    }

    // Set local name
    {
        IESTACK_SUPPOSE(type_as->lchild, EINVAL, "Name used for local is not unused!");
        switch(type_as->lchild->evaluates_to->kind) {
        case VLTL_LANG_TOKEN_KIND_UNKNOWN:
            local_name = type_as->lchild->evaluates_to->unknown;
            break;
        case VLTL_LANG_TOKEN_KIND_CONSTANT:
            local_name = type_as->lchild->evaluates_to->constant->name;
            break;
        case VLTL_LANG_TOKEN_KIND_GLOBAL:
            local_name = type_as->lchild->evaluates_to->global->name;
            break;
        case VLTL_LANG_TOKEN_KIND_LOCAL:
            local_name = type_as->lchild->evaluates_to->local->name;
            break;
        case VLTL_LANG_TOKEN_KIND_FUNCTION:
            local_name = type_as->lchild->evaluates_to->function->name;
            break;
        default:
            IESTACK_RETURN(EINVAL, "Bad name for local variable!");
        }

        IESTACK_SUPPOSE(
            !vltl_lang_function_local_exists_here(vltl_global_context.function, local_name),
            EINVAL,
            "Name used for local is not unused!"
        );
    }

    size_t ignore_src_len = 0;
    char *copy_of_local_name = varena_alloc(&vltl_global_allocator, 1 + strlen(local_name));
    btrc_strncpy(&ignore_src_len, copy_of_local_name, local_name, strlen(local_name));

    IESTACK_SUPPOSE(vltl_global_context.function != NULL, EINVAL, "Cannot create local if not inside function!");

    Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
    if(created_literal == NULL) {
        ret = ENOMEM;
        IESTACK_RETURN2(vltl_global_errors, ret, "Could not allocate enough memory!");
    }
    *created_literal = (Vltl_lang_literal) {
        .name = NULL,
        .type = local_type,
        .attributes = { 0 },
        .fields = { 0 }
    };

    IESTACK_HANDLE(
        vltl_lang_function_local_set(
            vltl_global_context.function, copy_of_local_name, local_type,
            NULL, created_literal
        ),
        "Failed to set newly created local!"
    );

    tree->root = tree->root->lchild;
    tree->root->parent = NULL;
    tree->root->lchild = tree->root->lchild->lchild;
    tree->root->lchild->evaluates_to->kind = VLTL_LANG_TOKEN_KIND_LOCAL,
                                      vltl_lang_function_local_get(&(tree->root->lchild->evaluates_to->local), vltl_global_context.function, local_name);
    tree->root->lchild->parent = tree->root;

    *done_now = false;
    return 0;
}

int vltl_reshape_ast_tree_function(Vltl_ast_tree *tree, bool *done_now) {
    int ret = 0;
    const char *function_name = NULL;
    Vltl_ast_operation *parameters = NULL;

    IESTACK_SUPPOSE(tree != NULL, EINVAL, "Tree is NULL!");
    IESTACK_SUPPOSE(done_now != NULL, EINVAL, "Bool done_now is NULL!");

    // Get function name
    {
        IESTACK_SUPPOSE(tree->root->lchild, EINVAL, "Function has no name!");
        IESTACK_SUPPOSE(tree->root->lchild->kind == VLTL_AST_OPERATION_KIND_EVAL, EINVAL, "Function has no name!");
        IESTACK_SUPPOSE(tree->root->lchild->evaluates_to->kind == VLTL_LANG_TOKEN_KIND_UNKNOWN, EINVAL, "Function has no name!");
        function_name = tree->root->lchild->evaluates_to->unknown;
    }

    // Get parameters
    {
        IESTACK_SUPPOSE(tree->root->rchild, EINVAL, "Function has no arguments!");
        IESTACK_SUPPOSE(tree->root->rchild->lchild, EINVAL, "Function has no arguments!");
        IESTACK_SUPPOSE(
            tree->root->rchild->lchild->kind == VLTL_AST_OPERATION_KIND_TYPEAS ||
            tree->root->rchild->lchild->kind == VLTL_AST_OPERATION_KIND_CSV,
            EINVAL, "Function has no arguments!"
        );
        parameters = tree->root->rchild->lchild;
    }

    IESTACK_SUPPOSE(vltl_global_context.indentation_level == 0, ENOTRECOVERABLE, "Can't open function here!");
    vltl_global_context.indentation_level = 1;
    IESTACK_HANDLE(
        nkht_init(
            &(vltl_global_context.bodies[0].local_variables),
            sizeof(Vltl_lang_local *)
        ),
        "Could not initialize hash table for local variables!"
    );
    vltl_global_context.bodies[0].body_kind = VLTL_LANG_BODY_KIND_FUNCTION;

    Vltl_lang_function *created_function = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_function));
    if(created_function == NULL) {
        ret = ENOMEM;
        IESTACK_PUSH2(vltl_global_errors, ret, "Could not allocate enough memory!");
        return ret;
    }

    ret = vltl_lang_function_init(created_function, function_name);
    if(ret) {
        IESTACK_PUSH2(vltl_global_errors, ret, "Could not initialize lexer function!");
        return ret;
    }

    ret = nkht_set(vltl_global_table_functions, function_name, &created_function);
    if(ret) {
        IESTACK_PUSH2(vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
        return ret;
    }
    vltl_global_context.function = created_function;

    if(parameters->kind == VLTL_AST_OPERATION_KIND_TYPEAS) {
        Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
        if(created_literal == NULL) {
            ret = ENOMEM;
            IESTACK_PUSH2(vltl_global_errors, ret, "Could not allocate enough memory!");
            return ret;
        }
        *created_literal = (Vltl_lang_literal) {
            .name = NULL,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .fields = { (void *) 0 }
        };

        if(parameters->lchild->evaluates_to->unknown == NULL) {
            ret = EINVAL;
            IESTACK_PUSH2(vltl_global_errors, ret, "Unknown string pointer is NULL!");
            return ret;
        }
        ret = vltl_lang_function_local_set(
                  vltl_global_context.function, parameters->lchild->evaluates_to->unknown, &vltl_lang_type_long,
                  NULL, created_literal);
        if(ret) {
            IESTACK_PUSH2(vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
            return ret;
        }

        *(tree->root) = (Vltl_ast_operation) {
            .kind = VLTL_AST_OPERATION_KIND_CDECL1,
            .belongs_to = tree->root->belongs_to,
            .result_type = tree->root->result_type,
            .traced_by = tree->root->traced_by
        };
    } else if( parameters->kind == VLTL_AST_OPERATION_KIND_CSV) {
        for(size_t i = 0; i < vltl_ast_operation_argc(*(parameters)); i++) {
            Vltl_ast_operation *typeas_operation = parameters->arguments[i];

            Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
            if(created_literal == NULL) {
                ret = ENOMEM;
                IESTACK_PUSH2(vltl_global_errors, ret, "Could not allocate enough memory!");
                return ret;
            }
            *created_literal = (Vltl_lang_literal) {
                .name = NULL,
                .type = &vltl_lang_type_long,
                .attributes = { 0 },
                .fields = { (void *) 0 }
            };

            if(typeas_operation->lchild->evaluates_to->unknown == NULL) {
                ret = EINVAL;
                IESTACK_PUSH2(vltl_global_errors, ret, "Unknown string pointer is NULL!");
                return ret;
            }
            ret = vltl_lang_function_local_set(
                      vltl_global_context.function, typeas_operation->lchild->evaluates_to->unknown, &vltl_lang_type_long,
                      NULL, created_literal);
            if(ret) {
                IESTACK_PUSH2(vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
                return ret;
            }
        }

        size_t num_arguments = vltl_ast_operation_argc(*(parameters));
        *(tree->root) = (Vltl_ast_operation) {
            .belongs_to = tree->root->belongs_to,
            .result_type = tree->root->result_type,
            .traced_by = tree->root->traced_by
        };
        switch(num_arguments) {
        case 0:
            tree->root->kind = VLTL_AST_OPERATION_KIND_CDECL0;
            break;
        case 1:
            tree->root->kind = VLTL_AST_OPERATION_KIND_CDECL1;
            break;
        case 2:
            tree->root->kind = VLTL_AST_OPERATION_KIND_CDECL2;
            break;
        case 3:
            tree->root->kind = VLTL_AST_OPERATION_KIND_CDECL3;
            break;
        case 4:
            tree->root->kind = VLTL_AST_OPERATION_KIND_CDECL4;
            break;
        default:
            IESTACK_RETURN(ENOTRECOVERABLE, "Don't know that register!");
            break;
        }
    } else {
        IESTACK_RETURN(EINVAL, "Bad function!");
    }

    Vltl_lang_token *evaluates_to = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_token));
    evaluates_to->kind = VLTL_LANG_TOKEN_KIND_FUNCTION;
    evaluates_to->function = created_function;
    tree->root->evaluates_to = evaluates_to;

    *done_now = false;
    return 0;
}

int vltl_reshape_ast_tree_recurse_comma(Vltl_ast_tree *tree, Vltl_ast_operation *operation) {
    if(tree == NULL) {
        IESTACK_RETURN(EINVAL, "tree is NULL!");
    } else if(operation == NULL) {
        IESTACK_RETURN(EINVAL, "operation is NULL!");
    }

    operation->kind = VLTL_AST_OPERATION_KIND_CSV;
    size_t first_unused_index = 0;
    Vltl_ast_operation *lchild = operation->lchild;
    Vltl_ast_operation *rchild = operation->rchild;

    if(lchild->kind == VLTL_AST_OPERATION_KIND_CSV) {
        bool done = false;
        size_t i = 0;
        for(
            i = 0;
            !done &&
            i < VLTL_AST_OPERATION_ARGUMENTS_MAX &&
            first_unused_index < VLTL_AST_OPERATION_ARGUMENTS_MAX;
            i++
        ) {
            if(lchild->arguments[i] == NULL) {
                done = true;
                break;
            }

            operation->arguments[first_unused_index++] = lchild->arguments[i];
            lchild->arguments[i]->parent = operation;
        }
    } else {
        first_unused_index++;
    }

    if(first_unused_index >= VLTL_AST_OPERATION_ARGUMENTS_MAX) {
        IESTACK_RETURN(EXFULL, "Too many children of left CSV!");
    }

    if(rchild->kind == VLTL_AST_OPERATION_KIND_CSV) {
        bool done = false;
        size_t i = 0;
        for(
            i = 0;
            !done &&
            i < VLTL_AST_OPERATION_ARGUMENTS_MAX &&
            first_unused_index < VLTL_AST_OPERATION_ARGUMENTS_MAX;
            i++
        ) {
            if(rchild->arguments[i] == NULL) {
                done = true;
                break;
            }

            operation->arguments[first_unused_index++] = rchild->arguments[i];
            rchild->arguments[i]->parent = operation;
        }
    } else {
        operation->arguments[first_unused_index++] = rchild;
    }

    return 0;
}

int vltl_reshape_ast_tree_recurse(Vltl_ast_tree *tree, Vltl_ast_operation *operation) {
    int ret = 0;
    if(tree == NULL || operation == NULL) {
        IESTACK_RETURNF2(
            vltl_global_errors, EINVAL,
            "Arguments are NULL : tree = %p, operation = %p!",
            (void *) tree, (void *) operation
        );
    }

    for(size_t i = 0; i < vltl_ast_operation_argc(*operation); i++) {
        ret = vltl_reshape_ast_tree_recurse(tree, operation->arguments[i]);
        if(ret) {
            IESTACK_RETURN2(
                vltl_global_errors, EINVAL, "Unexpected error calling vltl_ast_tree_reshape_recurse!"
            );
        }
    }

    switch(operation->kind) {
    case VLTL_AST_OPERATION_KIND_COMMA:
        IESTACK_HANDLE(vltl_reshape_ast_tree_recurse_comma(tree, operation), "Failed to reshape comma operation!");
        break;
    default:
        break;
    }

    return 0;
}

int vltl_reshape_ast_tree(Vltl_ast_tree *tree, bool *done_now) {
    IESTACK_SUPPOSE(tree != NULL, EINVAL, "Tree is null!");
    IESTACK_SUPPOSE(done_now != NULL, EINVAL, "Boolean done_now is null!");

    *done_now = false;
    switch(tree->root->kind) {
    case VLTL_AST_OPERATION_KIND_EXTERNAL:
        IESTACK_HANDLE(
            vltl_reshape_ast_tree_external(tree, done_now),
            "Reshaping tree with external root node failed!"
        );
        break;
    case VLTL_AST_OPERATION_KIND_CONSTANT:
        IESTACK_HANDLE(
            vltl_reshape_ast_tree_constant(tree, done_now),
            "Reshaping tree with external root node failed!"
        );
        break;
    case VLTL_AST_OPERATION_KIND_GLOBAL:
        IESTACK_HANDLE(
            vltl_reshape_ast_tree_global(tree, done_now),
            "Reshaping tree with external root node failed!"
        );
        break;
    case VLTL_AST_OPERATION_KIND_LOCAL:
        IESTACK_HANDLE(
            vltl_reshape_ast_tree_local(tree, done_now),
            "Reshaping tree with external root node failed!"
        );
        break;
    case VLTL_AST_OPERATION_KIND_FUNCTION:
        IESTACK_HANDLE(vltl_reshape_ast_tree_recurse(tree, tree->root), "Failure recursively reshaping ast tree!");
        IESTACK_HANDLE(
            vltl_reshape_ast_tree_function(tree, done_now),
            "Reshaping tree with function root node failed!"
        );
        break;
    default:
        break;
    }

    if(!done_now) {
        IESTACK_HANDLE(vltl_reshape_ast_tree_recurse(tree, tree->root), "Failure recursively reshaping ast tree!");
    }

    return 0;
}
