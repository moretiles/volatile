#include <global.h>

#include <string.h>
#include <errno.h>

Vltl_global_config vltl_global_config = {
    .isa = VLTL_ISA_AMD64,
    .pic_enabled = true
};
Vltl_global_context vltl_global_context = {
    .filename = "test.vltl",
    .function_name = "main",
    .line_number = 1
};
Vltl_global_registers vltl_global_registers = { 0 };

Vltl_global_register vltl_global_register_amd64_rax = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_RESERVED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_RESERVED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RAX
};

Vltl_global_register vltl_global_register_amd64_rbx = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RBX
};

Vltl_global_register vltl_global_register_amd64_rcx = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RCX
};
Vltl_global_register vltl_global_register_amd64_rdx = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_RESERVED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_RESERVED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RDX
};

Vltl_global_register vltl_global_register_amd64_rdi = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RDI
};
Vltl_global_register vltl_global_register_amd64_rsi = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RSI
};

Vltl_global_register vltl_global_register_amd64_rbp = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_INUSE,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_INUSE,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RBP
};

Vltl_global_register vltl_global_register_amd64_rsp = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_INUSE,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_INUSE,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_RSP
};

Vltl_global_register vltl_global_register_amd64_r8 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R8
};

Vltl_global_register vltl_global_register_amd64_r9 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R9
};

Vltl_global_register vltl_global_register_amd64_r10 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R10
};
Vltl_global_register vltl_global_register_amd64_r11 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R11
};

Vltl_global_register vltl_global_register_amd64_r12 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R12
};
Vltl_global_register vltl_global_register_amd64_r13 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R13
};

Vltl_global_register vltl_global_register_amd64_r14 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R14
};

Vltl_global_register vltl_global_register_amd64_r15 = {
    .isa = VLTL_ISA_AMD64,
    .status_current = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .status_default = VLTL_GLOBAL_REGISTER_STATUS_UNUSED,
    .as_amd64 = VLTL_GLOBAL_REGISTER_AMD64_R15
};

__attribute__((constructor)) int vltl_global_registers_init() {
    vltl_global_registers_reset();

    switch(vltl_global_config.isa) {
    case VLTL_ISA_AMD64:
        return vltl_global_registers_init_amd64();
        break;
    default:
        return EINVAL;
        break;
    }

    return EINVAL;
}

int vltl_global_registers_init_amd64() {
    _Static_assert(VLTL_GLOBAL_REGISTERS_AMD64_TOTAL <= VLTL_GLOBAL_REGISTERS_MAX_TOTAL, "The AMD64 architecture supports more general purpose registers than this program is designed to handle!");
    _Static_assert(VLTL_GLOBAL_REGISTERS_AMD64_UNUSED <= VLTL_GLOBAL_REGISTERS_MAX_TOTAL, "The AMD64 architecture supports more general purpose registers than this program is designed to handle!");
    _Static_assert(VLTL_GLOBAL_REGISTERS_AMD64_RESERVED <= VLTL_GLOBAL_REGISTERS_MAX_TOTAL, "The AMD64 architecture supports more general purpose registers than this program is designed to handle!");
    _Static_assert(VLTL_GLOBAL_REGISTERS_AMD64_INUSE <= VLTL_GLOBAL_REGISTERS_MAX_TOTAL, "The AMD64 architecture supports more general purpose registers than this program is designed to handle!");

    vltl_global_registers = (Vltl_global_registers) {
        .isa = VLTL_ISA_AMD64,
        .num_total = 0,
        .num_unused = 0,
        .num_reserved = 0,
        .num_inuse = 0
    };

    // unused, low cost
    vltl_global_registers_insert(&vltl_global_register_amd64_r11);
    vltl_global_registers_insert(&vltl_global_register_amd64_r10);
    vltl_global_registers_insert(&vltl_global_register_amd64_r9);
    vltl_global_registers_insert(&vltl_global_register_amd64_r8);
    vltl_global_registers_insert(&vltl_global_register_amd64_rcx);
    vltl_global_registers_insert(&vltl_global_register_amd64_rsi);
    vltl_global_registers_insert(&vltl_global_register_amd64_rdi);

    // unused, high cost
    vltl_global_registers_insert(&vltl_global_register_amd64_r15);
    vltl_global_registers_insert(&vltl_global_register_amd64_r14);
    vltl_global_registers_insert(&vltl_global_register_amd64_r13);
    vltl_global_registers_insert(&vltl_global_register_amd64_r12);
    vltl_global_registers_insert(&vltl_global_register_amd64_rbx);

    // reserved
    vltl_global_registers_insert(&vltl_global_register_amd64_rax);
    vltl_global_registers_insert(&vltl_global_register_amd64_rdx);

    // in-use
    vltl_global_registers_insert(&vltl_global_register_amd64_rbp);
    vltl_global_registers_insert(&vltl_global_register_amd64_rsp);

    vltl_global_registers_clear();

    if(
        (vltl_global_registers.num_total != VLTL_GLOBAL_REGISTERS_AMD64_TOTAL) ||
        (vltl_global_registers.num_unused != VLTL_GLOBAL_REGISTERS_AMD64_UNUSED) ||
        (vltl_global_registers.num_reserved != VLTL_GLOBAL_REGISTERS_AMD64_RESERVED) ||
        (vltl_global_registers.num_inuse != VLTL_GLOBAL_REGISTERS_AMD64_INUSE)
    ) {
        return ENOTRECOVERABLE;
    }

    return 0;
}

int vltl_global_registers_reset() {
    memset(&vltl_global_registers, 0, sizeof(Vltl_global_registers));

    return 0;
}

int vltl_global_registers_clear() {
    int ret = 0;
    for(size_t i = 0; i < vltl_global_registers.num_total; i++) {
        ret = vltl_global_registers_update(
                  vltl_global_registers.registers[i],
                  vltl_global_registers.registers[i]->status_default
              );

        if(ret != 0) {
            return ret;
        }
    }

    return 0;
}

int vltl_global_registers_insert(Vltl_global_register *reg) {
    if(reg == NULL) {
        return EINVAL;
    }

    switch(reg->status_current) {
    case VLTL_GLOBAL_REGISTER_STATUS_UNUSED:
        vltl_global_registers.num_unused++;
        break;
    case VLTL_GLOBAL_REGISTER_STATUS_RESERVED:
        vltl_global_registers.num_reserved++;
        break;
    case VLTL_GLOBAL_REGISTER_STATUS_INUSE:
        vltl_global_registers.num_inuse++;
        break;
    default:
        return EINVAL;
        break;
    }
    vltl_global_registers.registers[vltl_global_registers.num_total] = reg;
    vltl_global_registers.num_total++;

    return 0;
}

int vltl_global_registers_update(
    Vltl_global_register *reg, const Vltl_global_register_status status_current
) {
    if(reg == NULL) {
        return EINVAL;
    }

    bool present = false;
    for(size_t i = 0; i < vltl_global_registers.num_total; i++) {
        if(vltl_global_registers.registers[i] == reg) {
            present = true;
            break;
        }
    }

    if(!present) {
        return EINVAL;
    }

    switch(status_current) {
    case VLTL_GLOBAL_REGISTER_STATUS_UNUSED:
        vltl_global_registers.num_unused++;
        break;
    case VLTL_GLOBAL_REGISTER_STATUS_RESERVED:
        vltl_global_registers.num_reserved++;
        break;
    case VLTL_GLOBAL_REGISTER_STATUS_INUSE:
        vltl_global_registers.num_inuse++;
        break;
    default:
        return EINVAL;
        break;
    }

    switch(reg->status_current) {
    case VLTL_GLOBAL_REGISTER_STATUS_UNUSED:
        vltl_global_registers.num_unused--;
        break;
    case VLTL_GLOBAL_REGISTER_STATUS_RESERVED:
        vltl_global_registers.num_reserved--;
        break;
    case VLTL_GLOBAL_REGISTER_STATUS_INUSE:
        vltl_global_registers.num_inuse--;
        break;
    default:
        return ENOTRECOVERABLE;
        break;
    }

    reg->status_current = status_current;

    return 0;
}

int vltl_global_registers_use(Vltl_global_register **dest_ptr) {
    if(dest_ptr == NULL) {
        return EINVAL;
    }

    for(size_t i = 0; i < vltl_global_registers.num_total; i++) {
        if(vltl_global_registers.registers[i]->status_current == VLTL_GLOBAL_REGISTER_STATUS_UNUSED) {
            *dest_ptr = vltl_global_registers.registers[i];
            return vltl_global_registers_update(
                       vltl_global_registers.registers[i],
                       VLTL_GLOBAL_REGISTER_STATUS_INUSE
                   );
        }
    }

    return EXFULL;
}

int vltl_global_registers_reserve(Vltl_global_register *reserve_this) {
    if(reserve_this == NULL) {
        return EINVAL;
    }

    for(size_t i = 0; i < vltl_global_registers.num_total; i++) {
        if(vltl_global_registers.registers[i] == reserve_this) {
            if(vltl_global_registers.registers[i]->status_current == VLTL_GLOBAL_REGISTER_STATUS_INUSE) {
                return EXFULL;
            }

            return vltl_global_registers_update(
                       vltl_global_registers.registers[i],
                       VLTL_GLOBAL_REGISTER_STATUS_RESERVED
                   );
        }
    }

    return EINVAL;
}

int vltl_global_registers_inuse(Vltl_global_register *reserve_this) {
    if(reserve_this == NULL) {
        return EINVAL;
    }

    for(size_t i = 0; i < vltl_global_registers.num_total; i++) {
        if(vltl_global_registers.registers[i] == reserve_this) {
            if(vltl_global_registers.registers[i]->status_current == VLTL_GLOBAL_REGISTER_STATUS_INUSE) {
                return EXFULL;
            }

            return vltl_global_registers_update(
                       vltl_global_registers.registers[i],
                       VLTL_GLOBAL_REGISTER_STATUS_INUSE
                   );
        }
    }

    return EINVAL;
}
