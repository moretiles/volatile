#include <isa.h>
#include <compile.h>

#include <errno.h>

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
