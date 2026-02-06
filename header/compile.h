#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <asm/instruction.h>
#include <asm/operand.h>
#include <asm/register.h>
#include <lang/type.h>
#include <lexer.h>
#include <ast.h>
#include <sast.h>

#include <stdio.h>

typedef struct vltl_compile_line {
    Vltl_lexer_line lexer_line;
    Vltl_ast_tree ast_tree;
    Vltl_sast_tree sast_tree;
} Vltl_compile_line;

typedef struct vltl_compile_file {
    // src_filename names the file containing VLTL source code.
    char src_filename[99];
    FILE *src_file;
    Vltl_compile_line src_lines[99];

    // tmp_filename names the file assembly instructions are written to, before the gnu assembler is invoked.
    char tmp_filename[99];
    FILE *tmp_file;

    // dest_filename names the file that is the final, produced binary.
    char dest_filename[99];
    FILE *dest_file;
} Vltl_compile_file;

// Express a sast_operation as an operand (only useful when writing a compiled version of the program)
int vltl_compile_operation_operandify(Vltl_asm_operand *dest, const Vltl_sast_operation operation);

// Convert a single sast_operation to a series of assembly instructions and write them to dest.
int vltl_compile_operation_convert(FILE *dest, Vltl_sast_operation *src);

// recursive helper function for use by vltl_compile_convert
int vltl_compile_convert_recurse(FILE *dest, Vltl_sast_tree *src, Vltl_sast_operation *operation);

// Convert the src tree to a series of assembly instructions and write them to dest.
// Involves making post-order traversal of src writing compiled instructions to dest as operations appear.
int vltl_compile_convert(FILE *dest, Vltl_sast_tree *src);

// Compile source code in file named src_filename, write assembly to file named dest_filename.
int vltl_compile_file(char *dest_filename, char *src_filename);

#ifdef __cplusplus
}
#endif
