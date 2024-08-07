#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#ifndef STRINGBUILDER_API
    #define STRINGBUILDER_API
#endif

typedef StringView StringView;
typedef struct StringBuilder StringBuilder;

#define string_builder(size_) string_builder_(((char[size_]){}), size_ * sizeof(char))

#define STRLEN(str_) _Generic(&(str_),\
    char(*)[sizeof(str_)]: sizeof(str_) - 1, \
    const char**: strlen(*(char**)(&str_)), \
    char**: strlen(*(char**)(&str_)), \
    const StringView*: ((StringView*)&str_)->length, \
    StringView*: ((StringView*)&str_)->length \
)

#define STRDATA(str_) _Generic(&(str_),\
    char(*)[sizeof(str_)]: str_, \
    const char**: *(const char**)(&str_), \
    char**: *(char**)(&str_), \
    const StringView*: ((const StringView*)&str_)->data, \
    StringView*: ((StringView*)&str_)->data \
)

#define string_builder_append(sb_, str_) string_builder_append_((sb_), STRDATA((str_)), STRLEN((str_)))

STRINGBUILDER_API StringBuilder *string_builder_alloc(size_t capacity);
STRINGBUILDER_API const char* string_builder_cstr(StringBuilder *sb);
STRINGBUILDER_API void string_builder_free(StringBuilder *sb);

#endif //STRINGBUILDER_H

#ifdef STRINGBUILDER_IMPLEMENTATION

const size_t DEFAULT_STRING_BUILDER_CAPACITY = 1024;

struct StringBuilder {
    size_t current_max_size;
    size_t length;
    size_t string_length;
    char *data;
    StringBuilder *previus;
    int heap_buffer:1;
    int heap_self:1;
};

StringBuilder string_builder_(char *buffer, const size_t capacity) {
    StringBuilder sb;
    sb.current_max_size = capacity;
    sb.length = 0;
    sb.string_length = 0;
    sb.data = buffer;
    sb.heap_buffer = false;
    sb.heap_self = false;
    sb.previus = NULL;
    return sb;
}

StringBuilder *string_builder_alloc(const size_t capacity) {
    StringBuilder *sb = malloc(sizeof(StringBuilder));
    sb->current_max_size = capacity;
    sb->length = 0;
    sb->string_length = 0;
    sb->data = malloc(capacity);
    sb->heap_buffer = true;
    sb->heap_self = true;
    sb->previus = NULL;
    return sb;
}

static void string_bulder_swap(StringBuilder *a, StringBuilder *b) {
    const StringBuilder tmp = *a;
    *a = *b;
    *b = tmp;
    b->string_length = a->string_length;
    a->heap_self = tmp.heap_self;
}

void string_builder_append_(StringBuilder *sb, const char *str, const size_t len) {
    if (sb->length + len >= sb->current_max_size) {
        StringBuilder *new_builder = string_builder_alloc(DEFAULT_STRING_BUILDER_CAPACITY);
        string_bulder_swap(sb,new_builder);
        sb->previus = new_builder;
        int i = 0;
    }
    memcpy(sb->data + sb->length, str, len);
    sb->length += len;
    sb->string_length += len;
}

const char* string_builder_cstr(StringBuilder *sb) {
    char *data = malloc(sb->string_length + 1);
    StringBuilder *current = sb;
    while (current != NULL) {
        memcpy(data, current->data, current->length);
        current = current->previus;
    }
    data[sb->string_length] = '\0';
    return data;
}

void string_builder_free(StringBuilder *sb) {
    StringBuilder *current = sb;
    while (current != NULL) {
        StringBuilder *next = current->previus;
        if (current->heap_buffer && current->data != NULL) {
            free(current->data);
        }
        if (current->heap_self) {
            free(current);
        }
        current = next;
    }
}

#endif //STRINGBUILDER_IMPLEMENTATION

