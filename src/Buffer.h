#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "len.h"

typedef struct BufferHeader {
	struct BufferHeader *prev;
	bool heap_allocated;
	size_t capacity;
	size_t length;
	char data[];
} BufferHeader;

#ifndef STATEMENT
	#define STATEMENT(X) do { X } while (0)
#endif

#ifndef BUFFER_API
	#define BUFFER_API
#endif

#ifndef CARRAY_LITERAL
	#define CARRAY_LITERAL(type_, size_, ...) (type_[size_]){__VA_ARGS__}
#endif

#define buffer_stackalloc(type_, size_) \
	(struct  { \
		BufferHeader header; \
		char data[size_ * sizeof(type_)]; \
	})\
	{ \
		.header = { .capacity = size_ } \
	}.data

#define buffer_alloc(type_, size_) buffer_allocate_(sizeof(type_), size_)->data

#define buffer_advance(buffer_, length_) STATEMENT( \
	void *__temp = advance_((buffer_), sizeof(*(buffer_)), (length_)); \
    (buffer_) = __temp; \
)

#define buffer_write(buffer_, value_, length_) STATEMENT( \
	memcpy((buffer_) + len(buffer_), (value_), (length_)); \
)

BUFFER_API void buffer_clear(void* buffer);

#endif // BUFFER_H
#ifdef BUFFER_IMPLEMENTATION

const size_t BUFFER_DEFAULT_CAPACITY = 1024;

BufferHeader *buffer_allocate_(const size_t element_size, const size_t capacity) {
    BufferHeader *header = malloc(element_size * capacity + sizeof(BufferHeader));
    header->capacity = (uint32_t)capacity;
    header->heap_allocated = true;
	header->length = 0;
	header->prev = NULL;
    return header;
}

static size_t maxsz(size_t a, size_t b) {
    return a > b ? a : b;
}

void* advance_(void *buffer, const size_t element_size, const size_t amount) {
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
    	BufferHeader *prev = header->prev;
    	if (prev == NULL)
    	{
    		prev = header->prev = buffer_allocate_(element_size, new_cap);
    	}
    	else
    	{
    		BufferHeader *tmp = prev->prev;
    		prev->prev = header;
    		header->prev = tmp;
    	}
	    header->length += amount;
	    return &prev[1];
    }
    header->length += amount;
    return &header[1];
}

void buffer_clear(void* buffer)
{
	if (buffer == NULL) return;

	BufferHeader *node = (BufferHeader *)buffer - 1;
	while(node != NULL)
	{
		node->length = 0;
	 	node = node->prev;
	}
}

#endif //BUFFER_IMPLEMENTATION
