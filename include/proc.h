#pragma once
#include "common.h"

#define CPU_MAX      4  // 最大CPU数

#define PROC_NAME_MAX 16 // プロセス名の最大長
#define PROC_MAX      64 // 最大プロセス数

#define KSTACK_SIZE   4096 // カーネルスタックサイズ

//!
//! コンテキスト情報を保持する構造体
//! @note スイッチ時に保存・復元されるレジスタを格納する
//!
struct context
{
    uint32_t ra;    // return address
    uint32_t sp;    // stack pointer
    uint32_t s0;    // callee-saved register
    uint32_t s1;    // callee-saved register
    uint32_t s2;    // callee-saved register
    uint32_t s3;    // callee-saved register
    uint32_t s4;    // callee-saved register
    uint32_t s5;    // callee-saved register
    uint32_t s6;    // callee-saved register
    uint32_t s7;    // callee-saved register
    uint32_t s8;    // callee-saved register
    uint32_t s9;    // callee-saved register
    uint32_t s10;   // callee-saved register
    uint32_t s11;   // callee-saved register
};

//!
//! CPU情報を保持する構造体
//! @note CPUごとに1つ存在する
//!
struct cpu
{
    //! @note 現状はシングルコア対応のみのため、CPU IDは0固定
    struct proc* current_proc;  // 現在実行中のプロセス
    struct context context;     // CPUコンテキスト
    uint32_t cpu_id;            // CPUのID
    uint32_t noff;              // 割り込み無効化のネストカウント
    uint32_t intena;            // 割り込み有効化前の状態
};

//!
//! プロセス状態を表す列挙型
//!
enum proc_state 
{
    PROC_STATE_UNUSED = 0,  // 未使用
    PROC_STATE_READY,       // 使用可能
    PROC_STATE_RUNNING,     // 実行中
    PROC_STATE_ZOMBIE,      // ゾンビ状態
};

//!
//! プロセス情報を保持する構造体
//! @note システム内の全プロセス分だけ存在する
//!
struct proc 
{
    uint32_t        pid;                    // プロセスID
    enum proc_state state;                  // プロセスの状態

    struct context  context;                // プロセスコンテキスト

    uint32_t        *pagetable;             // ページテーブル

    void            *kstack_base;           // カーネルスタックのベースアドレス
    void            *kstack_top;            // カーネルスタックのトップアドレス

    char            name[PROC_NAME_MAX];    // プロセス名
};

extern struct proc *g_current_proc; // 現在実行中のプロセス

//!
//! プロセス管理の初期化
//! @note boot時に一度だけ呼び出される
//!
void proc_init(void);

//!
//! プロセスのエントリポイント関数型
//!
typedef void (*proc_entry_t)(void *arg);

//!
//! 新しいプロセスを作成する
//! @param entry プロセスのエントリポイント関数
//! @param arg プロセスに渡す引数（未使用）
//! @param name プロセス名
//! @return 作成したプロセスのプロセスID
//!
uint32_t proc_create(proc_entry_t entry, void* arg, const char* name);

//!
//! スケジューラを起動する
//! @note この関数は戻らない
//!
void scheduler_start(void) __attribute__((noreturn));

//!
//! 現在のプロセスをCPUから解放し、他のプロセスに実行を移す
//!
void yield(void);

//!
//! タイマー割り込み時に呼び出されるスケジューラ関数
//!
void schedule_tick(void);

//!
//! 現在のプロセスを終了する
//! @note この関数は戻らない
//!
void proc_exit(void) __attribute__((noreturn));

//!
//! 現在のプロセスを取得する
//! @return 現在のプロセスのポインタ
//!
struct proc* get_current_proc(void);

//!
//! 現在のCPUを取得する
//! @return 現在のCPUのポインタ
//!
struct cpu* mycpu(void);