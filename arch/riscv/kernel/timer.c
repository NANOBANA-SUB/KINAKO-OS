#include "timer.h"

#define SBI_SET_TIMER 0

static inline void __sbi_set_timer(uint64_t stime_value)
{
    uint32_t lo = (uint32_t)stime_value;
    uint32_t hi = (uint32_t)(stime_value >> 32);

    register uint32_t a0 asm("a0") = lo;
    register uint32_t a1 asm("a1") = hi;
    register uint32_t a7 asm("a7") = SBI_SET_TIMER;

    asm volatile (
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a7)
        : "memory"
    );
}

static inline uint64_t __read_time(void)
{
    uint32_t lo;
    asm volatile ("rdtime %0" : "=r"(lo));
    return (uint64_t)lo;
}

#define TICK_INTERVAL 1000

void timer_set_next(void)
{
    uint64_t now = __read_time();
    __sbi_set_timer(now + TICK_INTERVAL);
}

void timer_init(void)
{
    timer_set_next();

    // SIE.STIE を有効化（Supervisor Timer Interrupt）
    asm volatile(
        "csrs sie, %0\n"
        :
        : "r"(1 << 5)   // STIE bit
    );

    // sstatus.SIE も有効化しておく（割り込み全体の許可）
    asm volatile(
        "csrs sstatus, %0\n"
        :
        : "r"(1 << 1)   // SIE bit
    );
}