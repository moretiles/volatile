#pragma once

#include <isa.h>
#include <lang/type.h>
#include <lang/function.h>
#include <asm/register.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_asm_operand_kind {
    VLTL_ASM_OPERAND_KIND_UNSET,
    VLTL_ASM_OPERAND_KIND_INVALID,

    VLTL_ASM_OPERAND_KIND_TBD,
    VLTL_ASM_OPERAND_KIND_UNKNOWN,
    VLTL_ASM_OPERAND_KIND_FUNCTION,
    VLTL_ASM_OPERAND_KIND_MEMORY,
    VLTL_ASM_OPERAND_KIND_REGISTER,
    VLTL_ASM_OPERAND_KIND_IMMEDIATE
} Vltl_asm_operand_kind;

typedef enum vltl_asm_operand_memory_kind {
    VLTL_ASM_OPERAND_MEMORY_KIND_UNSET,
    VLTL_ASM_OPERAND_MEMORY_KIND_INVALID,

    VLTL_ASM_OPERAND_MEMORY_KIND_LOCAL,
    VLTL_ASM_OPERAND_MEMORY_KIND_GLOBAL
} Vltl_asm_operand_memory_kind;

typedef struct vltl_asm_operand_memory {
    Vltl_asm_operand_memory_kind memory_kind;
    const char *name;
    Vltl_lang_type_integral integral_type;
    uint64_t value;
} Vltl_asm_operand_memory;

typedef struct vltl_asm_operand_register {
    enum vltl_isa isa;
    union {
        Vltl_asm_register_8086 value_8086;
        Vltl_asm_register_intel32 value_intel32;
        Vltl_asm_register_amd64 value_amd64;
    };
} Vltl_asm_operand_register;

typedef enum vltl_asm_operand_immediate_representation {
    VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_UNSET,
    VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_INVALID,

    VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE2,
    VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
    VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE16
} Vltl_asm_operand_immediate_representation;

typedef struct vltl_asm_operand_immediate {
    Vltl_lang_type_integral integral_type;

    // an immediate value must be a number represented in a number of bytes less-than-or-equal-to the register size
    Vltl_asm_operand_immediate_representation representation;
    uint64_t value;
} Vltl_asm_operand_immediate;

typedef struct vltl_asm_operand {
    enum vltl_asm_operand_kind kind;
    union {
        const char *as_unknown;
        Vltl_lang_function *as_function;
        Vltl_asm_operand_memory as_memory;
        Vltl_asm_operand_register as_register;
        Vltl_asm_operand_immediate as_immediate;
    };
} Vltl_asm_operand;

// public facing
bool vltl_asm_operand_valid(const Vltl_asm_operand operand);
int vltl_asm_operand_detokenize(char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand operand);
int vltl_asm_operand_stringify(FILE *dest, const Vltl_asm_operand operand);

// internal
bool vltl_asm_operand_memory_kind_valid(const Vltl_asm_operand_memory_kind memory_kind);
bool vltl_asm_operand_memory_valid(const Vltl_asm_operand_memory as_memory);
bool vltl_asm_operand_register_valid(const Vltl_asm_operand_register as_register);
bool vltl_asm_operand_immediate_valid(const Vltl_asm_operand_immediate as_immediate);

int vltl_asm_operand_function_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_lang_function operand
);
int vltl_asm_operand_memory_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_memory operand
);
int vltl_asm_operand_register_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_register operand
);
int vltl_asm_operand_immediate_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_immediate operand
);

// amd64
bool vltl_asm_operand_memory_amd64_valid(const Vltl_asm_operand_memory memory);
int vltl_asm_operand_memory_amd64_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_memory operand
);

bool vltl_asm_operand_register_amd64_valid(const Vltl_asm_operand_register operand);
int vltl_asm_operand_register_amd64_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_register operand
);

bool vltl_asm_operand_immediate_amd64_valid(const Vltl_asm_operand_immediate operand);
int vltl_asm_operand_immediate_amd64_detokenize(
    char *buf, size_t buf_cap, size_t *buf_len, const Vltl_asm_operand_immediate operand
);

// tbd
#define vltl_asm_operand_tbd { \
    .kind = VLTL_ASM_OPERAND_KIND_TBD, \
}

// amd64 structs
#define vltl_asm_operand_amd64_rax { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RAX \
    } \
}

#define vltl_asm_operand_amd64_rbx { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RBX \
    } \
}

#define vltl_asm_operand_amd64_rcx { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RCX \
    } \
}

#define vltl_asm_operand_amd64_rdx { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RDX \
    } \
}

#define vltl_asm_operand_amd64_rdi { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RDI \
    } \
}

#define vltl_asm_operand_amd64_rsi { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RSI \
    } \
}

#define vltl_asm_operand_amd64_rbp { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RBP \
    } \
}

#define vltl_asm_operand_amd64_rsp { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_RSP \
    } \
}

#define vltl_asm_operand_amd64_r8 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R8 \
    } \
}

#define vltl_asm_operand_amd64_r9 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R9 \
    } \
}

#define vltl_asm_operand_amd64_r10 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R10 \
    } \
}

#define vltl_asm_operand_amd64_r11 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R11 \
    } \
}

#define vltl_asm_operand_amd64_r12 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R12 \
    } \
}

#define vltl_asm_operand_amd64_r13 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R13 \
    } \
}

#define vltl_asm_operand_amd64_r14 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R14 \
    } \
}

#define vltl_asm_operand_amd64_r15 { \
    .kind = VLTL_ASM_OPERAND_KIND_REGISTER, \
    .as_register = { \
        .isa = VLTL_ISA_AMD64, \
        .value_amd64 = VLTL_ASM_REGISTER_AMD64_R15 \
    } \
}

#ifdef __cplusplus
}
#endif
