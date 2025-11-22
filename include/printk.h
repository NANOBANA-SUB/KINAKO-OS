#pragma once
#include "common.h"
#include "console.h"

//!
//! フォーマット付きでカーネルログを出力する
//! @param fmt フォーマット文字列
//!
void printk(const char *fmt, ...);