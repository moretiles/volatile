#pragma once

#include <ds/vht.h>
#include <ds/nkht.h>
#include <lang/type.h>
#include <lang/local.h>

#ifdef __cplusplus
extern "C" {
#endif

// How much indentation (in terms of distinct bodies) are allowed
#define VLTL_LANG_BODY_CAP (256)

// Value incremented for each body for use in labels
#define VLTL_LANG_BODY_LABEL_ITERATE (1000)

// Offset used to prepare labels when bodies open up
#define VLTL_LANG_BODY_LABEL_BARE_OPEN (1)
#define VLTL_LANG_BODY_LABEL_IF_OPEN (2)
#define VLTL_LANG_BODY_LABEL_ELIF_OPEN (3)
#define VLTL_LANG_BODY_LABEL_ELSE_OPEN (4)
#define VLTL_LANG_BODY_LABEL_WHILE_OPEN (5)
#define VLTL_LANG_BODY_LABEL_FUNCTION_OPEN (6)

// Offset used to prepare labels when bodies close up
#define VLTL_LANG_BODY_LABEL_BARE_CLOSE (11)
#define VLTL_LANG_BODY_LABEL_IF_CLOSE (12)
#define VLTL_LANG_BODY_LABEL_ELIF_CLOSE (13)
#define VLTL_LANG_BODY_LABEL_ELSE_CLOSE (14)
#define VLTL_LANG_BODY_LABEL_WHILE_CLOSE (15)
#define VLTL_LANG_BODY_LABEL_FUNCTION_CLOSE (16)

// Offset used to prepare labels at end of chain of if/elif/else
#define VLTL_LANG_BODY_LABEL_IFELIFELSE_CLOSE (21)

// status is incomplete before the function signature has been scanned
// status is signature after the function signature has been scanned
// status is complete after the function body has been scanned
typedef enum vltl_lang_body_kind {
    VLTL_LANG_BODY_KIND_UNSET,
    VLTL_LANG_BODY_KIND_INVALID,

    VLTL_LANG_BODY_KIND_BARE,
    VLTL_LANG_BODY_KIND_IF,
    VLTL_LANG_BODY_KIND_ELIF,
    VLTL_LANG_BODY_KIND_ELSE,
    VLTL_LANG_BODY_KIND_WHILE,
    VLTL_LANG_BODY_KIND_FUNCTION
} Vltl_lang_body_kind;

typedef struct vltl_lang_body {
    Vltl_lang_body_kind body_kind;
    Nkht local_variables;
} Vltl_lang_body;

#ifdef __cplusplus
}
#endif
