#pragma once

#include <ds/vht.h>
#include <ds/nkht.h>
#include <lang/type.h>
#include <lang/local.h>

#ifdef __cplusplus
extern "C" {
#endif

// status is incomplete before the function signature has been scanned
// status is signature after the function signature has been scanned
// status is complete after the function body has been scanned
typedef enum vltl_lang_function_status {
    VLTL_LANG_FUNCTION_STATUS_UNSET,
    VLTL_LANG_FUNCTION_STATUS_INVALID,

    VLTL_LANG_FUNCTION_STATUS_INCOMPLETE,
    VLTL_LANG_FUNCTION_STATUS_SIGNATURE,
    VLTL_LANG_FUNCTION_STATUS_COMPLETE
} Vltl_lang_function_status;

typedef struct vltl_lang_function {
    // used to indicate how complete an understanding of the function has been achieved
    Vltl_lang_function_status parse_status;

    // used during first pass that covers only function signature
    const char *name;
    const Vltl_lang_type *return_type;
    const Vltl_lang_type_attribute *return_type_attributes[9];
    size_t expected_argc;
    const Vltl_lang_type *parameters[9];
    const Vltl_lang_type_attribute *parameter_attributes[9][9];

    // used during second pass that considers function body
    Nkht *locals;
    size_t stack_frame_size;
    size_t stack_frame_cap;
    Vht *registers_used;
} Vltl_lang_function;

int vltl_lang_function_init(Vltl_lang_function *dest, const char *name);
int vltl_lang_function_local_get(Vltl_lang_local **dest, Vltl_lang_function *function, const char *name);
int vltl_lang_function_local_set(
    Vltl_lang_function *function, const char *name, const Vltl_lang_type *type,
    Vltl_lang_type_attribute *attributes[9], Vltl_lang_literal *literal
);

#ifdef __cplusplus
}
#endif
