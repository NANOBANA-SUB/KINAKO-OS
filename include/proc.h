#pragma once
#include "common.h"

#define PROC_NAME_MAX 16 // プロセス名の最大長
#define PROC_MAX      64 // 最大プロセス数

#define PROC_UNUSED 0 // 未使用
#define PROC_RUNNABLE  1 // 実行可能

struct proc 
{
    uint32_t pid; // プロセスID
    uint32_t state; // プロセス状態
    vaddr32_t sp; // コンテクストスイッチ用スタックポインタ
    uint8_t stack[8192]; // カーネルスタック
};