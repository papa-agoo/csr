////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct vector;

struct vector* vector_create(size_t capacity, size_t element_size); // FIXME fixed size vector
void vector_destroy(struct vector* vector);

void vector_clear(struct vector* vector);
void vector_reset(struct vector* vector);
s32 vector_resize(struct vector* vector, size_t capacity);

void vector_push_back_(struct vector* vector, void* element);
void vector_pop_back(struct vector* vector);

void vector_set_(struct vector* vector, u32 idx, void* element);
void* vector_get(struct vector* vector, u32 idx);

void vector_set_range(struct vector* vector, u32 idx, size_t count, void* elements);

u32 vector_size(struct vector* vector);
u32 vector_capacity(struct vector* vector);

// FIXME better names
size_t vector_byte_length(struct vector* vector);
size_t vector_capacity_byte_length(struct vector* vector);
size_t vector_element_byte_length(struct vector* vector);

////////////////////////////////////////

#define vector_push_back(vector, element) (vector_push_back_(vector, &(element)))

#define vector_set(vector, idx, element) (vector_set_(vector, idx, &(element)))

#define vector_is_empty(vector) (vector_size(vector) == 0)
#define vector_is_full(vector) (vector_size(vector) == vector_capacity(vector))

#define vector_first(vector) (vector_get(vector, 0))
#define vector_last(vector) (vector_get(vector, vector_size(vector) - 1))
#define vector_data(vector) (vector_first(vector))
