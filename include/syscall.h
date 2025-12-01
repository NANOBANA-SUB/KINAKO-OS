#pragma once
#include "common.h"

#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_READ 3
#define SYS_CLOSE 4
#define SYS_PUTCHAR 5
#define SYS_GETCHAR 6

//!
//! システムコールの呼び出しを行う
//! @param syscall_no システムコール
//! @param arg0
//! @param arg1
//! @param arg2
//!
int syscall(uint32_t syscall_no, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2);