#pragma once
#include "common.h"

//!
//! コンソールに1文字出力する
//! @param ch 出力する文字
//!
void console_putc(char ch);

//!
//! コンソールに文字列を出力する
//! @param buf 出力する文字列
//!
void console_write(const char *buf);