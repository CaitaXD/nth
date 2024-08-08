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
    void *sb_buffer;
} StringBuilder;

#define sb_stackalooc(size_) ((StringBuilder) { buffer_stackalloc(char, size_) })
#define sb_alloc(size_) ((StringBuilder) { buffer_alloc(char, size_) })

#define sb_append(sb_, str_) STATEMENT( \
    const size_t __str_length = STRLEN((str_)); \
    const char *__str_ptr = STRDATA((str_)); \
    void *__sb_buffer = (sb_).sb_buffer; \
    buffer_write(__sb_buffer, __str_ptr, __str_length); \
	buffer_advance(__sb_buffer, __str_length); \
)

SB_API const char* sb_cstr(const StringBuilder sb);
SB_API void sb_free(StringBuilder sb);


#endif //SB_API

#ifdef SB_IMPLEMENTATION
#undef SB_IMPLEMENTATION

const size_t DEFAULT_STRING_BUILDER_CAPACITY = 1024;

const char* sb_cstr(const StringBuilder sb) {
    const BufferHeader *current = (BufferHeader *)sb.sb_buffer - 1;
    const size_t length = current->length;
    char *cstr = malloc(length + 1);
    while (current != NULL) {
        memcpy(cstr, &current->data[0], length);
        current = current->prev;
    }
    cstr[length] = '\0';
    return cstr;
}

void sb_free(const StringBuilder sb) {
    BufferHeader *current = (BufferHeader *)sb.sb_buffer - 1;
    while (current != NULL) {
        BufferHeader *next = current->prev;
        if (current->heap_allocated) {
            free(current);
        }
        current = next;
    }
}

#endif //SB_IMPLEMENTATION

