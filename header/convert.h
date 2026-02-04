#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <global.h>
#include <asm/operand.h>

// Copy to dest the asm operand corresponding to the global register in src.
int vltl_convert_global_register_to_asm_operand(Vltl_asm_operand *dest, const Vltl_global_register *src);

// Copy to dest the memory address of the global register corresponding to the asm operand in src.
int vltl_convert_asm_operand_to_global_register(Vltl_global_register **dest_ptr, const Vltl_asm_operand src);

// amd64
int vltl_convert_amd64_global_register_to_asm_operand(Vltl_asm_operand *dest, const Vltl_global_register *src);
int vltl_convert_amd64_asm_operand_to_global_register(Vltl_global_register **dest_ptr, const Vltl_asm_operand src);

#ifdef __cplusplus
}
#endif
