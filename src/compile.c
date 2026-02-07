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
    if(dest == NULL || src == NULL || !vltl_sast_operation_valid(*src)) {
        return EINVAL;
    }

    // handle instruction
    Vltl_asm_instruction as_instruction = { 0 };
    {
        // only amd64 supported for now
        if(vltl_global_config.isa != VLTL_ISA_AMD64) {
            return ENOTRECOVERABLE;
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
        case VLTL_SAST_OPERATION_KIND_EVAL:
        // this is a psuedo-instruction
        default:
            return EINVAL;
            break;
        }

        ret = vltl_asm_instruction_stringify(dest, as_instruction);
        if(ret != 0) {
            return ret;
        }
    }

    // handle operands
    {
        // only amd64 supported for now
        if(vltl_global_config.isa != VLTL_ISA_AMD64) {
            return ENOTRECOVERABLE;
        }

        size_t this_operation_argc = vltl_sast_operation_argc(*src);
        for(size_t i = 0; i < this_operation_argc; i++) {
            const Vltl_sast_operation current_operation = *(src->arguments[i]);
            fputs(" ", dest);

            Vltl_asm_operand as_operand = { 0 };
            ret = vltl_compile_operation_operandify(&as_operand, current_operation);
            if(ret != 0) {
                return ret;
            }
            ret = vltl_asm_operand_stringify(dest, as_operand);
            if(ret != 0) {
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
        return EINVAL;
    }

    for(size_t i = 0; i < vltl_sast_operation_argc(*operation); i++) {
        Vltl_sast_operation *ith_operation = operation->arguments[i];
        if(ith_operation->kind == VLTL_SAST_OPERATION_KIND_EVAL) {
            continue;
        } else {
            ret = vltl_compile_convert_recurse(dest, src, ith_operation);
        }

        if(ret != 0) {
            return ret;
        }
    }

    ret = vltl_compile_operation_convert(dest, operation);
    if(ret != 0) {
        return ret;
    }
    return 0;
}

int vltl_compile_convert(FILE *dest, Vltl_sast_tree *src) {
    if(dest == NULL || src == NULL) {
        return EINVAL;
    }

    return vltl_compile_convert_recurse(dest, src, src->root);
}

int vltl_compile_file(char *dest_filename, char *src_filename) {
    int ret = 0;
    FILE *src_file = NULL;
    FILE *assembly_file = NULL;
    FILE *dest_file = NULL;
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
    char *assembly_filename = varena_alloc(&vltl_global_allocator, 2 * filename_cap);
    memcpy(&(assembly_filename[0]), dest_filename, dest_filename_len);
    memcpy(&(assembly_filename[dest_filename_len]), assembly_filename_extension, assembly_filename_extension_len);
    assembly_filename[dest_filename_len + assembly_filename_extension_len] = 0;
    assembly_file = fopen(assembly_filename, "w");
    if(!assembly_file) {
        goto vltl_compile_file_error;
    }

    dest_file = fopen(dest_filename, "w");
    if(!dest_file) {
        ret = EINVAL;
        goto vltl_compile_file_error;
    }

    const size_t big_buf_cap = 9999;
    char *big_buf = varena_alloc(&vltl_global_allocator, big_buf_cap);
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
    size_t line_number = 1;
    char *start_of_line = &(big_buf[0]);
    if(fgets(big_buf, big_buf_cap, src_file) == NULL) {
        done = true;
    }
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
            return ret;
        }

        while(
                nkht_iterate_next(
                    vltl_global_table_globals, &iterator, (void *) &iterated_global_key, &iterated_global_val
                    ) != ENODATA
             ) {
            if(iterated_global_key == NULL || iterated_global_val == NULL) {
                return EINVAL;
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
    {
        Nkht_iterator iterator = { 0 };
        char *iterated_constant_key = NULL;
        Vltl_lang_constant *iterated_constant_val = NULL;
        ret = nkht_iterate_start(vltl_global_table_constants, &iterator);
        if(ret) {
            return ret;
        }

        while(
                nkht_iterate_next(
                    vltl_global_table_constants, &iterator, (void *) &iterated_constant_key, &iterated_constant_val
                    ) != ENODATA
             ) {
            if(iterated_constant_key == NULL || iterated_constant_val == NULL) {
                return EINVAL;
            }

            fprintf(
                assembly_file,
                "%s: .long %ld\n",
                iterated_constant_key,
                (int64_t) iterated_constant_val->literal->fields[0]
            );
        }
    }

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
