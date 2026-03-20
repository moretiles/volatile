#include <asm/instruction.h>

#include <errno.h>

bool vltl_asm_instruction_amd64_valid(const Vltl_asm_instruction_amd64 instruction) {
    switch(instruction) {
    case VLTL_ASM_INSTRUCTION_AMD64_CALL:
    case VLTL_ASM_INSTRUCTION_AMD64_NOP:
    case VLTL_ASM_INSTRUCTION_AMD64_MOV:
    case VLTL_ASM_INSTRUCTION_AMD64_INC:
    case VLTL_ASM_INSTRUCTION_AMD64_DEC:
    case VLTL_ASM_INSTRUCTION_AMD64_ADD:
    case VLTL_ASM_INSTRUCTION_AMD64_SUB:
    case VLTL_ASM_INSTRUCTION_AMD64_IMUL:
    case VLTL_ASM_INSTRUCTION_AMD64_IDIV:
    case VLTL_ASM_INSTRUCTION_AMD64_RET:
        return true;
        break;
    default:
        return false;
        break;
    }

    return false;
}

const char *vltl_asm_instruction_amd64_detokenize(const Vltl_asm_instruction_amd64 instruction) {
    switch(instruction) {
    case VLTL_ASM_INSTRUCTION_AMD64_CALL:
        return "call";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_NOP:
        return "nop";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_MOV:
        return "mov";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_LEA:
        return "lea";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_INC:
        return "inc";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_DEC:
        return "dec";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_ADD:
        return "add";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_SUB:
        return "sub";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_IMUL:
        return "imul";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_IDIV:
        return "xor %rdx, %rdx\n\tidiv";
        break;
    case VLTL_ASM_INSTRUCTION_AMD64_RET:
        return "mov %rsp, %rbp\n"
               "\tpop %r15\n"
               "\tpop %r14\n"
               "\tpop %r13\n"
               "\tpop %r12\n"
               "\tpop %rbx\n"
               "\tpop %rbp\n"
               "\tret";
        break;
    default:
        return NULL;
        break;
    }

    return NULL;
}

int vltl_asm_instruction_amd64_stringify(FILE *dest, const Vltl_asm_instruction_amd64 instruction) {
    if(dest == NULL) {
        return EINVAL;
    }

    fputs(vltl_asm_instruction_amd64_detokenize(instruction), dest);

    return 0;
}

bool vltl_asm_instruction_valid(const Vltl_asm_instruction instruction) {
    switch(instruction.instruction_kind) {
    case VLTL_ASM_INSTRUCTION_KIND_AMD64:
        return vltl_asm_instruction_amd64_valid(instruction.as_amd64);
        break;
    default:
        return false;
        break;
    }

    return false;
}

const char *vltl_asm_instruction_detokenize(const Vltl_asm_instruction instruction) {
    switch(instruction.instruction_kind) {
    case VLTL_ASM_INSTRUCTION_KIND_AMD64:
        return vltl_asm_instruction_amd64_detokenize(instruction.as_amd64);
        break;
    default:
        return NULL;
        break;
    }

    return NULL;
}

int vltl_asm_instruction_stringify(FILE *dest, const Vltl_asm_instruction instruction) {
    switch(instruction.instruction_kind) {
    case VLTL_ASM_INSTRUCTION_KIND_AMD64:
        return vltl_asm_instruction_amd64_stringify(dest, instruction.as_amd64);
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}
