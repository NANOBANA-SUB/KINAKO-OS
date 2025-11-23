#pragma once
#include "common.h"
#include "proc.h"
//!!
//! コンテキストスイッチを行う
//! @param old_context 古いコンテクスト構造体
//! @param new_context 新しいコンテクスト構造体
//!!
void context_switch(struct context *old_context, struct context *new_context);