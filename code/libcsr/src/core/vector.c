////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct vector
{
    size_t capacity;

    size_t element_size;
    size_t element_count;

    // FIXME need some kind of protection from this: free( vector_get(v, 0) )
    void* data;
};

#define _element_offset(vector, index) (vector->element_size * index)
#define _element_ptr(vector, index) (vector->data + _element_offset(vector, index))
#define _element_out_of_bounds(vector, index) (!(index >= 0 && index < vector->capacity))

////////////////////////////////////////

struct vector* vector_create(size_t capacity, size_t element_size)
{
    check_expr(capacity > 0);
    check_expr(element_size > 0);

    struct vector* vector = calloc(1, sizeof(struct vector));
    check_mem(vector);

    vector->data = calloc(capacity, element_size);
    check_mem(vector->data);

    vector->capacity = capacity;
    vector->element_size = element_size;
    vector->element_count = 0;

    return vector;

error:
    if (vector)
        free(vector);

    return NULL;
}

void vector_destroy(struct vector* vector)
{
    check_ptr(vector);

    if (vector->data)
    {
        vector_reset(vector);

        free(vector->data);
    }

    free(vector);

error:
    return;
}

static void _remove_element(struct vector* vector, u32 idx)
{
    memset(_element_ptr(vector, idx), 0, vector->element_size);

    vector->element_count--;
}

void vector_reset(struct vector* vector)
{
    check_ptr(vector);

    memset(vector->data, 0, vector->element_count * vector->element_size);

    vector->element_count = 0;

error:
    return;
}

s32 vector_resize(struct vector* vector, size_t capacity)
{
    check_ptr(vector);
    check_expr(capacity > 0);

    if (capacity == vector->capacity)
        return 0;

    if (capacity > vector->capacity) {
        vector->capacity = capacity;
    }
    else {
        if (capacity < vector->element_count)
            vector->capacity = vector->element_count;
    }

    void* data = realloc(vector->data, vector->capacity * vector->element_size);
    check_mem(data);

    vector->data = data;

    // FIXME init new memory block with zeros
    // memset(vector->data, 0, vector->element_count * vector->element_size);

    return 0; // FIXME

error:
    return -1;
}

void vector_push_back_(struct vector* vector, void* element)
{
    check_ptr(vector);

    if (vector_is_full(vector)) {
        vector_resize(vector, vector->capacity * 2);
    }

    vector_set_(vector, vector->element_count, element);

error:
    return;
}

void vector_pop_back(struct vector* vector)
{
    check_ptr(vector);
    check_expr(!vector_is_empty(vector));

    _remove_element(vector, vector->element_count - 1);

error:
    return;
}

void vector_set_range(struct vector* vector, u32 idx, size_t count, void* elements)
{
    check_ptr(vector);

    check_expr(!_element_out_of_bounds(vector, idx));
    check_expr(!_element_out_of_bounds(vector, idx + count));

    memcpy(_element_ptr(vector, idx), elements, vector->element_size * count);

    vector->element_count += count; // FIXME ??

error:
    return;
}

void vector_set_(struct vector* vector, u32 idx, void* element)
{
    check_ptr(vector);

    check_expr(!_element_out_of_bounds(vector, idx));

    if (idx >= vector->element_count) {
        vector->element_count = idx + 1;
    }

    if (element) {
        memcpy(_element_ptr(vector, idx), element, vector->element_size);
    }
    else {
        _remove_element(vector, idx);
    }

error:
    return;
}

void* vector_get(struct vector* vector, u32 idx)
{
    check_ptr(vector);

    check_expr(!_element_out_of_bounds(vector, idx));

    return _element_ptr(vector, idx);

error:
    return NULL;
}

u32 vector_size(struct vector* vector)
{
    check_ptr(vector);

    return vector->element_count;

error:
    return 0;
}

u32 vector_capacity(struct vector* vector)
{
    check_ptr(vector);

    return vector->capacity;

error:
    return 0;
}

size_t vector_byte_length(struct vector* vector)
{
    check_ptr(vector);

    return vector->element_count * vector->element_size;

error:
    return 0;
}

size_t vector_capacity_byte_length(struct vector* vector)
{
    check_ptr(vector);

    return vector->capacity * vector->element_size;

error:
    return 0;
}

size_t vector_element_byte_length(struct vector* vector)
{
    check_ptr(vector);

    return vector->element_size;

error:
    return 0;
}
