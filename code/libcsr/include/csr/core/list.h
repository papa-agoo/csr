////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct list_node;

struct list_node
{
    struct list_node* prev;
    struct list_node* next;

    void* value;
};

struct list
{
    struct list_node* first;
    struct list_node* last;

    u32 num_nodes;
};

// callback for freeing the nodes value ptr
typedef void (list_free_value_cb_t)(void* value_ptr);

////////////////////////////////////////

struct list* list_create();
void list_destroy(struct list* list, list_free_value_cb_t* cb);
void list_clear(struct list* list, list_free_value_cb_t* cb);

struct list_node* list_insert(struct list* list, struct list_node* ref_node);
void* list_remove(struct list* list, struct list_node* node);

void list_remove_by_value(struct list* list, void* value);

void list_push_front(struct list* list, void* value);
void* list_pop_front(struct list* list);

void list_push_back(struct list* list, void* value);
void* list_pop_back(struct list* list);

////////////////////////////////////////

#define list_count(A) ((A)->num_nodes)
#define list_empty(A) ((A)->first == NULL && (A)->last == NULL)

#define list_first(A) ((A)->first)
#define list_last(A) ((A)->last)

#define LIST_FOREACH(L, S, M, V) \
    struct list_node* _node = NULL; struct list_node* V = NULL; \
    for (V = _node = L->S; _node != NULL; V = _node = _node->M)
