#include <lang/local.h>
#include <global.h>

Vltl_lang_local *vltl_lang_local_create(
    const char *name,
    const Vltl_lang_type *type, Vltl_lang_type_attribute *attributes[9], Vltl_lang_literal *literal
) {
    Vltl_lang_local tmp = { 0 };
    Vltl_lang_local *local_ptr = &tmp;
    if(name == NULL || type == NULL) {
        return NULL;
    }

    int ret = vltl_lang_local_init(&local_ptr, local_ptr, name, type, attributes, literal);
    if(ret) {
        return NULL;
    }
    Vltl_lang_local *created = varena_alloc(&vltl_global_allocator, sizeof(Vltl_lang_local));
    if(created == NULL) {
        return NULL;
    }
    *created = tmp;

    return created;
}

int vltl_lang_local_init(
    Vltl_lang_local **dest, void *memory, const char *name,
    const Vltl_lang_type *type, Vltl_lang_type_attribute *attributes[9], Vltl_lang_literal *literal
) {
    if(dest == NULL || memory == NULL || type == NULL) {
        return EINVAL;
    }

    Vltl_lang_local *local = memory;
    *local = (Vltl_lang_local) {
        0
    };
    local->name = name;
    local->type = type;
    if(attributes != NULL) {
        memcpy(local->attributes, attributes, 9 * sizeof(Vltl_lang_type_attribute *));
    }
    local->literal = literal;
    local->frame_offset = 0;

    *dest = local;
    return 0;
}
