#pragma once

// satp modes
#define SATP_SV32     1UL << 31
#define SATP_SV39     8UL << 60

// sstatus register flags
#define SSTATUS_SPIE     (1UL << 5)
#define SSTATUS_SUM      (1UL << 18)

// scause values
#define SCAUSE_ECALL    8

// CSR access helpers
#define READ_CSR(reg) ({          \
    unsigned long __tmp;              \
    asm volatile ("csrr %0, " #reg    \
                  : "=r"(__tmp));     \
    __tmp;                             \
})

#define WRITE_CSR(reg, val) ({     \
    asm volatile ("csrw " #reg ", %0" \
                  :                  \
                  : "rK"(val));      \
})
