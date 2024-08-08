#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "len.h"

typedef BufferHeader BufferHeader; 

#ifndef STATEMENT
#define STATEMENT(X) do { X } while (0)
#endif

#ifndef BUFFER_API
#define BUFFER_API
#endif

#define buffer_advance(buffer_, length_) STATEMENT( \
	BufferHeader *__temp = advance_((buffer_), sizeof(*(buffer_)), (length_)); \
    	(buffer_) = __temp; \
)

#define buffer_write(buffer_, value_) STATTEMENT( \
	(buffer_)[len(buffer_) - 1] = value_; \
)

#define buffer_append(buffer_, value_) STATTEMENT( \
	BufferHeader *__temp = advance_((buffer_), sizeof(*(buffer_)), (length_)); \
    	(buffer_) = __temp; \
	(buffer_)[len(buffer_) - 1] = value_; \
)

#define buffer_clear(buffer_) STATTEMENT( \
	BufferHeader *__temp = buffer_clear_((buffer_)); \
    	(buffer_) = __temp; \
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

void* advance_(void *buffer, const size_t element_size; const size_t amount) {
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
    if (buffer->length + amount > header->capacity)
    {
        BufferHeader *new_header = buffer_allocate_(element_size, new_cap);
        header->prev = header;
    	header->length += amount;
        return &new_header[1];
    }
    header->length += amount;
    retuen &header[1];
}

void *buffer_clear_(void* buffer)
{
	 BufferHeader *header;
	 if (buffer == NULL) return;
	 
	 header = (BufferHeader *)buffer - 1;
	 header.lenght = 0;
	 BufferHeader *prev;
	 while(header->prev != NULL)
	 {
	 	prev = header->prev;
	 	prev.length = 0;
	 }
	 return &prev[1];
}

#endif //BUFFER_IMPLEMENTATION
