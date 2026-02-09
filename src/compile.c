#include <ds/iestack.h>
#include <isa.h>
#include <compile.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int vltl_compile_operation_operandify(Vltl_asm_operand *dest, const Vltl_sast_operation operation) {
    if(dest == NULL || !vltl_sast_operation_valid(operation)) {
        return EINVAL;
    }

    switch(operation.kind) {
    case VLTL_SAST_OPERATION_KIND_EVAL:
        *dest = operation.evaluates_to;
        break;
    case VLTL_SAST_OPERATION_KIND_LOAD:
    case VLTL_SAST_OPERATION_KIND_STORE:
    case VLTL_SAST_OPERATION_KIND_ADD:
    case VLTL_SAST_OPERATION_KIND_SUB:
    case VLTL_SAST_OPERATION_KIND_RETURN:
        *dest = operation.destination;
        break;
    default:
        return EINVAL;
        break;
    }

    return 0;
}

int vltl_compile_operation_convert(FILE *dest, Vltl_sast_operation *src) {
    int ret = 0;
    if(dest == NULL || src == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : dest = %p, src = %p!",
            (void *) dest, (void *) src
        );
        return EINVAL;
    }

    if(!vltl_sast_operation_valid(*src)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "src is invalid!");
        return EINVAL;
    }

    // handle instruction
    Vltl_asm_instruction as_instruction = { 0 };
    {
        // only amd64 supported for now
        if(vltl_global_config.isa != VLTL_ISA_AMD64) {
            ret = ENOTRECOVERABLE;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unknown ISA so must fail!");
            return ret;
        }

        switch(src->kind) {
        case VLTL_SAST_OPERATION_KIND_LOAD:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_MOV;
            break;
        case VLTL_SAST_OPERATION_KIND_ADD:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_ADD;
            break;
        case VLTL_SAST_OPERATION_KIND_SUB:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_SUB;
            break;
        case VLTL_SAST_OPERATION_KIND_STORE:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_MOV;
            break;
        case VLTL_SAST_OPERATION_KIND_RETURN:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_RET;
            break;
        case VLTL_SAST_OPERATION_KIND_EVAL:
            // this is a psuedo-instruction
            return 0;
            break;
        default:
            ret = EINVAL;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unable to convert this sast_operation_kind!");
            return ret;
            break;
        }

        ret = vltl_asm_instruction_stringify(dest, as_instruction);
        if(ret != 0) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling vltl_asm_instruction_stringify!");
            return ret;
        }
    }

    // handle operands
    {
        // only amd64 supported for now
        if(vltl_global_config.isa != VLTL_ISA_AMD64) {
            ret = ENOTRECOVERABLE;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unknown ISA so fail!");
            return ret;
        }

        size_t this_operation_argc = vltl_sast_operation_argc(*src);
        bool this_operation_accepts_operands = true;
        switch(src->kind) {
        case VLTL_SAST_OPERATION_KIND_RETURN:
            this_operation_accepts_operands = false;
            break;
        default:
            break;
        }
        for(size_t i = 0; this_operation_accepts_operands && i < this_operation_argc; i++) {
            const Vltl_sast_operation current_operation = *(src->arguments[i]);
            fputs(" ", dest);

            Vltl_asm_operand as_operand = { 0 };
            ret = vltl_compile_operation_operandify(&as_operand, current_operation);
            if(ret != 0) {
                IESTACK_PUSH(
                    &vltl_global_errors, ret,
                    "Unexpected failure calling vltl_compile_operation_operandify!"
                );
                return ret;
            }
            ret = vltl_asm_operand_stringify(dest, as_operand);
            if(ret != 0) {
                IESTACK_PUSH(
                    &vltl_global_errors, ret,
                    "Unexpected failure calling vltl_asm_operand_stringify!"
                );
                return ret;
            }

            if(i + 1 < this_operation_argc) {
                fputs(",", dest);
            }
        }
    }

    fputs("\n", dest);

    return 0;
}

int vltl_compile_convert_recurse(FILE *dest, Vltl_sast_tree *src, Vltl_sast_operation *operation) {
    int ret = 0;
    if(dest == NULL || src == NULL || operation == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : dest = %p, src = %p, operation = %p!",
            (void *) dest, (void *) src, (void *) operation
        );
        return ret;
    }

    for(size_t i = 0; i < vltl_sast_operation_argc(*operation); i++) {
        Vltl_sast_operation *ith_operation = operation->arguments[i];
        if(ith_operation->kind == VLTL_SAST_OPERATION_KIND_EVAL) {
            continue;
        } else {
            ret = vltl_compile_convert_recurse(dest, src, ith_operation);
        }
    }

    ret = vltl_compile_operation_convert(dest, operation);
    if(ret != 0) {
        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure when calling vltl_compile_operation_convert!");
        return ret;
    }
    return 0;
}

int vltl_compile_convert(FILE *dest, Vltl_sast_tree *src) {
    int ret = 0;
    if(dest == NULL || src == NULL || src->root == NULL) {
        ret = EINVAL;
        IESTACK_PUSHF(
            &vltl_global_errors, ret,
            "Arguments are NULL : dest = %p, src = %p, src->root = %p!",
            (void *) dest, (void *) src, (void *) src->root
        );
        return ret;
    }

    switch(src->root->kind) {
    case VLTL_SAST_OPERATION_KIND_GLOBAL:
        // this is a psuedo-instruction
        ;

        Vltl_lang_global *created_global = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_global));
        Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
        if(created_global == NULL || created_literal == NULL) {
            ret = ENOMEM;
            IESTACK_PUSH(&vltl_global_errors, ret, "Could not allocate enough memory!");
            return ret;
        }
        *created_literal = (Vltl_lang_literal) {
            .name = NULL,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .fields = { (void *) src->root->evaluates_to.as_immediate.value }
        };
        *created_global = (Vltl_lang_global) {
            .name = src->root->destination.as_unknown,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .literal = created_literal
        };

        if(src->root->destination.as_unknown == NULL) {
            ret = EINVAL;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unknown string pointer is NULL!");
            return ret;
        }
        ret = nkht_set(vltl_global_table_globals, src->root->destination.as_unknown, &created_global);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
            return ret;
        }

        return 0;
        break;
    case VLTL_SAST_OPERATION_KIND_CONSTANT:
        // this is a psuedo-instruction
        ;

        Vltl_lang_constant *created_constant = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_constant));
        created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
        if(created_constant == NULL || created_literal == NULL) {
            ret = ENOMEM;
            IESTACK_PUSH(&vltl_global_errors, ret, "Could not allocate enough memory!");
            return ret;
        }
        *created_literal = (Vltl_lang_literal) {
            .name = NULL,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .fields = { (void *) src->root->evaluates_to.as_immediate.value }
        };
        *created_constant = (Vltl_lang_constant) {
            .name = src->root->destination.as_unknown,
            .type = &vltl_lang_type_long,
            .attributes = { 0 },
            .literal = created_literal
        };

        if(src->root->destination.as_unknown == NULL) {
            ret = EINVAL;
            IESTACK_PUSH(&vltl_global_errors, ret, "Unknown string pointer is NULL!");
            return ret;
        }
        ret = nkht_set(vltl_global_table_constants, src->root->destination.as_unknown, &created_constant);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
            return ret;
        }

        return 0;
        break;
    default:
        break;
    }

    return vltl_compile_convert_recurse(dest, src, src->root);
}

int vltl_compile_file(char *dest_filename, char *src_filename) {
    int ret = 0;
    FILE *src_file = NULL;
    FILE *assembly_file = NULL;
    char *assembly_filename = NULL;
    char *big_buf = NULL;
    char *debug_buf = NULL;
    FILE *dest_file = NULL;
    FILE *debug_file = NULL;
    const size_t filename_cap = 99;
    if(
        dest_filename == NULL || strlen(dest_filename) > filename_cap ||
        src_filename == NULL || strlen(src_filename) > filename_cap
    ) {
        ret = EINVAL;
        goto vltl_compile_file_error;
    }

    src_file = fopen(src_filename, "r");
    if(!src_file) {
        ret = EINVAL;
        goto vltl_compile_file_error;
    }

    const char *assembly_filename_extension = ".S";
    const size_t dest_filename_len = strlen(dest_filename);
    const size_t assembly_filename_extension_len = strlen(assembly_filename_extension);
    assembly_filename = varena_alloc(&vltl_global_allocator, 2 * filename_cap);
    memcpy(&(assembly_filename[0]), dest_filename, dest_filename_len);
    memcpy(&(assembly_filename[dest_filename_len]), assembly_filename_extension, assembly_filename_extension_len);
    assembly_filename[dest_filename_len + assembly_filename_extension_len] = 0;
    assembly_file = fopen(assembly_filename, "w");
    if(!assembly_file) {
        ret = ENOMEM;
        goto vltl_compile_file_error;
    }

    dest_file = fopen(dest_filename, "w");
    if(!dest_file) {
        ret = EINVAL;
        goto vltl_compile_file_error;
    }

    const size_t big_buf_cap = 9999;
    const size_t debug_buf_cap = 9999;
    size_t debug_buf_len = 0;
    big_buf = varena_alloc(&vltl_global_allocator, big_buf_cap);
    if(big_buf == NULL) {
        ret = ENOMEM;
        goto vltl_compile_file_error;
    }

    fputs(".intel_syntax\n", assembly_file);
    fputs("\n", assembly_file);
    fputs(".global main\n", assembly_file);
    fputs("\n", assembly_file);
    fputs(".text\n", assembly_file);
    fputs("main:\n", assembly_file);

    bool done = false;
    if(fgets(big_buf, big_buf_cap, src_file) == NULL) {
        done = true;
    }

    // old
    size_t line_number = 1;
    char *start_of_line = &(big_buf[0]);
    while(!done) {
        Vltl_lexer_line line = { 0 };
        Vltl_ast_tree ast_tree = { 0 };
        Vltl_sast_tree sast_tree = { 0 };

        // lexer
        ret = vltl_lexer_line_convert(&line, start_of_line);
        if(ret) {
            goto vltl_compile_file_error;
        }

        // ast tree
        ret = vltl_ast_tree_convert(&ast_tree, &line);
        if(ret) {
            debug_buf = varena_alloc(&vltl_global_allocator, debug_buf_cap);
            vltl_ast_tree_detokenize(debug_buf, debug_buf_cap, &debug_buf_len, ast_tree);
            debug_file = fopen("scratch/ast_debug.dot", "w");
            assert(debug_file != NULL);
            fputs(debug_buf, debug_file);
            fclose(debug_file);
            goto vltl_compile_file_error;
        }

        // sast tree
        ret = vltl_sast_tree_convert(&sast_tree, &ast_tree);
        if(ret) {
            debug_buf = varena_alloc(&vltl_global_allocator, debug_buf_cap);
            vltl_sast_tree_detokenize(debug_buf, debug_buf_cap, &debug_buf_len, sast_tree);
            debug_file = fopen("scratch/sast_debug.dot", "w");
            assert(debug_file != NULL);
            fputs(debug_buf, debug_file);
            fclose(debug_file);
            goto vltl_compile_file_error;
        }

        // compile
        fprintf(assembly_file, "// line #%lu\n", line_number++);
        ret = vltl_compile_convert(assembly_file, &sast_tree);
        fputs("\n", assembly_file);
        if(ret) {
            goto vltl_compile_file_error;
        }

        {
            if(fgets(big_buf, big_buf_cap, src_file) == NULL) {
                done = true;
            }
        }
    }

    fputs("mov %rax, %r11\n", assembly_file);
    fputs("ret\n", assembly_file);
    fputs("\n", assembly_file);

    fputs(".data\n", assembly_file);
    // Write all globals
    {
        Nkht_iterator iterator = { 0 };
        char *iterated_global_key = NULL;
        Vltl_lang_global *iterated_global_val = NULL;
        ret = nkht_iterate_start(vltl_global_table_globals, &iterator);
        if(ret) {
            goto vltl_compile_file_error;
        }

        while(
            nkht_iterate_next(
                vltl_global_table_globals, &iterator, (void *) &iterated_global_key, &iterated_global_val
            ) != ENODATA
        ) {
            if(iterated_global_key == NULL || iterated_global_val == NULL) {
                ret = ENOTRECOVERABLE;
                goto vltl_compile_file_error;
            }

            fprintf(
                assembly_file,
                "%s: .long %ld\n",
                iterated_global_key,
                (int64_t) iterated_global_val->literal->fields[0]
            );
        }
    }

    // Write all constants
    // Actually... don't
    /*
    {
        Nkht_iterator iterator = { 0 };
        char *iterated_constant_key = NULL;
        Vltl_lang_constant *iterated_constant_val = NULL;
        ret = nkht_iterate_start(vltl_global_table_constants, &iterator);
        if(ret) {
            goto vltl_compile_file_error;
        }

        while(
            nkht_iterate_next(
                vltl_global_table_constants, &iterator, (void *) &iterated_constant_key, &iterated_constant_val
            ) != ENODATA
        ) {
            if(iterated_constant_key == NULL || iterated_constant_val == NULL) {
                ret = ENOTRECOVERABLE;
                goto vltl_compile_file_error;
            }

            fprintf(
                assembly_file,
                "%s: .long %ld\n",
                iterated_constant_key,
                (int64_t) iterated_constant_val->literal->fields[0]
            );
        }
    }
    */

    // new
    /*
    done = false;
    while(!done) {
        Vltl_lexer_line line = { 0 };
        Vltl_ast_tree ast_tree = { 0 };
        Vltl_sast_tree sast_tree = { 0 };

        // lexer
        ret = vltl_lexer_line_convert(&line, start_of_line);
        if(ret) {
            goto vltl_compile_file_error;
        }

        if(inside function rn) {
            switch(first_token of line) {
            case "var":
            case "prelude":
            case "defer":
                // not yet supported
                ret = EINVAL;
                goto vltl_compile_file_error;
            case "}":
                // end of function
                // do something
                break;
            default:
                vqueue_enqueue(current_function_vqueue(current_function), &line);
                break;
            }
        } else {
            switch(first_token of line) {
            case "import":
            case "struct":
            case "func":
                // not yet supported
                ret = EINVAL;
                goto vltl_compile_file_error;
                break;
            case "const":
                vqueue_enqueue(&constant_lines, &line);
                break;
            case "global":
                vqueue_enqueue(&global_lines, &line);
                break;
            default:
                ret = EINVAL;
                goto vltl_compile_file_error;
                break;
            }
        }

        {
            if(fgets(big_buf, big_buf_cap, src_file) == NULL) {
                done = true;
            }
        }
    }

    for(auto lexer_line : import_lines) {
    }

    for(auto lexer_line : struct_lines) {
    }

    for(auto lexer_line : constant_lines) {
    }

    for(auto lexer_line : global_lines) {
    }

    for(auto lexer_function : lexer_functions) {
        for(auto variable_line : variable_lines) {
        }

        for(auto prelude_line : prelude_lines) {
        }

        for(auto normal_line : normal_lines) {
        }

        for(auto defer_line : defer_lines) {
        }
    }

    for(auto lexer_line : sorted_lines) {
        // ast tree
        ret = vltl_ast_tree_convert(&ast_tree, &line);
        if(ret) {
            goto vltl_compile_file_error;
        }
        //vltl_ast_tree_detokenize(buf, 999, &buf_len, ast_tree);
        //fputs(buf, file);

        // sast tree
        ret = vltl_sast_tree_convert(&sast_tree, &ast_tree);
        if(ret) {
            goto vltl_compile_file_error;
        }
        //vltl_sast_tree_detokenize(buf, 999, &buf_len, sast_tree);
        //fputs(buf, file);

        // compile
        fprintf(assembly_file, "// line #%lu\n", line_number++);
        ret = vltl_compile_convert(assembly_file, &sast_tree);
        fputs("\n", assembly_file);
        if(ret) {
            goto vltl_compile_file_error;
        }
    }
    */

    // Once I get more of an idea of how I want to handle symbols/linking then can write code to fork and call gcc
    //const char *gcc_path = "/bin/gcc";
    //char *gcc_argv[4] = {"/bin/gcc", assembly_filename, "-o", dest_filename};
    //char **gcc_envp = NULL;
    //if(!fork()) {
    //  ret = execve(gcc_path, gcc_argv, gcc_envp);
    //} else {
    //  wait for child
    //}
    //if(ret) {
    //goto vltl_compile_file_error;
    //}

vltl_compile_file_error:
    if(ret) {
        iestack_dump(&vltl_global_errors, stdout);
    }
    if(src_file) {
        fflush(src_file);
        fclose(src_file);
        src_file = NULL;
    }
    if(big_buf) {
        // do nothing
        big_buf = NULL;
    }
    if(assembly_filename) {
        // do nothing
        assembly_filename = NULL;
    }
    if(assembly_file) {
        fflush(assembly_file);
        fclose(assembly_file);
        assembly_file = NULL;
    }
    if(dest_file) {
        fflush(dest_file);
        fclose(dest_file);
        dest_file = NULL;
    }

    return ret;
}
