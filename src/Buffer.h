#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "len.h"

typedef struct BufferSegmentHeader {
	struct BufferSegmentHeader *prev;
	bool heap_allocated;
	size_t capacity;
	size_t seglen;
	char data[];
} BufferSegmentHeader;

#ifndef STATEMENT
	#define STATEMENT(X) do { X } while (0)
#endif

#ifndef BUFFER_API
	#define BUFFER_API
#endif

#ifndef CARRAY_LITERAL
	#define CARRAY_LITERAL(type_, len_, ...) (type_[len_]){__VA_ARGS__}
#endif

#define buffer_stackalloc(type_, len_) \
	(struct  { \
		BufferSegmentHeader header; \
		char data[len_ * sizeof(type_)]; \
	})\
	{ \
		.header = { .capacity = len_ } \
	}.data

#define buffer_alloc(type_, len_) buffer_allocate_(sizeof(type_), len_)->data

#define buffer_advance(buffer_, seglen_) STATEMENT( \
	BufferSegmentHeader *__temp = advance_((buffer_), sizeof(*(buffer_)), (seglen_)); \
    (buffer_) = __temp; \
)

#define buffer_write(buffer_, value_, bytes_) STATEMENT( \
	memcpy((buffer_) + len(buffer_), (value_), (bytes_)); \
)

BUFFER_API void buffer_clear(void* buffer);
BUFFER_API void buffer_free(void *buffer);
BUFFER_API size_t seglen_sum(const void *buffer);

#endif // BUFFER_H
#ifdef BUFFER_IMPLEMENTATION

const size_t BUFFER_DEFAULT_CAPACITY = 1024;

BufferSegmentHeader *buffer_allocate_(const size_t element_size, const size_t capacity) {
    BufferSegmentHeader *header = malloc(element_size * capacity + sizeof(BufferSegmentHeader));
    header->capacity = (uint32_t)capacity;
    header->heap_allocated = true;
	header->seglen = 0;
	header->prev = NULL;
    return header;
}

static size_t maxsz(size_t a, size_t b) {
    return a > b ? a : b;
}

BufferSegmentHeader* advance_(BufferSegmentHeader *buffer, const size_t element_size, const ssize_t amount) {
    BufferSegmentHeader *header;
    const size_t new_cap = maxsz(BUFFER_DEFAULT_CAPACITY, amount);
	if (buffer == NULL)
    {
        header = buffer_allocate_(element_size, new_cap);
    }
    else 
    {
    	header = buffer;
    }
    if (header->seglen + amount > header->capacity)
    {
    	BufferSegmentHeader *prev = header->prev;
    	if (prev == NULL)
    	{
    		prev = buffer_allocate_(element_size, new_cap);
    		prev->prev = header;
    	}
    	else
    	{
    		BufferSegmentHeader *tmp = prev->prev;
    		prev->prev = header;
    		header->prev = tmp;
    	}
    	prev->seglen = (ssize_t)prev->seglen + amount;
	    return prev;
    }
    header->seglen = (ssize_t)header->seglen + amount;
    return header;
}

void buffer_clear(void* buffer)
{
	if (buffer == NULL) return;

	BufferSegmentHeader *node = (BufferSegmentHeader *)buffer - 1;
	while(node != NULL)
	{
		node->seglen = 0;
	 	node = node->prev;
	}
}

void buffer_free(void *buffer) {
	if (buffer == NULL) return;

	BufferSegmentHeader *node = (BufferSegmentHeader *)buffer - 1;
	while(node != NULL)
	{
		if (node->heap_allocated) {
			free(node);
		}
		node = node->prev;
	}
}

size_t seglen_sum(const void *buffer) {
	if (buffer == NULL) return 0;

	size_t total = 0;
	const BufferSegmentHeader *node = (BufferSegmentHeader *)buffer - 1;
	while(node != NULL)
	{
		total += node->seglen;
		node = node->prev;
	}
	return total;
}
#endif //BUFFER_IMPLEMENTATION
