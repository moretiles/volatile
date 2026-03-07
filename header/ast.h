#pragma once

#include <lang/type.h>
#include <lang/token.h>
#include <lang/literal.h>
#include <asm/operand.h>
#include <lexer.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_AST_OPERATION_ARGUMENTS_MAX (9)

typedef enum vltl_ast_operation_precedence_order {
    VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_UNSET,
    VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_INVALID,

    VLTL_AST_OPERATION_PRECEDENCE_ORDER_LEFT_TO_RIGHT,
    VLTL_AST_OPERATION_PRECEDENCE_ORDER_RIGHT_TO_LEFT
} Vltl_ast_operation_precedence_order;

typedef enum vltl_ast_operation_precedence {
    VLTL_AST_OPERATION_PRECEDENCE_UNSET,
    VLTL_AST_OPERATION_PRECEDENCE_INVALID,

    VLTL_AST_OPERATION_PRECEDENCE_0,
    VLTL_AST_OPERATION_PRECEDENCE_1,
    VLTL_AST_OPERATION_PRECEDENCE_2,
    VLTL_AST_OPERATION_PRECEDENCE_3,
    VLTL_AST_OPERATION_PRECEDENCE_4,
    VLTL_AST_OPERATION_PRECEDENCE_5,
    VLTL_AST_OPERATION_PRECEDENCE_6,
    VLTL_AST_OPERATION_PRECEDENCE_7,
    VLTL_AST_OPERATION_PRECEDENCE_8,
    VLTL_AST_OPERATION_PRECEDENCE_9,
    VLTL_AST_OPERATION_PRECEDENCE_10,
    VLTL_AST_OPERATION_PRECEDENCE_11,
    VLTL_AST_OPERATION_PRECEDENCE_12,
    VLTL_AST_OPERATION_PRECEDENCE_13,
    VLTL_AST_OPERATION_PRECEDENCE_14,
    VLTL_AST_OPERATION_PRECEDENCE_15,
    VLTL_AST_OPERATION_PRECEDENCE_16,
    VLTL_AST_OPERATION_PRECEDENCE_17,
    VLTL_AST_OPERATION_PRECEDENCE_18,
    VLTL_AST_OPERATION_PRECEDENCE_19,
    VLTL_AST_OPERATION_PRECEDENCE_20
} Vltl_ast_operation_precedence;

// Any change made to enums in vltl_ast_operation_kind needs to update:
// - switch statement used by vltl_ast_operation_valid to check what operations are valid
typedef enum vltl_ast_operation_kind {
    VLTL_AST_OPERATION_KIND_UNSET,
    VLTL_AST_OPERATION_KIND_INVALID,

    VLTL_AST_OPERATION_KIND_EVAL,
    VLTL_AST_OPERATION_KIND_ADDRESS,
    VLTL_AST_OPERATION_KIND_INDIRECTION,
    VLTL_AST_OPERATION_KIND_GROUPING_OPEN,
    VLTL_AST_OPERATION_KIND_GROUPING_CLOSE,
    VLTL_AST_OPERATION_KIND_INDEX_OPEN,
    VLTL_AST_OPERATION_KIND_INDEX_CLOSE,
    VLTL_AST_OPERATION_KIND_CALL,
    VLTL_AST_OPERATION_KIND_MUL,
    VLTL_AST_OPERATION_KIND_DIV,
    VLTL_AST_OPERATION_KIND_ADD,
    VLTL_AST_OPERATION_KIND_SUB,
    VLTL_AST_OPERATION_KIND_TEST_EQUALS,
    VLTL_AST_OPERATION_KIND_COMMA,
    VLTL_AST_OPERATION_KIND_CSV,
    VLTL_AST_OPERATION_KIND_TYPEAS,
    VLTL_AST_OPERATION_KIND_EQUALS,
    VLTL_AST_OPERATION_KIND_RETURN,
    VLTL_AST_OPERATION_KIND_GLOBAL,
    VLTL_AST_OPERATION_KIND_CONSTANT,
    VLTL_AST_OPERATION_KIND_LOCAL,
    VLTL_AST_OPERATION_KIND_FUNCTION,
    VLTL_AST_OPERATION_KIND_IF,
    VLTL_AST_OPERATION_KIND_ELIF,
    VLTL_AST_OPERATION_KIND_ELSE,
    VLTL_AST_OPERATION_KIND_WHILE,
    VLTL_AST_OPERATION_KIND_BODY_OPEN,
    VLTL_AST_OPERATION_KIND_BODY_CLOSE
} Vltl_ast_operation_kind;

typedef struct vltl_ast_operation {
    Vltl_ast_operation_kind kind;

    struct vltl_ast_tree *belongs_to;
    struct vltl_ast_operation *parent;
    union {
        struct vltl_ast_operation *arguments[VLTL_AST_OPERATION_ARGUMENTS_MAX];
        struct {
            struct vltl_ast_operation *lchild;
            struct vltl_ast_operation *rchild;
        };
    };
    // pointer to the node on the sast tree representing this operation with less abstraction
    struct vltl_sast_operation *equivalent;

    // evaulates_to will be a type that has integral typing of VLTL_LANG_TYPE_INTEGRAL_TBD when unknown.
    // otherwise, it should be possible to replace this operation with the literal representing its known value!
    struct vltl_lang_token *evaluates_to;
    const Vltl_lang_type *result_type;
} Vltl_ast_operation;

typedef struct vltl_ast_tree {
    Vltl_ast_operation *root;
    Vltl_ast_operation *last;
} Vltl_ast_tree;

// Check whether a vltl_ast_operation_precedence_order is valid
bool vltl_ast_operation_precedence_order_valid(const Vltl_ast_operation_precedence_order precedence_order);

// Determine the precedence_order from precedence
int vltl_ast_operation_precedence_order_determine(
    Vltl_ast_operation_precedence_order *dest, const Vltl_ast_operation_precedence src
);

// Check whether a vltl_ast_operation_precedence is valid
bool vltl_ast_operation_precedence_valid(const Vltl_ast_operation_precedence operation_precedence);

// Determine the precedence level of an operation
int vltl_ast_operation_precedence_determine(Vltl_ast_operation_precedence *dest, const Vltl_ast_operation src);

// Stringify the operation in src and write it to dest failing if dest_cap is too small to write everything.
// The number of bytes written, including the null byte, will be placed in *(dest_len)
int vltl_ast_operation_kind_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_ast_operation_kind src
);

// Stringify the operation_kind in src and write it to dest failing if dest_cap is too small to write everything.
// The number of bytes written, including the null byte, will be placed in *(dest_len)
int vltl_ast_operation_detokenize(char *dest, size_t dest_cap, size_t *dest_len, const Vltl_ast_operation src);

// Check whether a vltl_ast_operation_kind is valid
bool vltl_ast_operation_kind_valid(const Vltl_ast_operation_kind operation_kind);

// Check whether a Vltl_ast_operation is valid
bool vltl_ast_operation_valid(const Vltl_ast_operation operation);

// Perform copy that overwrites dest with src and updates all first-level children of dest to have dest as parent.
int vltl_ast_operation_copy(Vltl_ast_operation *dest, const Vltl_ast_operation *src);

// Discover how many arguments an operation_kind expects
size_t vltl_ast_operation_expected_argc(const Vltl_ast_operation operation);

// Discover how many arguments have been set for a given operation
size_t vltl_ast_operation_argc(const Vltl_ast_operation operation);

// Initialize ast_operation by zeroing out and then setting kind and result_type
int vltl_ast_operation_init(
    Vltl_ast_operation *operation,
    Vltl_ast_operation_kind kind,
    Vltl_lang_token *evaluates_to,
    const Vltl_lang_type *result_type
);

// Insert new_child into tree below parent at parent->arguments[new_child_index].
int vltl_ast_operation_insert(
    Vltl_ast_tree *tree,
    Vltl_ast_operation *parent,
    Vltl_ast_operation *new_child,
    size_t new_child_index
);

// Does three things:
//   (1) Allocate memory to move and place address of newly allocated memory at *(created_child_ptr).
//   (2) Copy existing node adopt_this to newly allocated memory.
//   (3) Overwrite the memory used by the adopted node (adopt_this) with new_parent (while updating its parent)
//   (4) Set the first non-null argument of the new_parent to point to the adopted child
int vltl_ast_operation_adopt(
    Vltl_ast_tree *tree,
    Vltl_ast_operation *new_parent,
    Vltl_ast_operation *adopt_this
);

// Stringify the tree in src and write it to dest failing if dest_cap is too small to write everything.
// The number of bytes written, including the null byte, will be placed in *(dest_len)
int vltl_ast_tree_detokenize(
    char *dest,
    size_t dest_cap,
    size_t *dest_len,
    const Vltl_ast_tree src
);

// Insert operation into tree
// In consideration of the existing state of the tree and the priority of operation reordering may occur
int vltl_ast_tree_insert(Vltl_ast_tree *tree, Vltl_ast_operation *pushed);

// Build ast_tree in dest from lexer_line in src.
// Continue calling while value returned is 0.
// If the value returned is ENODATA then, well done, the line has been fully converted to the dest ast_tree!
int vltl_ast_tree_convert(Vltl_ast_tree *dest, Vltl_lexer_line *src);

#ifdef __cplusplus
}
#endif
