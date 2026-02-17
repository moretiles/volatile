#include <global.h>
#include <lang/function.h>

int vltl_lang_function_init(Vltl_lang_function *dest, const char *name) {
    if(dest == NULL || name == NULL) {
        return EINVAL;
    }

    *dest = (Vltl_lang_function) {
        0
    };
    dest->parse_status = VLTL_LANG_FUNCTION_STATUS_INCOMPLETE;
    dest->name = name;
    dest->return_type = &vltl_lang_type_long;
    dest->expected_argc = 0;
    dest->locals = nkht_create(sizeof(Vltl_lang_local *));
    if(dest->locals == NULL) {
        return ENOMEM;
    }
    dest->stack_frame_size = 0;
    dest->stack_frame_cap = 0;
    // TODO: Store registers used by function in serious way
    //   Will require that, for the current ISA, enqueue every global register as a key with its value set to false
    dest->registers_used = vht_create(sizeof(Vltl_global_register *), sizeof(bool));
    if(dest->registers_used == NULL) {
        return ENOMEM;
    }

    return 0;
}

int vltl_lang_function_local_get(Vltl_lang_local **dest, Vltl_lang_function *function, const char *name) {
    if(function == NULL || name == NULL) {
        return EINVAL;
    }

    return nkht_get(function->locals, name, dest);
}

int vltl_lang_function_local_set(
    Vltl_lang_function *function, const char *name, const Vltl_lang_type *type,
    Vltl_lang_type_attribute *attributes[9], Vltl_lang_literal *literal
) {
    int ret = 0;
    Vltl_lang_local *local = NULL;
    if(function == NULL || name == NULL || type == NULL) {
        return EINVAL;
    }

    ret = nkht_get(function->locals, name, &local);
    if(ret != ENODATA) {
        // already exists
        return EINVAL;
    }

    local = vltl_lang_local_create(name, type, attributes, literal);
    if(local == NULL) {
        return ENOMEM;
    }

    ret = nkht_set(function->locals, local->name, &local);
    if(ret) {
        return ret;
    }

    if(function->stack_frame_cap == function->stack_frame_size) {
        // TODO: Use the type of the local to determine how much to increment stack by
        function->stack_frame_cap += 8;
    }
    function->stack_frame_size += 8;
    local->frame_offset = function->stack_frame_size;

    return 0;
}
