#include <lang/symbol.h>

#include <stddef.h>

bool vltl_lang_symbol_valid(const Vltl_lang_symbol symbol) {
    switch(symbol.scope) {
    case VLTL_LANG_SYMBOL_SCOPE_LOCAL:
    case VLTL_LANG_SYMBOL_SCOPE_GLOBAL:
    case VLTL_LANG_SYMBOL_SCOPE_INTRINSIC:
        break;
    default:
        return false;
        break;
    }

    if(symbol.name == NULL) {
        return false;
    }

    if(symbol.type == NULL) {
        return false;
    }

    return true;
}
