#pragma once

#include <stdbool.h>
#include <ast.h>
#include <sast.h>

#ifdef __cplusplus
extern "C" {
#endif

// Reshape the tree
// Modifies nodes
// Handles psuedo-operations
int vltl_reshape_ast_tree(Vltl_ast_tree *tree, bool *done_now);

#ifdef __cplusplus
}
#endif
