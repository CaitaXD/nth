#ifndef STRINGVIEW_H
#define STRINGVIEW_H

#ifndef STRINGVIEW_API
    #define STRINGVIEW_API
#endif

#include <stdbool.h>
#include <stddef.h>
#include "len.h"

#ifndef STRLEN
    #define STRLEN(str_) _Generic(&(str_),\
        char(*)[sizeof(str_)]: sizeof(str_) - 1, \
        const char**: strlen(*(char**)(&str_)), \
        char**: strlen(*(char**)(&str_)), \
        const StringView*: ((StringView*)&str_)->length, \
        StringView*: ((StringView*)&str_)->length \
    )
#endif

#ifndef STRDATA
    #define STRDATA(str_) _Generic(&(str_),\
        char(*)[sizeof(str_)]: str_, \
        const char**: *(const char**)(&str_), \
        char**: *(char**)(&str_), \
        const StringView*: ((const StringView*)&str_)->data, \
        StringView*: ((StringView*)&str_)->data \
    )
#endif

typedef struct StringView {
    const char *data;
    size_t length;
} StringView;

#define string_view_literal(data_) (StringView) {.data = data_, .length = sizeof(data_) - 1 }
#define string_view_format "%.*s"
#define string_view_args(sv_) (sv_).length, (sv_).data
#define string_view_unpack(sv_) (sv_).data, (sv_).length

STRINGVIEW_API StringView string_view(const char *data, size_t length);
STRINGVIEW_API bool string_view_equals(StringView a, StringView b);
STRINGVIEW_API int string_view_compare(StringView a, StringView b);
STRINGVIEW_API bool string_view_starts_with(StringView haystack, StringView needle);
STRINGVIEW_API bool string_view_ends_with(StringView haystack, StringView needle);
STRINGVIEW_API int string_view_index_of(StringView haystack, StringView needle);
STRINGVIEW_API int string_view_last_index_of(StringView haystack, StringView needle);
STRINGVIEW_API StringView string_view_token(StringView str, const char* delimeter);

#endif //STRINGVIEW_H
#ifdef STRINGVIEW_IMPLEMENTATION
#undef STRINGVIEW_IMPLEMENTATION

StringView string_view(const char *data, const size_t length) {
    return (StringView){.data = data, .length = length};
}

bool string_view_equals(const StringView a, const StringView b) {
    return a.length == b.length && memcmp(a.data, b.data, a.length) == 0;
}

int string_view_compare(const StringView a, const StringView b) {
    const size_t min_length = a.length < b.length ? a.length : b.length;
    return memcmp(a.data, b.data, min_length);
}

bool string_view_starts_with(const StringView haystack, const StringView needle) {
    if (haystack.length < needle.length) {
        return false;
    }
    const StringView prefix = string_view(haystack.data, needle.length);
    return string_view_equals(prefix, needle);
}

bool string_view_ends_with(const StringView haystack, const StringView needle) {
    if (haystack.length < needle.length) {
        return false;
    }
    const StringView suffix = string_view(haystack.data + haystack.length - needle.length, needle.length);
    return string_view_equals(suffix, needle);
}

int string_view_index_of(const StringView haystack, const StringView needle) {
    const char *p = strstr(haystack.data, needle.data);
    if (p == NULL) {
        return -1;
    }
    return p - haystack.data;
}

int string_view_last_index_of(const StringView haystack, const StringView needle) {
    const char* haystack_end = haystack.data + haystack.length - needle.length;
    for(const char *p = haystack_end; p >= haystack.data; --p)
    {
        for(size_t i = 0; i < needle.length; ++i) {
            if(p[i] != needle.data[i])
                goto next;
        }
        return p - haystack.data;

        next:;
    }
    return 0;
}

StringView string_view_token(const StringView str, const char* delimeter) {
    const int index = string_view_index_of(str, string_view_literal(delimeter));
    if (index == -1) {
        return (StringView){};
    }
    return string_view(str.data, index);
}

#endif //STRINGVIEW_IMPLEMENTATION