#include "list.h"

void list_init(struct list_node* head) 
{
    head->prev = head;
    head->next = head;
}

void list_insert_head(struct list_node* head, struct list_node* node) 
{
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void list_insert_tail(struct list_node* head, struct list_node* node) 
{
    node->prev = head->prev;
    node->next = head;
    head->prev->next = node;
    head->prev = node;
}

void list_remove(struct list_node* node) 
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

bool list_is_empty(struct list_node* head) 
{
    return head->next == head;
}

struct list_node* list_get_head(struct list_node* head) 
{
    return head->next;
}

struct list_node* list_get_tail(struct list_node* head) 
{
    return head->prev;
}

struct list_node* list_get_next(struct list_node* node) 
{
    return node->next;
}

struct list_node* list_get_prev(struct list_node* node) 
{
    return node->prev;
}

size_t list_count(struct list_node* head) 
{
    size_t count = 0;
    struct list_node* current = head->next;
    while (current != head) 
    {
        count++;
        current = current->next;
    }
    return count;
}

void list_clear(struct list_node* head) 
{
    struct list_node* current = head->next;
    while (current != head) 
    {
        struct list_node* next = current->next;
        list_remove(current);
        current = next;
    }
}

void for_each(struct list_node* head, void (*func)(struct list_node*)) 
{
    struct list_node* current = head->next;
    while (current != head) 
    {
        struct list_node* next = current->next; // 次のノードを保存
        func(current);
        current = next;
    }
}