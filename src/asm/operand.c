#include <global.h>
#include <isa.h>
#include <lang/type.h>
#include <asm/register.h>
#include <asm/operand.h>
#include <lang/local.h>
#include <ds/btrc.h>

#include <errno.h>
#include <string.h>

bool vltl_asm_operand_valid(const Vltl_asm_operand operand) {
    switch(operand.kind) {
    case VLTL_ASM_OPERAND_KIND_UNKNOWN:
        return operand.as_unknown != NULL;
    case VLTL_ASM_OPERAND_KIND_TBD:
        return true;
        break;
    case VLTL_ASM_OPERAND_KIND_FUNCTION:
        // TODO: fully implement functions
        return true;
        break;
    case VLTL_ASM_OPERAND_KIND_MEMORY:
        return vltl_asm_operand_memory_valid(operand.as_memory);
        break;
    case VLTL_ASM_OPERAND_KIND_REGISTER:
        return vltl_asm_operand_register_valid(operand.as_register);
        break;
    case VLTL_ASM_OPERAND_KIND_IMMEDIATE:
        return vltl_asm_operand_immediate_valid(operand.as_immediate);
        break;
    default:
        return false;
        break;
    }

    return false;
}

bool vltl_asm_operand_memory_valid(const Vltl_asm_operand_memory as_memory) {
    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_asm_operand_memory_amd64_valid(as_memory);
        break;
    default:
        return false;
        break;
    }

    return false;
}

bool vltl_asm_operand_register_valid(const Vltl_asm_operand_register as_register) {
    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_asm_operand_register_amd64_valid(as_register);
        break;
    default:
        return false;
        break;
    }

    return false;
}

bool vltl_asm_operand_immediate_valid(const Vltl_asm_operand_immediate as_immediate) {
    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_asm_operand_immediate_amd64_valid(as_immediate);
        break;
    default:
        return false;
        break;
    }

    return false;
}

int vltl_asm_operand_detokenize(char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand operand) {
    const char *str;
    int ret = 0;

    switch(operand.kind) {
    case VLTL_ASM_OPERAND_KIND_UNSET:
        str = "???";
        return btrc_strncpy(buf_len, buf, str, strlen(str));
        break;
    case VLTL_ASM_OPERAND_KIND_UNKNOWN:
        if (operand.as_unknown == NULL) {
            return EINVAL;
        }

        BTRC_SNPRINTF(&ret, buf_len, buf, buf_cap, "%s", operand.as_unknown);
        return ret;
        break;
    case VLTL_ASM_OPERAND_KIND_TBD:
        str = "TBD";
        return btrc_strncpy(buf_len, buf, str, strlen(str));
        break;
    case VLTL_ASM_OPERAND_KIND_FUNCTION:
        return vltl_asm_operand_function_detokenize(buf, buf_cap, buf_len, *(operand.as_function));
        break;
    case VLTL_ASM_OPERAND_KIND_MEMORY:
        return vltl_asm_operand_memory_detokenize(buf, buf_cap, buf_len, operand.as_memory);
        break;
    case VLTL_ASM_OPERAND_KIND_REGISTER:
        return vltl_asm_operand_register_detokenize(buf, buf_cap, buf_len, operand.as_register);
        break;
    case VLTL_ASM_OPERAND_KIND_IMMEDIATE:
        return vltl_asm_operand_immediate_detokenize(buf, buf_cap, buf_len, operand.as_immediate);
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}

int vltl_asm_operand_function_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_lang_function operand
) {
    int ret = 0;
    int buf_len_helper = 0;
    if(buf == NULL || buf_cap == 0 || buf_len == NULL) {
        return EINVAL;
    }

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        BTRC_SNPRINTF(&ret, &buf_len_helper, buf, buf_cap, "%s@PLT", operand.name);
        return ret;
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}


int vltl_asm_operand_memory_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_memory operand
) {
    if(buf == NULL || buf_cap == 0 || buf_len == NULL) {
        return EINVAL;
    }

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_asm_operand_memory_amd64_detokenize(buf, buf_cap, buf_len, operand);
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}

int vltl_asm_operand_stringify(
    FILE *dest, const Vltl_asm_operand operand
) {
#define VLTL_ASM_OPERAND_STRINGIFY_BUF_CAP (99)
    size_t buf_len = 0;
    char buf[VLTL_ASM_OPERAND_STRINGIFY_BUF_CAP] = { 0 };
    if(dest == NULL) {
        return EINVAL;
    }

    int ret = vltl_asm_operand_detokenize(buf, VLTL_ASM_OPERAND_STRINGIFY_BUF_CAP, &buf_len, operand);
    if(ret != 0) {
        return ret;
    }

    fputs(buf, dest);
    return 0;
}

int vltl_asm_operand_register_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_register operand
) {
    if(buf == NULL || buf_cap == 0 || buf_len == NULL) {
        return EINVAL;
    }

    switch(operand.isa) {
    case VLTL_ISA_AMD64:
        return vltl_asm_operand_register_amd64_detokenize(buf, buf_cap, buf_len, operand);
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}

int vltl_asm_operand_immediate_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_immediate operand
) {
    if(buf == NULL || buf_cap == 0 || buf_len == NULL) {
        return EINVAL;
    }

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_asm_operand_immediate_amd64_detokenize(buf, buf_cap, buf_len, operand);
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}

bool vltl_asm_operand_memory_kind_valid(const Vltl_asm_operand_memory_kind memory_kind) {
    switch(memory_kind) {
    case VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL:
    case VLTL_ASM_OPERAND_MEMORY_KIND_GLOBAL:
        return true;
        break;
    default:
        return false;
        break;
    }

    return true;
}

bool vltl_asm_operand_memory_amd64_valid(const Vltl_asm_operand_memory memory) {
    if(!vltl_asm_operand_memory_kind_valid(memory.memory_kind) || memory.name == NULL) {
        return false;
    }

    return true;
}

int vltl_asm_operand_memory_amd64_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_memory memory
) {
    int ret = 0;
    int buf_len_helper = 0;
    Vltl_lang_local *local = NULL;
    if(buf == NULL || buf_cap <= 1 || buf_len == NULL) {
        return EINVAL;
    }

    switch(memory.memory_kind) {
    case VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL:
        ret = vltl_lang_function_local_get(&local, vltl_global_context.function, memory.name);
        if(ret) {
            return ret;
        }
        BTRC_SNPRINTF(&ret, &buf_len_helper, buf, buf_cap, "-%lu[%%rbp]", local->frame_offset);
        break;
    case VLTL_ASM_OPERAND_MEMORY_KIND_GLOBAL:
        if(vltl_global_config.pic_enabled) {
            BTRC_SNPRINTF(&ret, &buf_len_helper, buf, buf_cap, "%s[%%rip]", memory.name);
        } else {
            BTRC_SNPRINTF(&ret, &buf_len_helper, buf, buf_cap, "%s", memory.name);
        }
        break;
    default:
        return EINVAL;
        break;
    }

    *buf_len = (size_t) buf_len_helper;
    return ret;
}

bool vltl_asm_operand_register_amd64_valid(const Vltl_asm_operand_register operand) {
    if(operand.isa != VLTL_ISA_AMD64) {
        return false;
    }

    switch(operand.value_amd64) {
    case VLTL_ASM_REGISTER_AMD64_RAX:
    case VLTL_ASM_REGISTER_AMD64_RBX:
    case VLTL_ASM_REGISTER_AMD64_RCX:
    case VLTL_ASM_REGISTER_AMD64_RDX:
    case VLTL_ASM_REGISTER_AMD64_RSI:
    case VLTL_ASM_REGISTER_AMD64_RDI:
    case VLTL_ASM_REGISTER_AMD64_RBP:
    case VLTL_ASM_REGISTER_AMD64_RSP:
    case VLTL_ASM_REGISTER_AMD64_R8:
    case VLTL_ASM_REGISTER_AMD64_R9:
    case VLTL_ASM_REGISTER_AMD64_R10:
    case VLTL_ASM_REGISTER_AMD64_R11:
    case VLTL_ASM_REGISTER_AMD64_R12:
    case VLTL_ASM_REGISTER_AMD64_R13:
    case VLTL_ASM_REGISTER_AMD64_R14:
    case VLTL_ASM_REGISTER_AMD64_R15:
        return true;
        break;
    default:
        return false;
        break;
    }

    return false;
}

int vltl_asm_operand_register_amd64_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_register operand
) {
    int ret = 0;
    int buf_len_helper = 0;
    const char *as_string = NULL;
    if(buf == NULL || buf_cap <= 1 || buf_len == NULL || operand.isa != VLTL_ISA_AMD64) {
        return EINVAL;
    }

    if(
        operand.value_amd64 <= VLTL_ASM_REGISTER_AMD64_INVALID ||
        operand.value_amd64 >= VLTL_ASM_REGISTER_AMD64_EOF
    ) {
        return EINVAL;
    }

    as_string = vltl_asm_register_detokenize_table[operand.value_amd64];
    BTRC_SNPRINTF(&ret, &buf_len_helper, buf, buf_cap, "%%%s", as_string);
    *buf_len = (size_t) buf_len_helper;
    return ret;
}

bool vltl_asm_operand_immediate_amd64_valid(const Vltl_asm_operand_immediate operand) {
    if(!vltl_lang_type_integral_valid(operand.integral_type)) {
        return false;
    }

    switch(operand.integral_type) {
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR8:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR16:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR32:
    case VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64:
        break;
    default:
        return false;
        break;
    }

    // ignore attributes for now

    switch(operand.representation) {
    case VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE2:
    case VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10:
    case VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE16:
        break;
    default:
        return false;
        break;
    }

    return true;
}

int vltl_asm_operand_immediate_amd64_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_immediate operand
) {
    int buf_len_helper = 0;
    int ret = 0;
    if(buf == NULL || buf_cap <= 1 || buf_len == NULL) {
        return EINVAL;
    }

    switch(operand.representation) {
    case VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE2:
    case VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10:
    case VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE16:
        // output as unsigned base10 no matter the representation format when compiling
        BTRC_SNPRINTF(&ret, &buf_len_helper, buf, buf_cap, "%lu", (uint64_t) operand.value);
        *buf_len = (size_t) buf_len_helper;
        break;
    default:
        return false;
        break;
    }

    return ret;
}
