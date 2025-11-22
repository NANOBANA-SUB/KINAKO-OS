#pragma once
#include "common.h"

//!!
//! コンテキストスイッチを行う
//! @param old_sp 古いスタックポインタの保存先
//! @param new_sp 新しいスタックポインタの読み込み元
//!!
void context_switch(uint32_t *old_sp, uint32_t *new_sp);