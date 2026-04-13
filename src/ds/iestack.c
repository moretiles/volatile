#include <ds/vstack.h>
#include <ds/iestack.h>

#include <assert.h>

_Thread_local char iestack_buffer_for_formatting[IESTACK_ERROR_STRLEN];
_Thread_local Iestack **iestack_global_errors;
_Thread_local int iestack_last_error;

int iestack_error_init(
    Iestack_error *error,
    int error_code, Iestack_error_kind error_kind, const char *msg,
    const char *filename, size_t linenumber
) {
    if(error == NULL) {
        return EINVAL;
    }

    *error = (Iestack_error) {
        0
    };
    error->error_code = error_code;
    error->error_kind = error_kind;
    size_t msg_len = strlen(msg);
    if(msg_len > (IESTACK_ERROR_STRLEN - 1)) {
        msg_len = IESTACK_ERROR_STRLEN - 1;
    }
    memcpy(&(error->msg), msg, msg_len);
    error->msg[msg_len] = 0;
    error->filename = filename;
    error->linenumber = linenumber;

    return 0;
}

void iestack_error_deinit(Iestack_error *error) {
    if(error == NULL) {
        return;
    }

    *error = (Iestack_error) {
        0
    };

    return;
}

Iestack *iestack_create(void) {
    Iestack *created_here = malloc(sizeof(Iestack));
    if(created_here == NULL) {
        return NULL;
    }

    int ret = iestack_init(created_here);
    if(ret) {
        free(created_here);
        created_here = NULL;
        return NULL;
    }

    return created_here;
}

int iestack_init(Iestack *stack) {
    if(stack == NULL) {
        return EINVAL;
    }

    stack->error_stack = vstack_create(sizeof(Iestack_error), IESTACK_CAP_ERRORS);
    if(stack->error_stack == NULL) {
        return ENOMEM;
    }

    return 0;
}

void iestack_deinit(Iestack *stack) {
    if(stack == NULL) {
        return;
    }

    if(stack->error_stack != NULL) {
        vstack_destroy(stack->error_stack);
        stack->error_stack = NULL;
    }

    return;
}

void iestack_destroy(Iestack *stack) {
    iestack_deinit(stack);
    free(stack);
}

int iestack_push(
    Iestack *stack,
    int error_code, Iestack_error_kind error_kind, const char *msg,
    const char *filename, size_t linenumber
) {
    Iestack_error current_error = { 0 };
    if(stack == NULL || msg == NULL || filename == NULL) {
        return EINVAL;
    }

    int ret = iestack_error_init(&current_error, error_code, error_kind, msg, filename, linenumber);
    if(ret) {
        return ret;
    }

    return iestack_push_direct(stack, &current_error);
}

int iestack_push_direct(Iestack *stack, Iestack_error *src_error) {
    if(stack == NULL || src_error == NULL) {
        return EINVAL;
    }

    iestack_last_error = src_error->error_code;
    return vstack_push(stack->error_stack, src_error);
}

int iestack_pop(Iestack *stack, Iestack_error *dest_error) {
    if(stack == NULL || dest_error == NULL) {
        return EINVAL;
    }

    return vstack_pop(stack->error_stack, dest_error);
}

int iestack_dump(Iestack *stack, FILE *dest) {
    Iestack_error current_error = { 0 };
    if(stack == NULL || dest == NULL) {
        return EINVAL;
    }

    if(vstack_len(stack->error_stack) == 0) {
        return 0;
    }

    int ret = 0;
    bool done = false;

    fprintf(dest, "Errors encountered = [\n");
    while(!done) {
        ret = iestack_pop(stack, &current_error);
        if(ret == ENODATA) {
            done = true;
            continue;
        } else if (ret == 0) {
            // all good
        } else {
            return ret;
        }

        fprintf(dest, "    {\n");
        fprintf(dest, "        error_code: %s (%i)\n",
                strerror(current_error.error_code), current_error.error_code
               );
        switch(current_error.error_kind) {
        case IESTACK_ERROR_KIND_FUNCTION:
            fprintf(dest, "        error_kind: FUNCTION\n");
            break;
        case IESTACK_ERROR_KIND_GOTO:
            fprintf(dest, "        error_kind: GOTO\n");
            break;
        default:
            break;
        }
        fprintf(dest, "        msg: %s\n", current_error.msg);
        fprintf(dest, "        filename: %s\n", current_error.filename);
        fprintf(dest, "        linenumber: %zu\n", current_error.linenumber);
        fprintf(dest, "    },\n");
    }
    fprintf(dest, "]\n");

    return 0;
}

#ifdef I_LIKE_BAD_IDEAS_A_LOT
#include <stdio.h>
void print_int(int i, int e, char *s) {
    printf("is int!\n");
}
void print_bool(bool b, int e, char *s) {
    printf("is bool!\n");
}
void print_hat(struct hat hat, int e, char *s) {
    printf("is hat!\n");
}
#endif
