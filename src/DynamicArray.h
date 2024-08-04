#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef DA_API
    #define DA_API static inline
#endif

#ifndef NO_STDC
    #include <stdlib.h>
    #include <string.h>
    #include <assert.h>
    #define DA_MEMCPY memcpy
    #define DA_MEMSET memset
    #define DA_ASSERT assert
    #define DA_MEMMOVE memmove
#endif

#ifndef DA_ALLOCATOR
    #define DA_MALLOC malloc
    #define DA_FREE free
    #define DA_REALLOC realloc
    #define DA_CALLOC calloc
#endif


#define DEFAULT_CAPACITY 4

#define DA_CONCAT_INTERNAL(x, y) x##y
#define DA_CONCAT(x, y) DA_CONCAT_INTERNAL(x, y)
#define DA_FN(name) DA_CONCAT(DA_NAMESPACE, name)
#define DA_STATEMENT(X) do { X } while (0)

typedef struct DynamicArrayHeader DynamicArrayHeader;

#endif //DYNAMICARRAY_H

__forceinline size_t da_len(const void *da);

#define da_add(da_, value_) DA_STATEMENT( \
    void *__temp = da_grow_((da_), sizeof(*(da_))); \
    (da_) = __temp;           \
    (da_)[da_len(da_) - 1] = value_; \
)

#define da_add_range(da_, ...) DA_STATEMENT( \
    __typeof__(*da_) *__args = ((__typeof__(*da_)[]){__VA_ARGS__}); \
    size_t __size = sizeof(((__typeof__(*da_)[]){__VA_ARGS__})); \
    size_t __count = __size / sizeof(*(da_)); \
    size_t __prev_len = da_len(da_); \
    void *__temp = da_grow_n_((da_), sizeof(*(da_)), __count); \
    (da_) = __temp; \
    DA_MEMCPY((da_) + __prev_len, __args, __size); \
)

#define da_insert(da_, index_, value_) DA_STATEMENT( \
    DA_ASSERT_NOTOUTOFBOUNDS((da_), (index_)); \
    void *__temp = da_grow_((da_), sizeof(*(da_))); \
    (da_) = __temp;           \
    DA_MEMCPY((da_) + index_ + 1, (da_) + index_, sizeof(*(da_)) * (da_len(da_) - index_)); \
    (da_)[index_] = value_; \
)

DA_API void da_remove_at(void *da, const size_t index);

#ifdef DYNAMICARRAY_IMPLEMENTATION
#define DA_ASSERT_NOTOUTOFBOUNDS(da_, index_) DA_ASSERT("Acess out of bounds" && (index_) < da_len(da_))

struct DynamicArrayHeader {
    size_t size;
    size_t capacity;
    char data[];
};

size_t da_len(const void *da)
{
    if (da == NULL) return 0;
    const DynamicArrayHeader *header = da;
    return header[-1].size;
}

DynamicArrayHeader* da_new_ (const size_t element_size, const size_t capacity) {
    DynamicArrayHeader *header = DA_MALLOC(element_size * capacity + sizeof(DynamicArrayHeader));
    memset(header, 0, element_size * capacity + sizeof(DynamicArrayHeader));
    header->capacity = (uint32_t)capacity;
    return header;
}

void *da_grow_ (void *da, const size_t element_size){
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

void *da_grow_n_(void *da, const size_t element_size, const size_t count) {
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
        header = new_array;
        new_array->capacity = header->capacity << 1U;
    }
    header->size += count;
    return &header[1];
}

void da_remove_at(void *da, const size_t index) {
    DA_ASSERT_NOTOUTOFBOUNDS(da, index);
    DA_ASSERT(da != NULL);
    const size_t size = da_len(da);
    void **da_ptr = da;
    for (int j = index + 1; j < size; j++)
    {
        da_ptr[j - 1] = da_ptr[j];
    }
    DynamicArrayHeader *header = da;
    header[-1].size--;
}

#endif //DYNAMICARRAY_IMPLEMENTATION