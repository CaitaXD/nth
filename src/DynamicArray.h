#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "len.h"

#ifndef DA_API
    #define DA_API static inline
#endif

#define DA_MALLOC malloc
#define DA_FREE free
#define DA_REALLOC realloc
#define DA_CALLOC calloc

#define DEFAULT_CAPACITY 4

#define DA_CONCAT_INTERNAL(x, y) x##y
#define DA_CONCAT(x, y) DA_CONCAT_INTERNAL(x, y)
#define DA_FN(name) DA_CONCAT(DA_NAMESPACE, name)
#define DA_STATEMENT(X) do { X } while (0)

typedef struct DynamicArrayHeader DynamicArrayHeader;

#endif //DYNAMICARRAY_H

#define da_with_capacity(type_, capacity_) da_new_(sizeof(type_), capacity_)

#define da_add(da_, value_) DA_STATEMENT( \
    void *__temp = da_grow_((da_), sizeof(*(da_))); \
    (da_) = __temp;           \
    (da_)[len(da_) - 1] = value_; \
)

#define da_add_range(da_, ...) DA_STATEMENT( \
    __typeof__(*da_) *__args = ((__typeof__(*da_)[]){__VA_ARGS__}); \
    size_t __size = sizeof(((__typeof__(*da_)[]){__VA_ARGS__})); \
    size_t __count = __size / sizeof(*(da_)); \
    size_t __prev_len = len(da_); \
    void *__temp = da_grow_n_((da_), sizeof(*(da_)), __count); \
    (da_) = __temp; \
    memcpy((da_) + __prev_len, __args, __size); \
)

#define da_insert(da_, index_, value_) DA_STATEMENT( \
    DA_ASSERT_NOTOUTOFBOUNDS((da_), (index_)); \
    void *__temp = da_grow_((da_), sizeof(*(da_))); \
    (da_) = __temp;           \
    memcpy((da_) + index_ + 1, (da_) + index_, sizeof(*(da_)) * (len(da_) - index_)); \
    (da_)[index_] = value_; \
)

DA_API void da_remove_at(void *da, const size_t index);

#ifdef DYNAMICARRAY_IMPLEMENTATION
#define DA_ASSERT_NOTOUTOFBOUNDS(da_, index_) assert("Acess out of bounds" && (index_) < len(da_))

struct DynamicArrayHeader {
    size_t capacity;
    size_t size;
    char data[];
};

DynamicArrayHeader* da_new_ (const size_t element_size, const size_t capacity) {
    DynamicArrayHeader *header = DA_MALLOC(element_size * capacity + sizeof(DynamicArrayHeader));
    memset(header, 0, element_size * capacity + sizeof(DynamicArrayHeader));
    header->capacity = (uint32_t)capacity;
    return header;
}

void* da_grow_ (void *da, const size_t element_size){
    DynamicArrayHeader *header;
    if (da == NULL)
    {
        header = da_new_(element_size, DEFAULT_CAPACITY);
    }
    else
    {
        header = (DynamicArrayHeader *)da - 1;
    }
    if (header->size == header->capacity)
    {
        DynamicArrayHeader *new_array = da_new_(element_size, header->capacity << 1U);
        volatile const size_t copy_size = element_size * header->capacity + sizeof(DynamicArrayHeader);
        memcpy(new_array, header, copy_size);
        header = new_array;
        new_array->capacity = header->capacity << 1U;
    }
    header->size++;
    return &header[1];
}

void* da_grow_n_(void *da, const size_t element_size, const size_t count) {
    DynamicArrayHeader *header;
    if (da == NULL)
    {
        header = da_new_(element_size, DEFAULT_CAPACITY);
    }
    else
    {
        header = (DynamicArrayHeader *)da - 1;
    }
    if (header->size + count > header->capacity)
    {
        DynamicArrayHeader *new_array = da_new_(element_size, header->capacity << 1U);
        volatile const size_t copy_size = element_size * header->capacity + sizeof(DynamicArrayHeader);
        memcpy(new_array, header, copy_size);
        free(header);
        header = new_array;
        new_array->capacity = header->capacity << 1U;
    }
    header->size += count;
    return &header[1];
}

void da_remove_at(void *da, const size_t index) {
    DA_ASSERT_NOTOUTOFBOUNDS(da, index);
    assert(da != NULL);
    const size_t size = len(da);
    void **da_ptr = da;
    for (int j = index + 1; j < size; j++)
    {
        da_ptr[j - 1] = da_ptr[j];
    }
    DynamicArrayHeader *header = da;
    header[-1].size--;
}

#endif //DYNAMICARRAY_IMPLEMENTATION
