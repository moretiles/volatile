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

typedef enum iestack_error_kind {
    IESTACK_ERROR_KIND_FUNCTION,
    IESTACK_ERROR_KIND_GOTO
} Iestack_error_kind;

typedef struct iestack_error {
    int error_code;
    Iestack_error_kind error_kind;
    char msg[IESTACK_ERROR_STRLEN];

    const char *filename;
    size_t linenumber;
} Iestack_error;

typedef struct iestack {
    Vstack *error_stack;
} Iestack;

int iestack_error_init(
    Iestack_error *error,
    int error_code, Iestack_error_kind error_kind, const char *msg,
    const char *filename, size_t linenumber
);
void iestack_error_deinit(Iestack_error *error);

Iestack *iestack_create();
int iestack_init(Iestack *stack);
void iestack_deinit(Iestack *stack);
void iestack_destroy(Iestack *stack);
int iestack_push(
    Iestack *stack,
    int error_code, Iestack_error_kind error_kind, const char *msg,
    const char *filename, size_t linenumber
);
int iestack_dump(Iestack *stack, FILE *dest);
int iestack_push_direct(Iestack *stack, Iestack_error *src_error);
int iestack_pop(Iestack *stack, Iestack_error *dest_error);

#ifdef __cplusplus
extern thread_local char iestack_buffer_for_formatting[IESTACK_ERROR_STRLEN];
#else
extern _Thread_local char iestack_buffer_for_formatting[IESTACK_ERROR_STRLEN];
#endif

#ifdef __cplusplus
extern thread_local Iestack **iestack_global_errors;
#else
extern _Thread_local Iestack **iestack_global_errors;
#endif

#ifdef __cplusplus
extern thread_local int iestack_last_error;
#else
extern _Thread_local int iestack_last_error;
#endif

#define IESTACK_PUSH3(stack, error_code, error_kind, string) assert(!iestack_push(stack, error_code, error_kind, string, __FILE__, __LINE__))

#define IESTACK_PUSHF3(stack, error_code, error_kind, fstring, ...) do { \
    size_t printed_len___ = 0; \
    int ret___ = 0; \
    iestack_buffer_for_formatting[0] = 0; \
    BTRC_SNPRINTF( \
        &ret___, &printed_len___, iestack_buffer_for_formatting, IESTACK_ERROR_STRLEN, fstring, __VA_ARGS__ \
    ); \
    IESTACK_PUSH3(stack, error_code, error_kind, iestack_buffer_for_formatting); \
} while(0)

#define IESTACK_RETURN3(stack, error_code, error_kind, string) do { \
    IESTACK_PUSH3(stack, error_code, error_kind, string); \
    return error_code; \
} while(0)

#define IESTACK_RETURNF3(stack, error_code, error_kind, fstring, ...) do { \
    IESTACK_PUSHF3(stack, error_code, error_kind, fstring, __VA_ARGS__); \
    return error_code; \
} while(0)

#define IESTACK_HANDLE3(stack, error_expr, error_kind, string) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_RETURN3(stack, ___error_code, error_kind, string); \
    } \
} while(0)

#define IESTACK_HANDLEF3(stack, error_expr, error_kind, fstring, ...) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_RETURNF3(stack, ___error_code, error_kind, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_SUPPOSE3(stack, bool_expr, error_code, error_kind, string) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_RETURN3(stack, error_code, error_kind, string); \
    } \
} while(0)

#define IESTACK_SUPPOSEF3(stack, bool_expr, error_code, error_kind, fstring, ...) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_RETURNF3(stack, error_code, error_kind, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_HANDLE3_CALLBACK(stack, error_expr, error_kind, callback_expr, string) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        callback_expr; \
        IESTACK_RETURN3(stack, ___error_code, error_kind, string); \
    } \
} while(0)

#define IESTACK_HANDLEF3_CALLBACK(stack, error_expr, error_kind, callback_expr, fstring, ...) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        callback_expr; \
        IESTACK_RETURNF3(stack, ___error_code, error_kind, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_SUPPOSE3_CALLBACK(stack, bool_expr, error_code, error_kind, callback_expr, string) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        callback_expr; \
        IESTACK_RETURN3(stack, error_code, error_kind, string); \
    } \
} while(0)

#define IESTACK_SUPPOSEF3_CALLBACK(stack, bool_expr, error_code, error_kind, callback_expr, fstring, ...) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        callback_expr; \
        IESTACK_RETURNF3(stack, error_code, error_kind, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_HANDLE3_GOTO(stack, error_expr, error_kind, goto_label, string) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_PUSH(stack, ___error_code, error_kind, string); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_HANDLEF3_GOTO(stack, error_expr, error_kind, goto_label, fstring, ...) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_PUSHF(stack, ___error_code, error_kind, fstring, __VA_ARGS__); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_SUPPOSE3_GOTO(stack, bool_expr, error_code, error_kind, goto_label, string) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_PUSH3(stack, ___error_code, error_kind, string); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_SUPPOSEF3_GOTO(stack, bool_expr, error_code, error_kind, goto_label, fstring, ...) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_PUSHF3(stack, ___error_code, error_kind, fstring, __VA_ARGS__); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_PUSH2(stack, error_code, string) assert(!iestack_push(stack, error_code, IESTACK_ERROR_KIND_FUNCTION, string, __FILE__, __LINE__))

#define IESTACK_PUSHF2(stack, error_code, fstring, ...) do { \
    size_t printed_len___ = 0; \
    int ret___ = 0; \
    iestack_buffer_for_formatting[0] = 0; \
    BTRC_SNPRINTF( \
        &ret___, &printed_len___, iestack_buffer_for_formatting, IESTACK_ERROR_STRLEN, fstring, __VA_ARGS__ \
    ); \
    IESTACK_PUSH2(stack, error_code, iestack_buffer_for_formatting); \
} while(0)

#define IESTACK_RETURN2(stack, error_code, string) do { \
    IESTACK_PUSH2(stack, error_code, string); \
    return error_code; \
} while(0)

#define IESTACK_RETURNF2(stack, error_code, fstring, ...) do { \
    IESTACK_PUSHF2(stack, error_code, fstring, __VA_ARGS__); \
    return error_code; \
} while(0)

#define IESTACK_HANDLE2(stack, error_expr, string) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_RETURN2(stack, ___error_code, string); \
    } \
} while(0)

#define IESTACK_HANDLEF2(stack, error_expr, fstring, ...) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_RETURNF2(stack, ___error_code, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_SUPPOSE2(stack, bool_expr, error_code, string) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_RETURN2(stack, error_code, string); \
    } \
} while(0)

#define IESTACK_SUPPOSEF2(stack, bool_expr, error_code, fstring, ...) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_RETURNF2(stack, error_code, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_HANDLE2_CALLBACK(stack, error_expr, callback_expr, string) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        callback_expr; \
        IESTACK_RETURN2(stack, ___error_code, string); \
    } \
} while(0)

#define IESTACK_HANDLEF2_CALLBACK(stack, error_expr, callback_expr, fstring, ...) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        callback_expr; \
        IESTACK_RETURNF2(stack, ___error_code, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_SUPPOSE2_CALLBACK(stack, bool_expr, error_code, callback_expr, string) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        callback_expr; \
        IESTACK_RETURN2(stack, error_code, string); \
    } \
} while(0)

#define IESTACK_SUPPOSEF2_CALLBACK(stack, bool_expr, error_code, callback_expr, fstring, ...) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        callback_expr; \
        IESTACK_RETURNF2(stack, error_code, fstring, __VA_ARGS__); \
    } \
} while(0)

#define IESTACK_HANDLE2_GOTO(stack, error_expr, goto_label, string) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_PUSH3(stack, ___error_code, IESTACK_ERROR_KIND_GOTO, string); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_HANDLEF2_GOTO(stack, error_expr, goto_label, fstring, ...) do { \
    const int ___error_code = error_expr; \
    if(___error_code) { \
        IESTACK_PUSHF3(stack, ___error_code, IESTACK_ERROR_KIND_GOTO, fstring, __VA_ARGS__); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_SUPPOSE2_GOTO(stack, bool_expr, error_code, goto_label, string) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_PUSH3(stack, error_code, IESTACK_ERROR_KIND_GOTO, string); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_SUPPOSEF2_GOTO(stack, bool_expr, error_code, goto_label, fstring, ...) do { \
    const bool ___bool_value = bool_expr; \
    if(!___bool_value) { \
        IESTACK_PUSHF3(stack, error_code, IESTACK_ERROR_KIND_GOTO, fstring, __VA_ARGS__); \
        goto goto_label; \
    } \
} while(0)

#define IESTACK_PUSH(error_expr, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_PUSH2(*iestack_global_errors, error_expr, string); \
} while(0)

#define IESTACK_PUSHF(error_expr, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_PUSHF2(*iestack_global_errors, error_expr, fstring, __VA_ARGS__); \
} while(0)

#define IESTACK_RETURN(error_expr, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_RETURN2(*iestack_global_errors, error_expr, string); \
} while(0)

#define IESTACK_RETURNF(error_expr, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_RETURNF2(*iestack_global_errors, error_expr, fstring, __VA_ARGS__); \
} while(0)

#define IESTACK_HANDLE(error_expr, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_HANDLE2(*iestack_global_errors, error_expr, string); \
} while(0)

#define IESTACK_HANDLEF(error_expr, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_HANDLEF2(*iestack_global_errors, error_expr, fstring, __VA_ARGS__); \
} while(0)

#define IESTACK_SUPPOSE(bool_expr, error_code, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_SUPPOSE2(*iestack_global_errors, bool_expr, error_code, string); \
} while(0)

#define IESTACK_SUPPOSEF(bool_expr, error_code, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_SUPPOSEF2(*iestack_global_errors, bool_expr, error_code, fstring, __VA_ARGS__); \
} while(0)

#define IESTACK_HANDLE_CALLBACK(error_expr, callback_expr, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_HANDLE2_CALLBACK(*iestack_global_errors, error_expr, callback_expr, string); \
} while(0)

#define IESTACK_HANDLEF_CALLBACK(error_expr, callback_expr, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_HANDLEF2_CALLBACK(*iestack_global_errors, error_expr, callback_expr, fstring, __VA_ARGS__); \
} while(0)

#define IESTACK_SUPPOSE_CALLBACK(bool_expr, error_code, callback_expr, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_SUPPOSE2_CALLBACK(*iestack_global_errors, bool_expr, error_code, callback_expr, string); \
} while(0)

#define IESTACK_SUPPOSEF_CALLBACK(bool_expr, error_code, callback_expr, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_SUPPOSEF2_CALLBACK( \
        *iestack_global_errors, bool_expr, error_code, callback_expr, fstring, __VA_ARGS__ \
    ); \
} while(0)

#define IESTACK_HANDLE_GOTO(error_expr, goto_label, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_HANDLE2_GOTO(*iestack_global_errors, error_expr, goto_label, string); \
} while(0)

#define IESTACK_HANDLEF_GOTO(error_expr, goto_label, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_HANDLEF2_GOTO(*iestack_global_errors, error_expr, goto_label, fstring, __VA_ARGS__); \
} while(0)

#define IESTACK_SUPPOSE_GOTO(bool_expr, error_code, goto_label, string) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_SUPPOSE2_GOTO(*iestack_global_errors, bool_expr, error_code, goto_label, string); \
} while(0)

#define IESTACK_SUPPOSEF_GOTO(bool_expr, error_code, goto_label, fstring, ...) do { \
    assert(iestack_global_errors != NULL); \
    IESTACK_SUPPOSEF2_GOTO( \
        *iestack_global_errors, bool_expr, error_code, goto_label, fstring, __VA_ARGS__ \
    ); \
} while(0)

#ifdef I_LIKE_BAD_IDEAS_A_LOT
// and_am_using_c23
void print_int(int i, int e, char *s);
void print_bool(bool b, int e, char *s);
struct hat {
    int a;
    int b;
};
void print_hat(struct hat h, int e, char *s);
#define FAIL_OPEN(expr, error_code, string) do { \
    auto ___eval_expr = expr; \
    _Generic((___eval_expr), \
        bool: print_bool, \
        int: print_int, \
        struct hat: print_hat \
    )(___eval_expr, error_code, string); \
} while(0)
#endif

#ifdef __cplusplus
}
#endif
