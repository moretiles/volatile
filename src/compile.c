#include <ds/iestack.h>
#include <lang/function.h>
#include <lang/body.h>
#include <isa.h>
#include <compile.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

size_t vltl_compile_line_trio_queue_advise(size_t num_elems) {
    return sizeof(Vltl_compile_line_trio_queue) + vqueue_advise(sizeof(Vltl_compile_line_trio), num_elems);
}

int vltl_compile_line_trio_queue_init(Vltl_compile_line_trio_queue **dest, void *memory, size_t num_elems) {
    Vltl_compile_line_trio_queue *queue = NULL;
    int ret = 0;
    char *memory_as_chars = memory;

    if(dest == NULL || memory == NULL || num_elems == 0) {
        return EINVAL;
    }

    queue = (void *) &(memory_as_chars[0]);
    ret = vqueue_init(&(queue->trio_queue), &(memory_as_chars[sizeof(Vltl_compile_line_trio_queue)]), sizeof(Vltl_compile_line_trio), num_elems);
    if(ret) {
        return ret;
    }
    *dest = queue;
    return 0;
}

int vltl_compile_line_trio_queue_enqueue(Vltl_compile_line_trio_queue *queue, Vltl_compile_line_trio *src) {
    if(queue == NULL || src == NULL) {
        return EINVAL;
    }

    return vqueue_enqueue(queue->trio_queue, src, false);
}

int vltl_compile_line_trio_queue_dequeue(Vltl_compile_line_trio_queue *queue, Vltl_compile_line_trio *dest) {
    if(queue == NULL || dest == NULL) {
        return EINVAL;
    }

    return vqueue_dequeue(queue->trio_queue, dest);
}

void vltl_compile_line_trio_queue_deinit(Vltl_compile_line_trio_queue *queue) {
    if(queue == NULL) {
        return;
    }

    vqueue_deinit(queue->trio_queue);
    queue->trio_queue = NULL;
    return;
}

int vltl_compile_operation_operandify(Vltl_asm_operand *dest, const Vltl_sast_operation operation) {
    if(dest == NULL || !vltl_sast_operation_valid(operation)) {
        return EINVAL;
    }

    switch(operation.kind) {
    case VLTL_SAST_OPERATION_KIND_EVAL:
        *dest = operation.evaluates_to;
        break;
    case VLTL_SAST_OPERATION_KIND_CALL:
    case VLTL_SAST_OPERATION_KIND_GROUPING_OPEN:
    case VLTL_SAST_OPERATION_KIND_GROUPING_CLOSE:
    case VLTL_SAST_OPERATION_KIND_COMMA:
    case VLTL_SAST_OPERATION_KIND_LOAD:
    case VLTL_SAST_OPERATION_KIND_STORE:
    case VLTL_SAST_OPERATION_KIND_ADD:
    case VLTL_SAST_OPERATION_KIND_SUB:
    case VLTL_SAST_OPERATION_KIND_MUL:
    case VLTL_SAST_OPERATION_KIND_DIV:
    case VLTL_SAST_OPERATION_KIND_TEST_EQUALS:
    case VLTL_SAST_OPERATION_KIND_RETURN:
    case VLTL_SAST_OPERATION_KIND_IF:
    case VLTL_SAST_OPERATION_KIND_ELIF:
    case VLTL_SAST_OPERATION_KIND_WHILE:
        *dest = operation.destination;
        break;
    default:
        return EINVAL;
        break;
    }

    return 0;
}

int vltl_compile_operation_convert_label(FILE *dest, Vltl_sast_operation *src) {
    size_t label_value = 0;
    Vltl_asm_operand as_operand = { 0 };
    Vltl_lang_body_kind body_kind = { 0 };
    size_t ignore_len = 0;
    const size_t operand_buf_cap = 99;
    char src1_operand_buf[99];
    char src2_operand_buf[99];
    char dest_operand_buf[99];
    const char *fstring = NULL;

    VLTL_SUPPOSE(dest != NULL, EINVAL, "dest is NULL!");
    VLTL_SUPPOSE(src != NULL, EINVAL, "src is NULL!");

    switch(src->kind) {
    case VLTL_SAST_OPERATION_KIND_BODY_OPEN:
        VLTL_SUPPOSE(
            vltl_global_context.indentation_level < (VLTL_LANG_BODY_CAP + 1),
            ENOTRECOVERABLE,
            "Body nesting depth exceeded!"
        );

        vltl_global_context.indentation_level += 1;
        VLTL_EXPECT(
            nkht_init(
                &(vltl_global_context.bodies[vltl_global_context.indentation_level - 1].local_variables),
                sizeof(Vltl_lang_local *)
            ),
            "Could not initialize hash table for local variables!"
        );

    label_value = (vltl_global_context.indentation_level - 1) * VLTL_LANG_BODY_LABEL_ITERATE;
    if(src->parent == NULL) {
        label_value += VLTL_LANG_BODY_LABEL_BARE_OPEN;
        body_kind = VLTL_LANG_BODY_KIND_BARE;
    } else {
        switch(src->parent->kind){
        case VLTL_SAST_OPERATION_KIND_FUNCTION:
            label_value += VLTL_LANG_BODY_LABEL_FUNCTION_OPEN;
            body_kind = VLTL_LANG_BODY_KIND_FUNCTION;
            break;
        case VLTL_SAST_OPERATION_KIND_IF:
            label_value += VLTL_LANG_BODY_LABEL_IF_OPEN;
            body_kind = VLTL_LANG_BODY_KIND_IF;
            break;
        case VLTL_SAST_OPERATION_KIND_ELIF:
            label_value += VLTL_LANG_BODY_LABEL_ELIF_OPEN;
            body_kind = VLTL_LANG_BODY_KIND_ELIF;
            break;
        case VLTL_SAST_OPERATION_KIND_ELSE:
            label_value += VLTL_LANG_BODY_LABEL_ELSE_OPEN;
            body_kind = VLTL_LANG_BODY_KIND_ELSE;
            break;
        case VLTL_SAST_OPERATION_KIND_WHILE:
            label_value += VLTL_LANG_BODY_LABEL_WHILE_OPEN;
            body_kind = VLTL_LANG_BODY_KIND_WHILE;
            break;
        default:
            VLTL_RETURN(ENOTRECOVERABLE, "Some invalid operation owns a body_open operation!");
            break;
        }
    }
        vltl_global_context.bodies[vltl_global_context.indentation_level - 1].body_kind = body_kind;
        fprintf(dest, "%lu:\n", label_value);

        break;
    case VLTL_SAST_OPERATION_KIND_BODY_CLOSE:
        ;

        // TODO: Fully implement functions
        VLTL_SUPPOSE(vltl_global_context.indentation_level > 0, ENOTRECOVERABLE, "Can't close unopened body!");
        nkht_deinit(&(vltl_global_context.bodies[vltl_global_context.indentation_level - 1].local_variables));
        vltl_global_context.indentation_level -= 1;
        if(vltl_global_context.indentation_level == 0) {
            label_value = vltl_global_context.indentation_level * VLTL_LANG_BODY_LABEL_ITERATE;
            VLTL_SUPPOSE(
                vltl_global_context.bodies[0].body_kind == VLTL_LANG_BODY_KIND_FUNCTION,
                ENOTRECOVERABLE,
                "Uhh it looks like something went horribly wrong in connection with this function..."
            );
            vltl_lang_function_deinit(vltl_global_context.function);
            vltl_global_context.function = NULL;
            label_value += VLTL_LANG_BODY_LABEL_FUNCTION_CLOSE;
            fprintf(dest, "%lu:\n", label_value);
        } else {
            label_value = vltl_global_context.indentation_level * VLTL_LANG_BODY_LABEL_ITERATE;
            switch(vltl_global_context.bodies[vltl_global_context.indentation_level].body_kind) {
            case VLTL_LANG_BODY_KIND_IF:
            case VLTL_LANG_BODY_KIND_ELIF:
            case VLTL_LANG_BODY_KIND_WHILE:
                label_value += VLTL_LANG_BODY_LABEL_IFELIFELSE_CLOSE;
                if(src->parent == NULL) {
                    fprintf(dest, "%lu:\n", label_value);
                } else {
                    fprintf(dest, "jmp %luf\n", label_value);
                }
                break;
            case VLTL_LANG_BODY_KIND_ELSE:
                label_value += VLTL_LANG_BODY_LABEL_IFELIFELSE_CLOSE;
                    fprintf(dest, "%lu:\n", label_value);
                    break;
            default:
                break;
            }

            label_value = vltl_global_context.indentation_level * VLTL_LANG_BODY_LABEL_ITERATE;
            switch(vltl_global_context.bodies[vltl_global_context.indentation_level].body_kind) {
            case VLTL_LANG_BODY_KIND_BARE:
                label_value += VLTL_LANG_BODY_LABEL_BARE_CLOSE;
                break;
            case VLTL_LANG_BODY_KIND_IF:
                label_value += VLTL_LANG_BODY_LABEL_IF_CLOSE;
                break;
            case VLTL_LANG_BODY_KIND_ELIF:
                label_value += VLTL_LANG_BODY_LABEL_ELIF_CLOSE;
                break;
            case VLTL_LANG_BODY_KIND_ELSE:
                label_value += VLTL_LANG_BODY_LABEL_ELIF_CLOSE;
                break;
            case VLTL_LANG_BODY_KIND_WHILE:
                // Unconditional jump to start of loop because this is a loop
                label_value += VLTL_LANG_BODY_LABEL_WHILE_OPEN;
                fprintf(dest, "jmp %lub\n", label_value);
                label_value -= VLTL_LANG_BODY_LABEL_WHILE_OPEN;

                label_value += VLTL_LANG_BODY_LABEL_WHILE_CLOSE;
                break;
            case VLTL_LANG_BODY_KIND_FUNCTION:
                VLTL_RETURN(ENOTRECOVERABLE, "Invalid use of nested function... maybe!");
                break;
            default:
                VLTL_RETURN(ENOTRECOVERABLE, "Some invalid operation once owned a body_open operation... maybe!");
                break;
            }
            fprintf(dest, "%lu:\n", label_value);
        }

        break;
    case VLTL_SAST_OPERATION_KIND_IF:
        label_value = (vltl_global_context.indentation_level - 1) * VLTL_LANG_BODY_LABEL_ITERATE;
        label_value += VLTL_LANG_BODY_LABEL_IF_CLOSE;
        fstring = "test %s, %s\n"
                  "\tjz %luf\n";

        VLTL_EXPECT(
            vltl_compile_operation_operandify(&as_operand, *src), "Could not operandify!"
        );
        VLTL_EXPECT(
            vltl_asm_operand_detokenize(dest_operand_buf, operand_buf_cap, &ignore_len, as_operand), "Could not detokenzie!"
        );
        fprintf(
            dest, fstring, dest_operand_buf, dest_operand_buf, label_value
        );

        break;
    case VLTL_SAST_OPERATION_KIND_ELIF:
        label_value = (vltl_global_context.indentation_level - 1) * VLTL_LANG_BODY_LABEL_ITERATE;
        label_value += VLTL_LANG_BODY_LABEL_ELIF_CLOSE;

        fstring = "test %s, %s\n"
                  "\tjz %luf\n";

        VLTL_EXPECT(
            vltl_compile_operation_operandify(&as_operand, *src), "Could not operandify!"
        );
        VLTL_EXPECT(
            vltl_asm_operand_detokenize(dest_operand_buf, operand_buf_cap, &ignore_len, as_operand), "Could not detokenzie!"
        );
        fprintf(
            dest, fstring, dest_operand_buf, dest_operand_buf, label_value
        );

        break;
    case VLTL_SAST_OPERATION_KIND_ELSE:
        break;
    case VLTL_SAST_OPERATION_KIND_WHILE:
        label_value = (vltl_global_context.indentation_level - 1) * VLTL_LANG_BODY_LABEL_ITERATE;
        label_value += VLTL_LANG_BODY_LABEL_WHILE_CLOSE;

        fstring = "test %s, %s\n"
                  "\tjz %luf\n";

        VLTL_EXPECT(
            vltl_compile_operation_operandify(&as_operand, *src), "Could not operandify!"
        );
        VLTL_EXPECT(
            vltl_asm_operand_detokenize(dest_operand_buf, operand_buf_cap, &ignore_len, as_operand), "Could not detokenzie!"
        );
        fprintf(
            dest, fstring, dest_operand_buf, dest_operand_buf, label_value
        );

        break;
    case VLTL_SAST_OPERATION_KIND_TEST_EQUALS:
        fstring = "cmp %s, %s\n"
                              "\tmov %s, 0\n"
                              "\tmov %s, 1\n"
                              "\tcmove %s, %s\n";

        VLTL_EXPECT(
            vltl_compile_operation_operandify(&as_operand, *(src->lchild)), "Could not operandify!"
        );
        VLTL_EXPECT(
            vltl_asm_operand_detokenize(src1_operand_buf, operand_buf_cap, &ignore_len, as_operand), "Could not detokenzie!"
        );
        VLTL_EXPECT(
            vltl_compile_operation_operandify(&as_operand, *(src->rchild)), "Could not operandify!"
        );
        VLTL_EXPECT(
            vltl_asm_operand_detokenize(src2_operand_buf, operand_buf_cap, &ignore_len, as_operand), "Could not detokenzie!"
        );
        VLTL_EXPECT(
            vltl_compile_operation_operandify(&as_operand, *src), "Could not operandify!"
        );
        VLTL_EXPECT(
            vltl_asm_operand_detokenize(dest_operand_buf, operand_buf_cap, &ignore_len, as_operand), "Could not detokenzie!"
        );
        fprintf(
            dest, fstring,
            src1_operand_buf, src2_operand_buf,
            dest_operand_buf,
            src2_operand_buf,
            dest_operand_buf, src2_operand_buf
        );

        break;
    default:
        VLTL_RETURN(EINVAL, "Cannot turn this kind of sast_operation into a label!");
    }

    return 0;
}

int vltl_compile_operation_convert_instruction(FILE *dest, Vltl_sast_operation *src) {
    int ret = 0;
    VLTL_SUPPOSE(dest != NULL, EINVAL, "dest is NULL!");
    VLTL_SUPPOSE(src != NULL, EINVAL, "src is NULL!");

    // handle instruction
    Vltl_asm_instruction as_instruction = { 0 };
    {
        switch(src->kind) {
        case VLTL_SAST_OPERATION_KIND_CALL:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_CALL;
            break;
        case VLTL_SAST_OPERATION_KIND_COMMA:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_NOP;
            break;
        case VLTL_SAST_OPERATION_KIND_GROUPING_OPEN:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_NOP;
            break;
        case VLTL_SAST_OPERATION_KIND_GROUPING_CLOSE:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_NOP;
            break;
        case VLTL_SAST_OPERATION_KIND_CSV:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_NOP;
            break;
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
        case VLTL_SAST_OPERATION_KIND_MUL:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_IMUL;
            break;
        case VLTL_SAST_OPERATION_KIND_DIV:
            as_instruction.instruction_kind = VLTL_ASM_INSTRUCTION_KIND_AMD64;
            as_instruction.as_amd64 = VLTL_ASM_INSTRUCTION_AMD64_IDIV;
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

        size_t this_operation_argc = vltl_sast_operation_args_argc(*src);
        // some operations make use of operands implicitly, so don't print them
        bool never_print_this_argument[VLTL_SAST_OPERATION_ARGUMENTS_MAX] = { 0 };

        switch(src->kind) {
        case VLTL_SAST_OPERATION_KIND_CALL:
            never_print_this_argument[1] = true;
            break;
        case VLTL_SAST_OPERATION_KIND_GROUPING_OPEN:
            never_print_this_argument[0] = true;
            never_print_this_argument[1] = true;
            break;
        case VLTL_SAST_OPERATION_KIND_COMMA:
            never_print_this_argument[0] = true;
            never_print_this_argument[1] = true;
            break;
        case VLTL_SAST_OPERATION_KIND_CSV:
            for(size_t i = 0; i < VLTL_SAST_OPERATION_ARGUMENTS_MAX; i++) {
                never_print_this_argument[i] = true;
            }
            break;
        case VLTL_SAST_OPERATION_KIND_RETURN:
            never_print_this_argument[0] = true;
            break;
        case VLTL_SAST_OPERATION_KIND_DIV:
            never_print_this_argument[0] = true;
            break;
        default:
            break;
        }
        for(size_t i = 0; i < this_operation_argc; i++) {
            if(never_print_this_argument[i]) {
                continue;
            }

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

            if((i + 1) < this_operation_argc && !never_print_this_argument[i + 1]) {
                fputs(",", dest);
            }
        }
    }

    fputs("\n", dest);

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

    // only amd64 supported for now
    if(vltl_global_config.isa != VLTL_ISA_AMD64) {
        ret = ENOTRECOVERABLE;
        IESTACK_PUSH(&vltl_global_errors, ret, "Unknown ISA so must fail!");
        return ret;
    }


    if(!vltl_sast_operation_valid(*src)) {
        ret = EINVAL;
        IESTACK_PUSH(&vltl_global_errors, ret, "src is invalid!");
        return EINVAL;
    }

    switch(src->kind) {
    case VLTL_SAST_OPERATION_KIND_BODY_OPEN:
    case VLTL_SAST_OPERATION_KIND_BODY_CLOSE:
    case VLTL_SAST_OPERATION_KIND_IF:
    case VLTL_SAST_OPERATION_KIND_ELIF:
    case VLTL_SAST_OPERATION_KIND_ELSE:
    case VLTL_SAST_OPERATION_KIND_WHILE:
    case VLTL_SAST_OPERATION_KIND_TEST_EQUALS:
        return vltl_compile_operation_convert_label(dest, src);
        break;
    default:
        return vltl_compile_operation_convert_instruction(dest, src);
        break;
    }
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

    for(size_t i = 0; i < vltl_sast_operation_args_argc(*operation); i++) {
        Vltl_sast_operation *ith_operation = operation->arguments[i];
        if(ith_operation->kind == VLTL_SAST_OPERATION_KIND_EVAL) {
            continue;
        } else {
            ret = vltl_compile_convert_recurse(dest, src, ith_operation);
            if(ret) {
                return ret;
            }
        }
    }

    for(size_t i = 0; i < vltl_sast_operation_before_argc(*operation); i++) {
        Vltl_sast_operation *ith_operation = operation->before[i];
        ret = vltl_compile_convert_recurse(dest, src, ith_operation);
        if(ret) {
            return ret;
        }
    }

    // indentation because this must be inside of a function
    fputs("\t", dest);

    VLTL_EXPECT(vltl_compile_operation_convert(dest, operation), "Failed to write compiled expression to dest!");

    for(size_t i = 0; i < vltl_sast_operation_after_argc(*operation); i++) {
        Vltl_sast_operation *ith_operation = operation->after[i];
        ret = vltl_compile_convert_recurse(dest, src, ith_operation);
        if(ret) {
            return ret;
        }
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
    case VLTL_SAST_OPERATION_KIND_FUNCTION:
        // this is a psuedo-instruction
        ;

        // TODO: Fully implement functions
        // Need to figure out how I want to store function and what is associated with it
        VLTL_SUPPOSE(vltl_global_context.indentation_level == 0, ENOTRECOVERABLE, "Can't open function here!");
        vltl_global_context.indentation_level = 1;
        VLTL_EXPECT(
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
            IESTACK_PUSH(&vltl_global_errors, ret, "Could not allocate enough memory!");
            return ret;
        }

        ret = vltl_lang_function_init(created_function, src->root->evaluates_to.as_unknown);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Could not initialize lexer function!");
            return ret;
        }

        ret = nkht_set(vltl_global_table_functions, src->root->evaluates_to.as_unknown, &created_function);
        if(ret) {
            IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
            return ret;
        }
        vltl_global_context.function = created_function;
        fputs(src->root->evaluates_to.as_unknown, dest);
        fputs(":\n", dest);

        // TODO: handle decrementing for storage of locals with a given function in a smart way
        fputs("\tpush %rbp\n", dest);
        fputs("\tpush %rbx\n", dest);
        fputs("\tpush %r12\n", dest);
        fputs("\tpush %r13\n", dest);
        fputs("\tpush %r14\n", dest);
        fputs("\tpush %r15\n", dest);
        fputs("\tmov %rbp, %rsp\n", dest);
        fputs("\tsub %rsp, 0x400\n", dest);

        if(src->root->rchild && src->root->rchild->lchild) {
            switch(src->root->rchild->lchild->kind) {
            case(VLTL_SAST_OPERATION_KIND_TYPEAS):
                ;
                Vltl_sast_operation *typeas_operation = src->root->rchild->lchild;

                Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
                if(created_literal == NULL) {
                    ret = ENOMEM;
                    IESTACK_PUSH(&vltl_global_errors, ret, "Could not allocate enough memory!");
                    return ret;
                }
                *created_literal = (Vltl_lang_literal) {
                    .name = NULL,
                    .type = &vltl_lang_type_long,
                    .attributes = { 0 },
                    .fields = { (void *) 0 }
                };

                if(typeas_operation->lchild->evaluates_to.as_unknown == NULL) {
                    ret = EINVAL;
                    IESTACK_PUSH(&vltl_global_errors, ret, "Unknown string pointer is NULL!");
                    return ret;
                }
                ret = vltl_lang_function_local_set(
                          vltl_global_context.function, typeas_operation->lchild->evaluates_to.as_unknown, &vltl_lang_type_long,
                          NULL, created_literal);
                if(ret) {
                    IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
                    return ret;
                }

                fputs("\tmov -8[%rbp], %rdi\n", dest);
                break;
            case(VLTL_SAST_OPERATION_KIND_CSV):
                for(size_t i = 0; i < vltl_sast_operation_args_argc(*(src->root->rchild->lchild)); i++) {
                    Vltl_sast_operation *typeas_operation = src->root->rchild->lchild->arguments[i];

                    Vltl_lang_literal *created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
                    if(created_literal == NULL) {
                        ret = ENOMEM;
                        IESTACK_PUSH(&vltl_global_errors, ret, "Could not allocate enough memory!");
                        return ret;
                    }
                    *created_literal = (Vltl_lang_literal) {
                        .name = NULL,
                        .type = &vltl_lang_type_long,
                        .attributes = { 0 },
                        .fields = { (void *) 0 }
                    };

                    if(typeas_operation->lchild->evaluates_to.as_unknown == NULL) {
                        ret = EINVAL;
                        IESTACK_PUSH(&vltl_global_errors, ret, "Unknown string pointer is NULL!");
                        return ret;
                    }
                    ret = vltl_lang_function_local_set(
                              vltl_global_context.function, typeas_operation->lchild->evaluates_to.as_unknown, &vltl_lang_type_long,
                              NULL, created_literal);
                    if(ret) {
                        IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
                        return ret;
                    }

                    switch(i) {
                    case 0:
                        fputs("\tmov -8[%rbp], %rdi\n", dest);
                        break;
                    case 1:
                        fputs("\tmov -16[%rbp], %rsi\n", dest);
                        break;
                    case 2:
                        fputs("\tmov -24[%rbp], %rdx\n", dest);
                        break;
                    case 3:
                        fputs("\tmov -32[%rbp], %rcx\n", dest);
                        break;
                    default:
                        VLTL_RETURN(ENOTRECOVERABLE, "Don't know that register!");
                        break;
                    }
                }
            default:
                break;
            }
        }

        return 0;
        break;
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
    case VLTL_SAST_OPERATION_KIND_LOCAL:
        // this is a psuedo-instruction
        ;

        if(vltl_global_context.function == NULL) {
            return EINVAL;
        }

        created_literal = varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_literal));
        if(created_literal == NULL) {
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

        switch(src->root->destination.kind) {
        case VLTL_ASM_OPERAND_KIND_MEMORY:
            if(src->root->destination.as_memory.memory_kind != VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL) {
                ret = EINVAL;
                IESTACK_PUSH(
                    &vltl_global_errors, ret, "Trying to create aliased local for something not a local!"
                );
                return ret;
            }
            ret = vltl_lang_function_local_set(
                      vltl_global_context.function, src->root->destination.as_memory.name, &vltl_lang_type_long,
                      NULL, created_literal
            );
            if(ret) {
                IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
                return ret;
            }
    
            Vltl_sast_operation *child_with_name_of_local = src->root->lchild->lchild;
            *(child_with_name_of_local) = (Vltl_sast_operation) {
                .kind = VLTL_SAST_OPERATION_KIND_EVAL,
                .belongs_to = child_with_name_of_local->belongs_to,
                .parent = child_with_name_of_local->parent,
                .evaluates_to = (Vltl_asm_operand) {
                    .kind = VLTL_ASM_OPERAND_KIND_MEMORY,
                    .as_memory = (Vltl_asm_operand_memory) {
                        .memory_kind = VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL,
                        .name = child_with_name_of_local->evaluates_to.as_memory.name,
                        .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64
                    }
                }
            };
            break;
        case VLTL_ASM_OPERAND_KIND_UNKNOWN:
            if(src->root->destination.as_unknown == NULL) {
                ret = EINVAL;
                IESTACK_PUSH(&vltl_global_errors, ret, "Unknown string pointer is NULL!");
                return ret;
            }
            ret = vltl_lang_function_local_set(
                      vltl_global_context.function, src->root->destination.as_unknown, &vltl_lang_type_long,
                      NULL, created_literal
            );
            if(ret) {
                IESTACK_PUSH(&vltl_global_errors, ret, "Unexpected failure calling nkht_set!");
                return ret;
            }
    
            child_with_name_of_local = src->root->lchild->lchild;
            *(child_with_name_of_local) = (Vltl_sast_operation) {
                .kind = VLTL_SAST_OPERATION_KIND_EVAL,
                .belongs_to = child_with_name_of_local->belongs_to,
                .parent = child_with_name_of_local->parent,
                .evaluates_to = (Vltl_asm_operand) {
                    .kind = VLTL_ASM_OPERAND_KIND_MEMORY,
                    .as_memory = (Vltl_asm_operand_memory) {
                        .memory_kind = VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL,
                        .name = child_with_name_of_local->evaluates_to.as_unknown,
                        .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64
                    }
                }
            };
            break;
        default:
            VLTL_RETURN(EINVAL, "Tried to create invalid local!");
        }

        return vltl_compile_convert_recurse(dest, src, src->root->lchild);
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

int vltl_compile_line(FILE *dest, const char *src_line, size_t line_number) {
    if(dest == NULL || src_line == NULL) {
        return EINVAL;
    }

    int ret = 0;

    Vltl_lexer_line line = { 0 };
    Vltl_ast_tree ast_tree = { 0 };
    Vltl_sast_tree sast_tree = { 0 };

    char *debug_buf = NULL;
    const size_t debug_buf_cap = 9999;
    size_t debug_buf_len = 0;
    FILE *debug_file = NULL;

    // lexer
    ret = vltl_lexer_line_convert(&line, src_line);
    if(ret) {
        return ret;
    }

    if(line.token_count == 0) {
        fprintf(dest, "// line #%lu, indentation %lu\n", line_number, vltl_global_context.indentation_level);
        fputs("\n", dest);
        return 0;
    }

    // ast tree
    ret = vltl_ast_tree_convert(&ast_tree, &line);
    // TODO: figure out better way to conditionally enable dumping of ast_tree to graphviz
    if(true || ret) {
        debug_buf = varena_alloc(&vltl_global_allocator, debug_buf_cap);
        vltl_ast_tree_detokenize(debug_buf, debug_buf_cap, &debug_buf_len, ast_tree);
        debug_file = fopen("scratch/ast_debug.dot", "w");
        assert(debug_file != NULL);
        fputs(debug_buf, debug_file);
        fclose(debug_file);
        if(ret) {
            return ret;
        }
    }

    // sast tree
    ret = vltl_sast_tree_convert(&sast_tree, &ast_tree);
    // TODO: figure out better way to conditionally enable dumping of sast_tree to graphviz
    if(true || ret) {
        debug_buf = varena_alloc(&vltl_global_allocator, debug_buf_cap);
        vltl_sast_tree_detokenize(debug_buf, debug_buf_cap, &debug_buf_len, sast_tree);
        debug_file = fopen("scratch/sast_debug.dot", "w");
        assert(debug_file != NULL);
        fputs(debug_buf, debug_file);
        fclose(debug_file);
        if(ret) {
            return ret;
        }
    }

    // compile
    fprintf(dest, "// line #%lu, indentation %lu\n", line_number, vltl_global_context.indentation_level);
    ret = vltl_compile_convert(dest, &sast_tree);
    fputs("\n", dest);
    if(ret) {
        return ret;
    }

    return ret;
}

int vltl_compile_file(char *dest_filename, char *src_filename) {
    int ret = 0;
    FILE *src_file = NULL;
    FILE *assembly_file = NULL;
    char *assembly_filename = NULL;
    char *big_buf = NULL;
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

    Vltl_compile_line_trio_queue *import_lines = varena_alloc(&vltl_global_allocator, vltl_compile_line_trio_queue_advise(20));
    ret = vltl_compile_line_trio_queue_init(&import_lines, import_lines, 20);
    if(ret) {
        goto vltl_compile_file_error;
    }
    Vltl_compile_line_trio_queue *struct_lines = varena_alloc(&vltl_global_allocator, vltl_compile_line_trio_queue_advise(20));
    ret = vltl_compile_line_trio_queue_init(&struct_lines, struct_lines, 20);
    if(ret) {
        goto vltl_compile_file_error;
    }
    Vltl_compile_line_trio_queue *constant_lines = varena_alloc(&vltl_global_allocator, vltl_compile_line_trio_queue_advise(20));
    ret = vltl_compile_line_trio_queue_init(&constant_lines, constant_lines, 20);
    if(ret) {
        goto vltl_compile_file_error;
    }
    Vltl_compile_line_trio_queue *global_lines = varena_alloc(&vltl_global_allocator, vltl_compile_line_trio_queue_advise(20));
    ret = vltl_compile_line_trio_queue_init(&global_lines, global_lines, 20);
    if(ret) {
        goto vltl_compile_file_error;
    }
    Vltl_compile_line_trio_queue *function_lines = varena_alloc(&vltl_global_allocator, vltl_compile_line_trio_queue_advise(20));
    ret = vltl_compile_line_trio_queue_init(&function_lines, function_lines, 20);
    if(ret) {
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
    big_buf = varena_alloc(&vltl_global_allocator, big_buf_cap);
    if(big_buf == NULL) {
        ret = ENOMEM;
        goto vltl_compile_file_error;
    }

    fputs(".intel_syntax\n", assembly_file);
    fputs("\n", assembly_file);
    fputs(".text\n", assembly_file);

    bool done = false;
    size_t line_number = 1;
    char *start_of_line = &(big_buf[0]);
    if(fgets(big_buf, big_buf_cap, src_file) == NULL) {
        done = true;
    }

    Vltl_compile_line_trio line_trio = { 0 };
    const size_t current_line_cap = 999;
    char *current_line = varena_alloc(&vltl_global_allocator, current_line_cap);

    while(!done) {
        // determine type of first token
        Vltl_lexer_token first_token = { 0 };
        size_t start_of_next_token = 0, end_of_next_token = 0;
        Vltl_lang_token_kind presumed_token_kind = { 0 };

        ret = vltl_lexer_token_chomp(
                  &start_of_next_token, &end_of_next_token, &presumed_token_kind, start_of_line
              );
        if(ret == 0) {
            ret = vltl_lexer_token_tokenize(
                      &first_token, &(start_of_line[start_of_next_token]),
                      end_of_next_token - start_of_next_token, presumed_token_kind
                  );
            if(ret) {
                goto vltl_compile_file_error;
            }
        } else if(ret == ENODATA) {
            // pass
        } else if(ret) {
            goto vltl_compile_file_error;
        }

        line_trio = (Vltl_compile_line_trio) {
            .filename = vltl_global_context.filename,
            .line_number = line_number++,
            .offset = ftell(src_file) - strlen(start_of_line)
        };

        if(first_token.token.kind == VLTL_LANG_TOKEN_KIND_OPERATION) {
            switch(first_token.token.operation->operation_kind) {
            case VLTL_LANG_OPERATION_KIND_CONSTANT:
                vltl_compile_line_trio_queue_enqueue(constant_lines, &line_trio);
                break;
            case VLTL_LANG_OPERATION_KIND_GLOBAL:
                vltl_compile_line_trio_queue_enqueue(global_lines, &line_trio);
                break;
            case VLTL_LANG_OPERATION_KIND_FUNCTION:
                vltl_compile_line_trio_queue_enqueue(function_lines, &line_trio);
                break;
            //case "import":
            //case "struct":
            // not yet supported
            case VLTL_LANG_OPERATION_KIND_UNSET:
                // this is a blank line
                break;
            default:
                // this is a line that is part of a function
                break;
            }
        }

        if(fgets(big_buf, big_buf_cap, src_file) == NULL) {
            done = true;
        }
    }

    {
        ret = vltl_compile_line_trio_queue_dequeue(import_lines, &line_trio);
        while(ret == 0) {
            fseek(src_file, line_trio.offset, SEEK_SET);
            fgets(current_line, current_line_cap, src_file);
            ret = vltl_compile_line(assembly_file, current_line, line_trio.line_number);
            if(ret) {
                goto vltl_compile_file_error;
            }

            ret = vltl_compile_line_trio_queue_dequeue(import_lines, &line_trio);
        }

        if(ret == ENODATA) {
            // done
        } else {
            goto vltl_compile_file_error;
        }
    }

    {
        ret = vltl_compile_line_trio_queue_dequeue(struct_lines, &line_trio);
        while(ret == 0) {
            fseek(src_file, line_trio.offset, SEEK_SET);
            fgets(current_line, current_line_cap, src_file);
            ret = vltl_compile_line(assembly_file, current_line, line_trio.line_number);
            if(ret) {
                goto vltl_compile_file_error;
            }

            ret = vltl_compile_line_trio_queue_dequeue(struct_lines, &line_trio);
        }

        if(ret == ENODATA) {
            // done
        } else {
            goto vltl_compile_file_error;
        }
    }

    {
        ret = vltl_compile_line_trio_queue_dequeue(constant_lines, &line_trio);
        while(ret == 0) {
            fseek(src_file, line_trio.offset, SEEK_SET);
            fgets(current_line, current_line_cap, src_file);
            ret = vltl_compile_line(assembly_file, current_line, line_trio.line_number);
            if(ret) {
                goto vltl_compile_file_error;
            }

            ret = vltl_compile_line_trio_queue_dequeue(constant_lines, &line_trio);
        }

        if(ret == ENODATA) {
            // done
        } else {
            goto vltl_compile_file_error;
        }
    }

    {
        ret = vltl_compile_line_trio_queue_dequeue(global_lines, &line_trio);
        while(ret == 0) {
            fseek(src_file, line_trio.offset, SEEK_SET);
            fgets(current_line, current_line_cap, src_file);
            ret = vltl_compile_line(assembly_file, current_line, line_trio.line_number);
            if(ret) {
                goto vltl_compile_file_error;
            }

            ret = vltl_compile_line_trio_queue_dequeue(global_lines, &line_trio);
        }

        if(ret == ENODATA) {
            // done
        } else {
            goto vltl_compile_file_error;
        }
    }

    {
        ret = vltl_compile_line_trio_queue_dequeue(function_lines, &line_trio);
        while(ret == 0) {
            size_t line_number_offset = 0;

            fseek(src_file, line_trio.offset, SEEK_SET);
            fgets(current_line, current_line_cap, src_file);
vltl_compile_file_debug:
            ret = vltl_compile_line(assembly_file, current_line, line_trio.line_number + line_number_offset++);
            if(ret) {
                goto vltl_compile_file_error;
            }

            while(vltl_global_context.function != NULL) {
                fgets(current_line, current_line_cap, src_file);
                ret = vltl_compile_line(assembly_file, current_line, line_trio.line_number + line_number_offset++);
                if(ret) {
                    goto vltl_compile_file_error;
                }
            }

            ret = vltl_compile_line_trio_queue_dequeue(function_lines, &line_trio);
        }

        if(ret == ENODATA) {
            // done
        } else {
            goto vltl_compile_file_error;
        }
    }

    fputs("\n", assembly_file);

    // Write all functions
    {
        Nkht_iterator iterator = { 0 };
        char *iterated_function_key = NULL;
        Vltl_lang_function *iterated_function_val = NULL;
        ret = nkht_iterate_start(vltl_global_table_functions, &iterator);
        if(ret) {
            goto vltl_compile_file_error;
        }

        while(
            nkht_iterate_next(
                vltl_global_table_functions, &iterator, (void *) &iterated_function_key, &iterated_function_val
            ) != ENODATA
        ) {
            if(iterated_function_key == NULL || iterated_function_val == NULL) {
                ret = ENOTRECOVERABLE;
                goto vltl_compile_file_error;
            }

            fprintf(assembly_file, ".global %s\n", iterated_function_key);
            fprintf(assembly_file, ".type %s, function\n", iterated_function_key);
        }
    }

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

            fprintf(assembly_file, ".global %s\n", iterated_global_key);
            fprintf(assembly_file, ".type %s, gnu_unique_object\n", iterated_global_key);
            fprintf(
                assembly_file,
                "%s: .quad %ld\n",
                iterated_global_key,
                (int64_t) iterated_global_val->literal->fields[0]
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
