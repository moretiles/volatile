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
    dest->stack_frame_size = 0;
    dest->stack_frame_cap = 0;
    dest->num_tmp_variables = 0;
    // TODO: Store registers used by function in serious way
    //   Will require that, for the current ISA, enqueue every global register as a key with its value set to false
    dest->registers_used = vht_create(sizeof(Vltl_global_register *), sizeof(bool));
    if(dest->registers_used == NULL) {
        return ENOMEM;
    }

    return 0;
}

void vltl_lang_function_deinit(Vltl_lang_function *function) {
    if(function == NULL) {
        return;
    }

    vht_destroy(function->registers_used);
    function->registers_used = NULL;

    return;
}

bool vltl_lang_function_local_exists_here(Vltl_lang_function *function, const char *name) {
    if(function == NULL || name == NULL) {
        return false;
    }

    Vltl_lang_local *local = NULL;
    size_t index = vltl_global_context.indentation_level - 1;
    Nkht *local_variables = &(vltl_global_context.bodies[index].local_variables);
    if(nkht_get(local_variables, name, &local)) {
        return false;
    }

    if(local == NULL) {
        return false;
    }

    return true;
}

bool vltl_lang_function_local_exists(Vltl_lang_function *function, const char *name) {
    if(function == NULL || name == NULL) {
        return false;
    }

    Vltl_lang_local *local = NULL;
    if(vltl_lang_function_local_get(&local, function, name)) {
        return false;
    }

    if(local == NULL) {
        return false;
    }

    return true;
}

int vltl_lang_function_local_get(Vltl_lang_local **dest, Vltl_lang_function *function, const char *name) {
    int ret = ENODATA;
    if(function == NULL || name == NULL) {
        return EINVAL;
    }

    for(size_t i = vltl_global_context.indentation_level; i > 0; i--) {
        ret = nkht_get(&(vltl_global_context.bodies[i - 1].local_variables), name, dest);
        if(ret == ENODATA) {
            continue;
        } else if(ret == 0) {
            break;
        } else {
            return ret;
        }
    }
    return ret;
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

    ret = nkht_get(
              &(vltl_global_context.bodies[vltl_global_context.indentation_level - 1].local_variables),
              name,
              &local
          );
    if(ret != ENODATA) {
        // already exists
        return EINVAL;
    }

    local = vltl_lang_local_create(name, type, attributes, literal);
    if(local == NULL) {
        return ENOMEM;
    }

    ret = nkht_set(
              &(vltl_global_context.bodies[vltl_global_context.indentation_level - 1].local_variables),
              local->name,
              &local
          );
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
