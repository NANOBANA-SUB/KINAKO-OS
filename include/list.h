#pragma once
#include "common.h"

//!
//! 双方向連結リストのノード構造体
//!
struct list_node 
{
    struct list_node* prev; // 前のノードへのポインタ
    struct list_node* next; // 次のノードへのポインタ
};

//!
//! 双方向連結リストの初期化
//! @param head リストの先頭ノードへのポインタ
//!
void list_init(struct list_node* head);

//!
//! ノードをリストの先頭に挿入する
//! @param head リストの先頭ノードへのポインタ
//! @param node 挿入するノードへのポインタ
//!
void list_insert_head(struct list_node* head, struct list_node* node);

//!
//! ノードをリストの末尾に挿入する
//! @param head リストの先頭ノードへのポインタ
//! @param node 挿入するノードへのポインタ
//!
void list_insert_tail(struct list_node* head, struct list_node* node);

//!
//! ノードをリストから削除する
//! @param node 削除するノードへのポインタ
//!
void list_remove(struct list_node* node);

//!
//! リストが空かどうかを確認する
//! @param head リストの先頭ノードへのポインタ
//! @return 空であれば1、そうでなければ0
//!
int list_is_empty(struct list_node* head);

//!
//! リストの先頭ノードを取得する
//! @param head リストの先頭ノードへのポインタ
//! @return 先頭ノードへのポインタ
//!
struct list_node* list_get_head(struct list_node* head);

//!
//! リストの末尾ノードを取得する
//! @param head リストの先頭ノードへのポインタ
//! @return 末尾ノードへのポインタ
//!
struct list_node* list_get_tail(struct list_node* head);

//!
//! リストの次のノードを取得する
//! @param node 現在のノードへのポインタ
//! @return 次のノードへのポインタ
//!
struct list_node* list_get_next(struct list_node* node);

//!
//! リストの前のノードを取得する
//! @param node 現在のノードへのポインタ
//! @return 前のノードへのポインタ
//!
struct list_node* list_get_prev(struct list_node* node);

//!
//! リスト内のノード数をカウントする
//! @param head リストの先頭ノードへのポインタ
//! @return ノード数
//!
size_t list_count(struct list_node* head);

//!
//! リスト内のノードをすべて削除する
//! @param head リストの先頭ノードへのポインタ
//!
void list_clear(struct list_node* head);

//!
//! リスト内の各ノードに対して関数を適用する
//! @param head リストの先頭ノードへのポインタ
//! @param func 各ノードに適用する関数へのポインタ
//!
void for_each(struct list_node* head, void (*func)(struct list_node*));

