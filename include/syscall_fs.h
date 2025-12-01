#pragma once
#include "common.h"

//!
//! 指定したパスからファイルをオープンする
//! @param user_path パス
//! @param omode オープンモード
//! @return 成功したら取得したFDを返す, 失敗したら-1
//!
int sys_open(const char *user_path, int omode);

//!
//! 指定したファイルディスクリプタを読み込む
//! @param fd ファイルディスクリプタ
//! @param user_buf 読み取り先のバッファ
//! @param n 読み取りサイズ
//! @return 失敗したら-1, 成功したらそれ以外
//!
int sys_read(int fd, void *user_buf, uint32_t n);

//!
//! 指定したファイルディスクリプタのファイルを閉じる
//! @param fd ファイルディスクリプタ
//! @return 成功したら0, 失敗したら-1
//!
int sys_close(int fd);