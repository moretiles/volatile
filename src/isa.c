#include <isa.h>

bool vltl_isa_valid(const Vltl_isa isa) {
    switch(isa) {
    case VLTL_ISA_8086:
    case VLTL_ISA_INTEL32:
    case VLTL_ISA_AMD64:
        return true;
        break;
    default:
        return false;
        break;
    }

    return false;
}
