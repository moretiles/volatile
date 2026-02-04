#include <convert.h>

#include <errno.h>

// Copy to dest the asm operand corresponding to the global register in src
int vltl_convert_global_register_to_asm_operand(Vltl_asm_operand *dest, const Vltl_global_register *src) {
    if(dest == NULL || src == NULL) {
        return EINVAL;
    }

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_convert_amd64_global_register_to_asm_operand(dest, src);
        break;
    default:
        return EINVAL;
        break;
    }

    return 0;
}

// Copy to dest the global register corresponding to the asm operand in src
int vltl_convert_asm_operand_to_global_register(Vltl_global_register **dest_ptr, const Vltl_asm_operand src) {
    if(dest_ptr == NULL) {
        return EINVAL;
    }

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_convert_amd64_asm_operand_to_global_register(dest_ptr, src);
        break;
    default:
        return EINVAL;
        break;
    }

    return 0;
}

int vltl_convert_amd64_global_register_to_asm_operand(Vltl_asm_operand *dest, const Vltl_global_register *src) {
    if(dest == NULL) {
        return EINVAL;
    }

    switch(src->as_amd64) {
    case VLTL_GLOBAL_REGISTER_AMD64_RAX:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rax;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RBX:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rbx;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RCX:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rcx;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RDX:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rdx;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RDI:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rdi;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RSI:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rsi;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RBP:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rbp;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_RSP:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_rsp;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R8:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r8;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R9:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r9;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R10:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r10;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R11:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r11;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R12:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r12;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R13:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r13;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R14:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r14;
        break;
    case VLTL_GLOBAL_REGISTER_AMD64_R15:
        *dest = (Vltl_asm_operand) vltl_asm_operand_amd64_r15;
        break;
    default:
        return EINVAL;
        break;
    }

    return 0;
}

int vltl_convert_amd64_asm_operand_to_global_register(
    Vltl_global_register **dest_ptr, const Vltl_asm_operand src
) {
    if(dest_ptr == NULL) {
        return EINVAL;
    }

    switch(src.as_register.value_amd64) {
    case VLTL_ASM_REGISTER_AMD64_RAX:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rax;
        break;
    case VLTL_ASM_REGISTER_AMD64_RBX:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rbx;
        break;
    case VLTL_ASM_REGISTER_AMD64_RCX:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rcx;
        break;
    case VLTL_ASM_REGISTER_AMD64_RDX:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rdx;
        break;
    case VLTL_ASM_REGISTER_AMD64_RDI:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rdi;
        break;
    case VLTL_ASM_REGISTER_AMD64_RSI:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rsi;
        break;
    case VLTL_ASM_REGISTER_AMD64_RBP:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rbp;
        break;
    case VLTL_ASM_REGISTER_AMD64_RSP:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_rsp;
        break;
    case VLTL_ASM_REGISTER_AMD64_R8:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r8;
        break;
    case VLTL_ASM_REGISTER_AMD64_R9:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r9;
        break;
    case VLTL_ASM_REGISTER_AMD64_R10:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r10;
        break;
    case VLTL_ASM_REGISTER_AMD64_R11:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r11;
        break;
    case VLTL_ASM_REGISTER_AMD64_R12:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r12;
        break;
    case VLTL_ASM_REGISTER_AMD64_R13:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r13;
        break;
    case VLTL_ASM_REGISTER_AMD64_R14:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r14;
        break;
    case VLTL_ASM_REGISTER_AMD64_R15:
        *dest_ptr = (Vltl_global_register *) &vltl_global_register_amd64_r15;
        break;
    default:
        return EINVAL;
        break;
    }

    return 0;
}
