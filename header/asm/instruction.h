#pragma once

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vltl_asm_instruction_amd64 {
    VLTL_ASM_INSTRUCTION_AMD64_UNSET,
    VLTL_ASM_INSTRUCTION_AMD64_INVALID,

    VLTL_ASM_INSTRUCTION_AMD64_CALL,
    VLTL_ASM_INSTRUCTION_AMD64_INC,
    VLTL_ASM_INSTRUCTION_AMD64_DEC,
    VLTL_ASM_INSTRUCTION_AMD64_NOP,
    VLTL_ASM_INSTRUCTION_AMD64_MOV,
    VLTL_ASM_INSTRUCTION_AMD64_LEA,
    VLTL_ASM_INSTRUCTION_AMD64_ADD,
    VLTL_ASM_INSTRUCTION_AMD64_SUB,
    VLTL_ASM_INSTRUCTION_AMD64_IMUL,
    VLTL_ASM_INSTRUCTION_AMD64_IDIV,
    VLTL_ASM_INSTRUCTION_AMD64_RET
} Vltl_asm_instruction_amd64;

typedef enum vltl_asm_instruction_kind {
    VLTL_ASM_INSTRUCTION_KIND_UNSET,
    VLTL_ASM_INSTRUCTION_KIND_INVALID,

    VLTL_ASM_INSTRUCTION_KIND_AMD64
} Vltl_asm_instruction_kind;

typedef struct vltl_asm_instruction {
    Vltl_asm_instruction_kind instruction_kind;

    union {
        Vltl_asm_instruction_amd64 as_amd64;
    };
} Vltl_asm_instruction;

bool vltl_asm_instruction_amd64_valid(const Vltl_asm_instruction_amd64 instruction);
const char *vltl_asm_instruction_amd64_detokenize(const Vltl_asm_instruction_amd64 instruction);
int vltl_asm_instruction_amd64_stringify(FILE *dest, const Vltl_asm_instruction_amd64 instruction);

bool vltl_asm_instruction_valid(const Vltl_asm_instruction instruction);
const char *vltl_asm_instruction_detokenize(const Vltl_asm_instruction instruction);
int vltl_asm_instruction_stringify(FILE *dest, const Vltl_asm_instruction instruction);

#ifdef __cplusplus
}
#endif
