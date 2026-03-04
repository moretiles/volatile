#pragma once

#include <ds/vstack.h>
#include <ds/btrc.h>

#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IESTACK_CAP_ERRORS (100)
#define IESTACK_ERROR_STRLEN (250)

typedef struct iestack_error {
    int error_code;
    char msg[IESTACK_ERROR_STRLEN];

    const char *filename;
    size_t linenumber;
} Iestack_error;

typedef struct iestack {
    Vstack *error_stack;
} Iestack;

int iestack_error_init(
    Iestack_error *error, int error_code, const char *msg, const char *filename, size_t linenumber
);
void iestack_error_deinit(Iestack_error *error);

int iestack_init(Iestack *stack);
void iestack_deinit(Iestack *stack);
int iestack_push(Iestack *stack, int error_code, const char *msg, const char *filename, size_t linenumber);
int iestack_dump(Iestack *stack, FILE *dest);
int iestack_push_direct(Iestack *stack, Iestack_error *src_error);
int iestack_pop(Iestack *stack, Iestack_error *dest_error);
#define IESTACK_PUSH(stack, error_code, string) assert(!iestack_push(stack, error_code, string, __FILE__, __LINE__))
#define IESTACK_PUSHF(stack, error_code, fstring, ...) \
{ \
    char buf_tmp___[IESTACK_ERROR_STRLEN]; \
    size_t printed_len___ = 0; \
    int ret___ = 0; \
    buf_tmp___[0] = 0; \
    BTRC_SNPRINTF(&ret___, &printed_len___, buf_tmp___, IESTACK_ERROR_STRLEN, fstring, __VA_ARGS__); \
    IESTACK_PUSH(stack, error_code, buf_tmp___); \
}
#define IESTACK_RETURN(stack, error_code, string) IESTACK_PUSH(stack, error_code, string); \
return error_code
#define IESTACK_RETURNF(stack, error_code, fstring, ...) IESTACK_PUSHF(stack, error_code, fstring, __VA_ARGS__); \
return error_code
#define IESTACK_EXPECT(stack, error_expr, string) { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_RETURN(stack, ___error_code, string); \
    } \
}
#define IESTACK_EXPECTF(stack, error_expr, fstring, ...) { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_RETURNF(stack, ___error_code, fstring, __VA_ARGS__); \
    } \
}
#define IESTACK_SUPPOSE(stack, bool_expr, error_code, string) { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_RETURN(stack, error_code, string); \
    } \
}
#define IESTACK_SUPPOSEF(stack, bool_expr, error_code, fstring, ...) { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_RETURNF(stack, error_code, fstring, __VA_ARGS__); \
    } \
}

// replace with your global error stack
#define VLTL_PUSH(error_expr, string) IESTACK_PUSH(&vltl_global_errors, error_expr, string)
#define VLTL_PUSHF(error_expr, fstring, ...) IESTACK_PUSHF(&vltl_global_errors, error_expr, fstring, __VA_ARGS__)
#define VLTL_RETURN(error_expr, string) IESTACK_RETURN(&vltl_global_errors, error_expr, string)
#define VLTL_RETURNF(error_expr, fstring, ...) IESTACK_RETURNF(&vltl_global_errors, error_expr, fstring, __VA_ARGS__)
#define VLTL_EXPECT(error_expr, string) IESTACK_EXPECT(&vltl_global_errors, error_expr, string)
#define VLTL_EXPECTF(error_expr, fstring, ...) IESTACK_EXPECTF(&vltl_global_errors, error_expr, fstring, __VA_ARGS__)
#define VLTL_SUPPOSE(bool_expr, error_code, string) IESTACK_SUPPOSE(&vltl_global_errors, bool_expr, error_code, string)
#define VLTL_SUPPOSEF(bool_expr, error_code, fstring, ...) IESTACK_SUPPOSEF(&vltl_global_errors, bool_expr, error_code, fstring, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
