#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_LANG_LITERAL_ATTRIBUTES_CAP (9LU)
#define VLTL_LANG_LITERAL_FIELDS_CAP (9)

typedef struct vltl_lang_literal {
    const char *name;
    const Vltl_lang_type *type;
    Vltl_lang_type_attribute *attributes[VLTL_LANG_LITERAL_ATTRIBUTES_CAP];
    void *fields[VLTL_LANG_LITERAL_FIELDS_CAP];
} Vltl_lang_literal;

#ifdef __cplusplus
}
#endif
