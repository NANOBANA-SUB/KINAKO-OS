#include "swich.h"
__attribute__((naked))
void context_switch(uint32_t *old_sp, uint32_t *new_sp) 
{
    __asm__ __volatile__ (
        // 実行中のコンテキストを保存
        "addi sp, sp, -13 * 4\n" // スタックポインタをさげる
        "sw ra, 0 * 4(sp)\n"
        "sw s0, 1 * 4(sp)\n"
        "sw s1, 2 * 4(sp)\n"
        "sw s2, 3 * 4(sp)\n"
        "sw s3, 4 * 4(sp)\n"
        "sw s4, 5 * 4(sp)\n"
        "sw s5, 6 * 4(sp)\n"
        "sw s6, 7 * 4(sp)\n"
        "sw s7, 8 * 4(sp)\n"
        "sw s8, 9 * 4(sp)\n"
        "sw s9, 10 * 4(sp)\n"
        "sw s10, 11 * 4(sp)\n"
        "sw s11, 12 * 4(sp)\n"

        // 古いスタックポインタを保存
        "sw sp, 0(a0)\n"
        // 新しいスタックポインタをロード
        "lw sp, 0(a1)\n"

        // 新しいコンテキストを復元
        "lw ra, 0 * 4(sp)\n"
        "lw s0, 1 * 4(sp)\n"
        "lw s1, 2 * 4(sp)\n"
        "lw s2, 3 * 4(sp)\n"
        "lw s3, 4 * 4(sp)\n"
        "lw s4, 5 * 4(sp)\n"
        "lw s5, 6 * 4(sp)\n"
        "lw s6, 7 * 4(sp)\n"
        "lw s7, 8 * 4(sp)\n"
        "lw s8, 9 * 4(sp)\n"
        "lw s9, 10 * 4(sp)\n"
        "lw s10, 11 * 4(sp)\n"  
        "lw s11, 12 * 4(sp)\n"
        "addi sp, sp, 13 * 4\n" // スタックポインタを戻す
        "ret\n"
    );
}