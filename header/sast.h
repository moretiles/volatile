#pragma once

#include <ds/vstack.h>
#include <asm/operand.h>
#include <ast.h>
#include <global.h>

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_SAST_OPERATION_ARGUMENTS_MAX (9)

typedef enum vltl_sast_operation_kind {
    VLTL_SAST_OPERATION_KIND_UNSET,
    VLTL_SAST_OPERATION_KIND_INVALID,

    VLTL_SAST_OPERATION_KIND_EVAL,
    VLTL_SAST_OPERATION_KIND_LOAD,
    VLTL_SAST_OPERATION_KIND_STORE,
    VLTL_SAST_OPERATION_KIND_ADD,
    VLTL_SAST_OPERATION_KIND_SUB
} Vltl_sast_operation_kind;

typedef struct vltl_sast_operation {
    Vltl_sast_operation_kind kind;

    struct vltl_sast_tree *belongs_to;
    struct vltl_sast_operation *parent;
    union {
        struct vltl_sast_operation *arguments[VLTL_SAST_OPERATION_ARGUMENTS_MAX];
        struct {
            struct vltl_sast_operation *lchild;
            struct vltl_sast_operation *rchild;
        };
    };
    // pointer to the node on the ast tree representing this operation more abstractly
    struct vltl_ast_operation *equivalent;

    // evaulates_to will be a type that has integral typing of VLTL_LANG_TYPE_INTEGRAL_TBD when unknown.
    // otherwise, it should be possible to replace this operation with the literal representing its known value!
    Vltl_asm_operand evaluates_to;
    Vltl_asm_operand destination;
} Vltl_sast_operation;

typedef struct vltl_sast_tree {
    Vltl_sast_operation *root;
    Vltl_sast_operation *last;
} Vltl_sast_tree;

// Stringify the operation in src and write it to dest failing if dest_cap is too small to write everything.
// The number of bytes written, including the null byte, will be placed in *(dest_len)
int vltl_sast_operation_kind_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_sast_operation_kind src
);

// Stringify the operation_kind in src and write it to dest failing if dest_cap is too small to write everything.
// The number of bytes written, including the null byte, will be placed in *(dest_len)
int vltl_sast_operation_detokenize(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_sast_operation src);

// check whether an operation kind is valid
bool vltl_sast_operation_kind_valid(const Vltl_sast_operation_kind operation_kind);

// Check whether a Vltl_sast_operation is valid
bool vltl_sast_operation_valid(const Vltl_sast_operation operation);

// Check whether a Vltl_sast_operation is incomplete
bool vltl_sast_operation_incomplete(const Vltl_sast_operation operation);

// Discover how many arguments an operation_kind expects
size_t vltl_sast_operation_kind_argc(const Vltl_sast_operation_kind operation_kind);

// Discover how many arguments an operation currently holds
size_t vltl_sast_operation_argc(const Vltl_sast_operation operation);

// Initialize sast_operation by zeroing out and then setting kind, evaluates_to, and destination
int vltl_sast_operation_init(
    Vltl_sast_operation *operation,
    Vltl_sast_operation_kind kind,
    Vltl_asm_operand evaluates_to,
    Vltl_asm_operand destination
);

// Insert new_child into tree below parent at parent->arguments[new_child_index].
// Address of memory allocated to store new_child persistently will be placed in *(created_child_ptr).
int vltl_sast_operation_insert(
    Vltl_sast_tree *tree,
    Vltl_sast_operation *parent,
    Vltl_sast_operation *new_child,
    size_t new_child_index
);

int vltl_sast_operation_insert_operand(
    Vltl_sast_tree *tree,
    Vltl_sast_operation **created_child_ptr,
    Vltl_sast_operation *parent,
    const Vltl_asm_operand operand,
    size_t new_child_index
);

// Does three things:
//   (1) Allocate memory for new_parent and place address at *(created_parent_ptr).
//   (2) Copy existing node adopt_this to new_parent->arguments[0].
//   (3) Take over position occupied by adopt_this in the tree.
int vltl_sast_operation_adopt(
    Vltl_sast_tree *tree,
    Vltl_sast_operation *new_parent,
    Vltl_sast_operation *adopt_this
);

// Recursive helper function
int vltl_sast_tree_connect_recurse(Vltl_sast_tree *tree, Vltl_sast_operation *operation);

// Connect an incomplete subtree to form a valid subtree.
// Converting a Vltl_ast_operation to a subtree of many Vltl_sast_operation will produce incomplete subtrees.
// This function handles boring, generic details so that only the operands at the beginning and end must be set.
int vltl_sast_tree_connect(Vltl_sast_tree *tree);

// Stringify the tree in src and write it to dest failing if dest_cap is too small to write everything.
// The number of bytes written, including the null byte, will be placed in *(dest_len)
int vltl_sast_tree_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_sast_tree src
);

// Convert an ast_operation to a sast_operation (creating child nodes as needed).
// Equivalent is treated as the root sast_operation produced from src.
// Important to note that memory will be allocated for child nodes, however, equivalent is used to store the root.
// Equivalent is incomplete so operations at or below it requiring args are pushed to insert_below_next.
int vltl_sast_operation_convert(
    Vltl_sast_operation **equivalent, Vstack *insert_below_next, Vltl_ast_operation *src
);

// Convert the tree of many Vltl_ast_operation from src to a tree of many Vltl_sast_operation in dest.
// Involves:
//   (1) Forward breadth-first-search of src inserting subtree composed of many sast_operation below parent.
//   (2) Backwards breadth-first-search of src connecting subtrees to complete them (also try to optimize).
int vltl_sast_tree_convert(Vltl_sast_tree *dest, Vltl_ast_tree *src);

// tbd
extern struct vltl_sast_operation vltl_sast_operation_tbd;

// amd64 structs
extern struct vltl_sast_operation vltl_sast_operation_amd64_rax;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rbx;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rcx;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rdx;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rdi;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rsi;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rbp;
extern struct vltl_sast_operation vltl_sast_operation_amd64_rsp;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r8;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r9;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r10;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r11;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r12;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r13;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r14;
extern struct vltl_sast_operation vltl_sast_operation_amd64_r15;

#ifdef __cplusplus
}
#endif
