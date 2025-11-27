#pragma once
#include "common.h"

#define SYS_PUTCHAR 1

//!
//! システムコールの呼び出しを行う
//! @param syscall_no システムコール
//! @param arg0
//! @param arg1
//! @param arg2
//!
uint32_t syscall(uint32_t syscall_no, int arg0, int arg1, int arg2);