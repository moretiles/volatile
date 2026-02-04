#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ANY CHANGE MADE TO VLTL_ISA SHOULD ACCOMPANY A CHANGE TO vltl_isa_valid
typedef enum vltl_isa {
    VLTL_ISA_UNSET,
    VLTL_ISA_INVALID,

    VLTL_ISA_8086,
    VLTL_ISA_INTEL32,
    VLTL_ISA_AMD64
} Vltl_isa;

bool vltl_isa_valid(const Vltl_isa isa);

#ifdef __cplusplus
}
#endif
