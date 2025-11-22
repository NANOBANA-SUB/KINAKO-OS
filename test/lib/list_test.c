#include "list.h"

// 双方向リストのテストコード

bool test_list() 
{
    struct list_node head;
    struct list_node node1, node2, node3;

    list_init(&head);

    if (!list_is_empty(&head)) return false;

    list_insert_head(&head, &node1);
    if (list_is_empty(&head)) return false;
    if (list_get_head(&head) != &node1) return false;

    list_insert_tail(&head, &node2);
    if (list_get_tail(&head) != &node2) return false;

    list_insert_head(&head, &node3);
    if (list_get_head(&head) != &node3) return false;

    if (list_count(&head) != 3) return false;

    list_remove(&node1);
    if (list_count(&head) != 2) return false;

    list_remove(&node2);
    list_remove(&node3);
    if (!list_is_empty(&head)) return false;

    return true;
}