#pragma once

#include <isa.h>
#include <ds/vqueue.h>
#include <ds/varena.h>
#include <ds/nkht.h>
#include <lang/function.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLTL_GLOBAL_REGISTERS_MAX_TOTAL (128)
#define VLTL_GLOBAL_REGISTERS_8086_TOTAL (8)
#define VLTL_GLOBAL_REGISTERS_INTEL32_TOTAL (8)
#define VLTL_GLOBAL_REGISTERS_AMD64_TOTAL (16)

#define VLTL_GLOBAL_REGISTERS_AMD64_UNUSED (12)
#define VLTL_GLOBAL_REGISTERS_AMD64_RESERVED (2)
#define VLTL_GLOBAL_REGISTERS_AMD64_INUSE (2)

typedef struct vltl_global_config {
    Vltl_isa isa;

    bool pic_enabled;
} Vltl_global_config;

typedef enum vltl_global_context_within {
    VLTL_GLOBAL_CONTEXT_WITHIN_UNSET,
    VLTL_GLOBAL_CONTEXT_WITHIN_INVALID,

    VLTL_GLOBAL_CONTEXT_WITHIN_IMPORT,
    VLTL_GLOBAL_CONTEXT_WITHIN_CONSTANT,
    VLTL_GLOBAL_CONTEXT_WITHIN_GLOBAL,
    VLTL_GLOBAL_CONTEXT_WITHIN_STRUCT,
    VLTL_GLOBAL_CONTEXT_WITHIN_FUNCTION,
    VLTL_GLOBAL_CONTEXT_WITHIN_LOCAL
} Vltl_global_context_within;

typedef struct vltl_global_context {
    Vltl_global_context_within within_what;

    const char *filename;
    Vltl_lang_function *function;
    size_t line_number;
} Vltl_global_context;

typedef enum vltl_global_register_8086 {
    VLTL_GLOBAL_REGISTER_8086_UNSET,
    VLTL_GLOBAL_REGISTER_8086_INVALID
} Vltl_global_register_8086;

typedef enum vltl_global_register_intel32 {
    VLTL_GLOBAL_REGISTER_INTEL32_UNSET,
    VLTL_GLOBAL_REGISTER_INTEL32_INVALID
} Vltl_global_register_intel32;

typedef enum vltl_global_register_amd64 {
    VLTL_GLOBAL_REGISTER_AMD64_UNSET,
    VLTL_GLOBAL_REGISTER_AMD64_INVALID,

    VLTL_GLOBAL_REGISTER_AMD64_RAX,
    VLTL_GLOBAL_REGISTER_AMD64_RBX,
    VLTL_GLOBAL_REGISTER_AMD64_RCX,
    VLTL_GLOBAL_REGISTER_AMD64_RDX,
    VLTL_GLOBAL_REGISTER_AMD64_RDI,
    VLTL_GLOBAL_REGISTER_AMD64_RSI,
    VLTL_GLOBAL_REGISTER_AMD64_RBP,
    VLTL_GLOBAL_REGISTER_AMD64_RSP,
    VLTL_GLOBAL_REGISTER_AMD64_R8,
    VLTL_GLOBAL_REGISTER_AMD64_R9,
    VLTL_GLOBAL_REGISTER_AMD64_R10,
    VLTL_GLOBAL_REGISTER_AMD64_R11,
    VLTL_GLOBAL_REGISTER_AMD64_R12,
    VLTL_GLOBAL_REGISTER_AMD64_R13,
    VLTL_GLOBAL_REGISTER_AMD64_R14,
    VLTL_GLOBAL_REGISTER_AMD64_R15,

    VLTL_GLOBAL_REGISTER_AMD64_EOF
} Vltl_global_register_amd64;

typedef enum vltl_global_register_status {
    VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    VLTL_GLOBAL_REGISTER_STATUS_RESERVED,
    VLTL_GLOBAL_REGISTER_STATUS_INUSE
} Vltl_global_register_status;

typedef struct vltl_global_register {
    Vltl_isa isa;
    Vltl_global_register_status status_current;
    Vltl_global_register_status status_default;

    union {
        Vltl_global_register_8086 as_8086;
        Vltl_global_register_intel32 as_intel32;
        Vltl_global_register_amd64 as_amd64;
    };
} Vltl_global_register;

// information on what registers are currently in-use
typedef struct vltl_global_registers {
    Vltl_isa isa;
    Vltl_global_register *registers[VLTL_GLOBAL_REGISTERS_MAX_TOTAL];

    size_t num_total;
    size_t num_unused;
    size_t num_reserved;
    size_t num_inuse;
} Vltl_global_registers;

__attribute__((constructor)) int vltl_global_init(void);

// initialize config singleton
int vltl_global_config_init(void);

// initialize context singleton
int vltl_global_context_init(void);

// initialize registers singleton
int vltl_global_registers_init(void);

// initialize the global allocator
int vltl_global_allocator_init(void);

// initialize the global errors stack
int vltl_global_errors_init(void);

// initialize the global tables
int vltl_global_table_init(void);
int vltl_global_table_constants_init(void);
int vltl_global_table_globals_init(void);
int vltl_global_table_locals_init(void);
int vltl_global_table_types_init(void);
int vltl_global_table_operations_init(void);
int vltl_global_table_attributes_init(void);
int vltl_global_table_functions_init(void);

// initialize when isa is AMD64
int vltl_global_registers_init_amd64(void);

// remove all registers from the global registers singleton
int vltl_global_registers_reset(void);

// update all registers to have their default value
int vltl_global_registers_clear(void);

// add new register to the global registers singleton
int vltl_global_registers_insert(Vltl_global_register *reg);

// update status of an existing register in the registers singleton
int vltl_global_registers_update(
    Vltl_global_register *reg, const Vltl_global_register_status status_current
);

// move the lowest cost, unused register from the unused linked list to the used linked list
int vltl_global_registers_use(Vltl_global_register **dest_ptr);

// reserve specific register if it is not already in-use
int vltl_global_registers_reserve(Vltl_global_register *reserve_this);

// mark specific register as being in-use if it is not already in-use
int vltl_global_registers_inuse(Vltl_global_register *reserve_this);

// global structs
extern struct vltl_global_config vltl_global_config;
extern struct vltl_global_context vltl_global_context;
extern struct vltl_global_registers vltl_global_registers;
extern struct varena *vltl_global_allocator;
extern struct iestack vltl_global_errors;

extern struct nkht *vltl_global_table_constants;
extern struct nkht *vltl_global_table_globals;
extern struct nkht *vltl_global_table_locals;
extern struct nkht *vltl_global_table_types;
extern struct nkht *vltl_global_table_functions;
// both of these would act as constants
extern struct nkht *vltl_global_table_operations;
extern struct nkht *vltl_global_table_attributes;

// amd64 structs
extern const struct vltl_global_register vltl_global_registers_amd64[];

extern struct vltl_global_register vltl_global_register_amd64_rax;
extern struct vltl_global_register vltl_global_register_amd64_rbx;
extern struct vltl_global_register vltl_global_register_amd64_rcx;
extern struct vltl_global_register vltl_global_register_amd64_rdx;
extern struct vltl_global_register vltl_global_register_amd64_rdi;
extern struct vltl_global_register vltl_global_register_amd64_rsi;
extern struct vltl_global_register vltl_global_register_amd64_rbp;
extern struct vltl_global_register vltl_global_register_amd64_rsp;
extern struct vltl_global_register vltl_global_register_amd64_r8;
extern struct vltl_global_register vltl_global_register_amd64_r9;
extern struct vltl_global_register vltl_global_register_amd64_r10;
extern struct vltl_global_register vltl_global_register_amd64_r11;
extern struct vltl_global_register vltl_global_register_amd64_r12;
extern struct vltl_global_register vltl_global_register_amd64_r13;
extern struct vltl_global_register vltl_global_register_amd64_r14;
extern struct vltl_global_register vltl_global_register_amd64_r15;

#ifdef __cplusplus
}
#endif
