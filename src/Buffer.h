#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef BufferHeader BufferHeader; 

#ifndef STATEMENT
#define STATEMENT(X) do { X } while (0)
#endif

#define buffer_write(buffer_, value_) STATEMENT( \
    void *__temp = advance_((da_), sizeof(*(da_))); \
    (da_) = __temp;           \
    (da_)[len(da_) - 1] = value_; \
)

#endif // BUFFER_H
#ifdef BUFFER_IMPLEMENTATION

const size_t BUFFER_DEFAULT_CAPACITY = 1024;

struct BufferHeader {
    struct BufferHeader prev*;
    bool heap_allocated;
    size_t capacity;
    size_t length;
    char data[];
}

BufferHeader *buffer_allocate_(const size_t element_size, const size_t capacity) {
    BufferHeader *header = malloc(element_size * capacity + sizeof(BufferHeader));
    memset(header, 0, element_size * capacity + sizeof(BufferHeader));
    header->capacity = (uint32_t)capacity;
    header->heap_allocated = true;
    return header;
}

static inline maxsz(size_t a, size_t b) {
    return (a > b) ? a : b;
}

void advance_(void *buffer, const size_t amount) {
    BufferHeader *header;
    const size_t new_cap = maxsz(BUFFER_DEFAULT_CAPACITY, amount);
    if (buffer == NULL)
    {
        header = buffer_allocate_(element_size, new_cap);
    }
    else
    {
        header = (BufferHeader *)buffer - 1;
    }
    if (header->length + amount > header->capacity)
    {
        BufferHeader temp = *header;
        BufferHeader *new_buffer = buffer_allocate_(element_size, new_cap);
        *header = *new_buffer;
        header->prev = temp;
    }
    header->length += amount;
}

#endif //BUFFER_IMPLEMENTATION