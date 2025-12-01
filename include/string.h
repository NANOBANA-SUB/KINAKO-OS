#pragma once
#include "common.h"

//!
//! メモリを指定した値で設定する
//! @param s 設定するメモリ領域の先頭アドレス
//! @param c 設定する値
//! @param n 設定するバイト数
//! @return sの値
//!
void *memset(void *s, int c, size_t n);

//!
//! メモリをコピーする
//! @param dest コピー先のメモリ領域の先頭アドレス
//! @param src コピー元のメモリ領域の先頭アドレス
//! @param n コピーするバイト数
//!
void *memcpy(void *dest, const void *src, size_t n);

//!
//! 指定バイト数のメモリブロックを比較する
//! @param buf1 比較対象メモリのポインタ
//! @param buf2 比較対象メモリのポインタ
//! @param n 比較サイズ
//! @return buf1 > buf2の場合1, buf1 = buf2の場合0, buf1 < buf2の場合-1
int memcmp(const void *buf1, const void *buf2, size_t n);

//!
//! 文字列の長さを取得する
//! @param s 文字列の先頭アドレス
//! @return 文字列の長さ
//!
size_t strlen(const char *s);

//!
//! 文字列をコピーする
//! @param dest コピー先の文字列の先頭アドレス
//! @param src コピー元の文字列の先頭アドレス
//! @return destの値
//!
char *strcpy(char *dest, const char *src);

//!
//! 文字列を指定した長さだけコピーする
//! @param dest コピー先の文字列の先頭アドレス
//! @param src コピー元の文字列の先頭アドレス
//! @param n コピーする最大バイト数
//! @return destの値
//!
char *strncpy(char *dest, const char *src, size_t n);

//!
//! 文字列を連結する
//! @param dest 連結先の文字列の先頭アドレス
//! @param src 連結元の文字列の先頭アドレス
//! @return destの値
//!
char *strcat(char *dest, const char *src);

//!
//! 文字列を比較する
//! @param s1 比較する文字列1の先頭アドレス
//! @param s2 比較する文字列2の先頭アドレス
//! @return s1がs2より小さい場合は負の値、等しい場合は0、s1がs2より大きい場合は正の値
//!
int strcmp(const char *s1, const char *s2);

//!
//! 文字列を指定した文字数で比較する
//! @param s1 比較する文字列1のポインタ
//! @param s2 比較する文字列2のポインタ
//! @param n 比較する文字数
//! @return s1とs2の文字列が等しい場合0, s1とs2の文字列が異なる場合0以外
//!
int strncmp(const char *s1, const char *s2, size_t n);