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

#ifdef __cplusplus
}
#endif
