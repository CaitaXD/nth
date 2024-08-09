#ifndef SB_API
#define SB_API

#ifndef SB_API
    #define SB_API
#endif

#ifndef STATEMENT
    #define STATEMENT(X) do { X } while (0)
#endif

#include "Buffer.h"
#include "StringView.h"

typedef struct StringBuilder {
    BufferSegmentHeader *sb_buffer;
} StringBuilder;

#define sb_stackalloc(size_) ((StringBuilder) { (BufferSegmentHeader *)buffer_stackalloc(char, size_) - 1 })
#define sb_alloc(size_) ((StringBuilder) { (BufferSegmentHeader *)buffer_alloc(char, size_) - 1 })

#define sb_append(sb_, str_) STATEMENT( \
    const size_t __str_length = STRLEN((str_)); \
    const char *__str_ptr = STRDATA((str_)); \
    void *__tmp = (sb_).sb_buffer->data; \
    buffer_write(__tmp, __str_ptr, __str_length); \
    (sb_).sb_buffer = buffer_header(__tmp); \
)

SB_API const char* sb_cstr(StringBuilder sb);
SB_API void sb_free(StringBuilder sb);
SB_API void sb_clear(StringBuilder sb);

#endif //SB_API

#ifdef SB_IMPLEMENTATION
#undef SB_IMPLEMENTATION

const size_t DEFAULT_STRING_BUILDER_CAPACITY = 1024;

static StringBuilder reverse(StringBuilder sb) {
    BufferSegmentHeader *current = sb.sb_buffer;
    BufferSegmentHeader *prev = NULL;
    BufferSegmentHeader *tmp = NULL;

    while (current != NULL) {
        tmp = current->prev;
        current->prev = prev;
        prev = current;
        current = tmp;
    }
    sb.sb_buffer = prev;
    return sb;
}

const char* sb_cstr(StringBuilder sb) {
    const size_t length = seglen_sum(sb.sb_buffer->data);
    char *cstr = malloc(length + 1);
    char *cursor = cstr;
    sb = reverse(sb);
    const BufferSegmentHeader *current = sb.sb_buffer;
    while (current != NULL) {
        memcpy(cursor, &current->data[0], current->seglen);
        cursor += current->seglen;
        current = current->prev;
    }
    cstr[length] = '\0';
    reverse(sb);
    return cstr;
}

void sb_free(const StringBuilder sb) {
    BufferSegmentHeader *current = sb.sb_buffer;
    while (current != NULL) {
        BufferSegmentHeader *next = current->prev;
        if (current->heap_allocated) {
            free(current);
        }
        current = next;
    }
}

void sb_clear(const StringBuilder sb) {
    buffer_clear(&sb.sb_buffer->data[0]);
}

#endif //SB_IMPLEMENTATION

