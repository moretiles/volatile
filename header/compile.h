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

typedef struct vltl_compile_line_trio {
    const char *filename;
    size_t line_number;
    size_t offset;
} Vltl_compile_line_trio;

typedef struct vltl_compile_line_trio_queue {
    Vqueue *trio_queue;
} Vltl_compile_line_trio_queue;

size_t vltl_compile_line_trio_queue_advise(size_t num_elems);
int vltl_compile_line_trio_queue_init(Vltl_compile_line_trio_queue **dest, void *memory, size_t num_elems);
int vltl_compile_line_trio_queue_enqueue(Vltl_compile_line_trio_queue *queue, Vltl_compile_line_trio *src);
int vltl_compile_line_trio_queue_dequeue(Vltl_compile_line_trio_queue *queue, Vltl_compile_line_trio *dest);
void vltl_compile_line_trio_queue_deinit(Vltl_compile_line_trio_queue *queue);

// Express a sast_operation as an operand (only useful when writing a compiled version of the program)
int vltl_compile_operation_operandify(Vltl_asm_operand *dest, const Vltl_sast_operation operation);

// Convert a single sast_operation to a series of assembly instructions and write them to dest.
int vltl_compile_operation_convert(FILE *dest, Vltl_sast_operation *src);

// recursive helper function for use by vltl_compile_convert
int vltl_compile_convert_recurse(FILE *dest, Vltl_sast_tree *src, Vltl_sast_operation *operation);

// Convert the src tree to a series of assembly instructions and write them to dest.
// Involves making post-order traversal of src writing compiled instructions to dest as operations appear.
int vltl_compile_convert(FILE *dest, Vltl_sast_tree *src);

// Compile single line of source code writing assembly to file
int vltl_compile_line(FILE *dest, const char *src_line, size_t line_number);

// Compile source code in file named src_filename, write assembly to file named dest_filename.
int vltl_compile_file(char *dest_filename, char *src_filename);

#ifdef __cplusplus
}
#endif
