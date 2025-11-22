#pragma once
#include "common.h"

//!
//! 現在の割り込み有効/無効状態を取得する
//! @return 有効なら1、無効なら0
//!
bool intr_get(void);

//!
//! 現在の割り込みを有効化する
//!
void intr_on(void);

//!
//! 現在の割り込みを無効化する
//!
void intr_off(void);