#pragma once
#include "common.h"

//!
//! タイマーの初期化を行う
//!
void timer_init(void);

//!
//! タイマーをインターバル分進める
//! @note インターバルの定数は実装ファイルに存在する
//!
void timer_set_next(void);