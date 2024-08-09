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

#define buffer_header(buffer_) ((BufferSegmentHeader *)buffer_ - 1)

#define buffer_write(buffer_, value_, bytes_) STATEMENT( \
	__tmp = buffer_write_((buffer_), (value_), (bytes_)); \
	(buffer_) = __tmp; \
)

BUFFER_API void buffer_clear(void* buffer);
BUFFER_API void buffer_free(void *buffer);
BUFFER_API size_t seglen_sum(const void *buffer);

#endif // BUFFER_H
#ifdef BUFFER_IMPLEMENTATION

#define BUFFER_DEFAULT_CAPACITY 1024

BufferSegmentHeader *buffer_allocate_(const size_t element_size, const size_t capacity) {
    BufferSegmentHeader *header = malloc(element_size * capacity + sizeof(BufferSegmentHeader));
    header->capacity = (uint32_t)capacity;
    header->heap_allocated = true;
	header->seglen = 0;
	header->prev = NULL;
    return header;
}

static size_t maxsz(const size_t a, const size_t b) { return a > b ? a : b; }
static size_t minsz(const size_t a, const size_t b) { return a < b ? a : b; }
static size_t minssz(const ssize_t a, const ssize_t b) { return a < b ? a : b; }


void *buffer_write_(void *buffer, const void *value, const size_t bytes) {
	BufferSegmentHeader *segment = buffer_header(buffer);
	size_t written = 0;
	while (written < bytes) {
		const ssize_t remaining_bytes = (ssize_t)bytes - (ssize_t)written;
		const ssize_t to_write = minssz(remaining_bytes, (ssize_t)segment->capacity - (ssize_t)segment->seglen);
		if (to_write > 0) {
			memcpy(segment->data + segment->seglen, value + written, to_write);
			written += to_write;
			segment->seglen += to_write;
		}
		else {
			if (segment->prev == NULL) {
				BufferSegmentHeader* newseg = buffer_allocate_(sizeof(char), BUFFER_DEFAULT_CAPACITY);
				newseg->prev = segment;
				segment = newseg;
			}
		}
	}
	return segment->data;
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
