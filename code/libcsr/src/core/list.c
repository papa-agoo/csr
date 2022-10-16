////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/list.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct list* list_create()
{
    return calloc(1, sizeof(struct list));
}

void list_destroy(struct list* list, list_free_value_cb_t* free_value_cb)
{
    check_ptr(list);

    list_clear(list, free_value_cb);

    free(list);

error:
    return;
}

void list_clear(struct list* list, list_free_value_cb_t* free_value_cb)
{
    check_ptr(list);

    while (!list_empty(list))
    {
        void* value = list_pop_back(list);

        if (free_value_cb) {
            free_value_cb(value);
        }
    }

error:
    return;
}

struct list_node* list_insert(struct list* list, struct list_node* ref_node)
{
    check_ptr(list);

    struct list_node* node = calloc(1, sizeof(struct list_node));
    check_mem(node);

    if (list_empty(list))
    {
        // list is empty
        list->first = node;
        list->last = node;
    }
    else if(ref_node == NULL)
    {
        // insert at the beginning
        node->next = list->first;
        node->next->prev = node;

        list->first = node;
    }
    else
    {
        // insert after ref_node
        node->prev = ref_node;
        node->next = ref_node->next;

        ref_node->next = node;

        if (ref_node == list->last) {
            list->last = node;
        }
    }

    list->num_nodes++;

    return node;

error:
    return NULL;
}

void* list_remove(struct list* list, struct list_node* node)
{
    check_ptr(list);
    check_ptr(node);

    check(!list_empty(list), "list cannot be empty");

    ////////////////////////////////////////

    if (node == list->first && node == list->last)
    {
        // remove last node, list is empty now
        list->first = NULL;
        list->last = NULL;
    }
    else if (node == list->first)
    {
        check(node->next != NULL, "node.next cannot be NULL");

        // remove node from the head
        list->first = node->next;
        list->first->prev = NULL;
    }
    else if (node == list->last)
    {
        check(node->prev != NULL, "node.prev cannot be NULL");

        // remove node from the tail
        list->last = node->prev;
        list->last->next = NULL;
    }
    else
    {
        // remove node from the middle
        struct list_node* left = node->prev;
        struct list_node* right = node->next;

        left->next = right;
        right->prev = left;
    }

    ////////////////////////////////////////

    void *value = node->value;

    list->num_nodes--;

    free(node);

    return value;

error:
    return NULL;
}

void list_remove_by_value(struct list* list, void* value)
{
    check_ptr(list);
    check_ptr(value);

    LIST_FOREACH(list, first, next, current)
    {
        if (current->value == value)
        {
            list_remove(list, current);

            return;
        }
    }

error:
    return;
}

void list_push_front(struct list* list, void* value)
{
    struct list_node* node = list_insert(list, NULL);
    check_ptr(node);

    node->value = value;

error:
    return;
}

void* list_pop_front(struct list* list)
{
    return list_remove(list, list->first);
}

void list_push_back(struct list* list, void* value)
{
    struct list_node* node = list_insert(list, list->last);
    check_ptr(node);

    node->value = value;

error:
    return;
}

void* list_pop_back(struct list* list)
{
    return list_remove(list, list->last);
}
