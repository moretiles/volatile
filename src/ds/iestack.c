#include <ds/vstack.h>
#include <ds/iestack.h>

#include <assert.h>

int iestack_error_init(
    Iestack_error *error, int error_code, const char *msg, const char *filename, size_t linenumber
) {
    if(error == NULL) {
        return EINVAL;
    }

    *error = (Iestack_error) {
        0
    };
    error->error_code = error_code;
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

int iestack_push(Iestack *stack, int error_code, const char *msg, const char *filename, size_t linenumber) {
    Iestack_error current_error = { 0 };
    if(stack == NULL || msg == NULL || filename == NULL) {
        return EINVAL;
    }

    int ret = iestack_error_init(&current_error, error_code, msg, filename, linenumber);
    if(ret) {
        return ret;
    }

    return iestack_push_direct(stack, &current_error);
}

int iestack_push_direct(Iestack *stack, Iestack_error *src_error) {
    if(stack == NULL || src_error == NULL) {
        return EINVAL;
    }

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
        fprintf(dest, "        msg: %s\n", current_error.msg);
        fprintf(dest, "        filename: %s\n", current_error.filename);
        fprintf(dest, "        linenumber: %lu\n", current_error.linenumber);
        fprintf(dest, "    },\n");
    }
    fprintf(dest, "]\n");

    return 0;
}
