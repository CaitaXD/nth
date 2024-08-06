#ifndef STRINGVIEW_H
#define STRINGVIEW_H

#ifndef STRINGVIEW_API
    #define STRINGVIEW_API
#endif
#include <stdbool.h>
#include <stddef.h>

typedef struct StringView StringView;

#define string_view_literal(data_) (StringView) {.data = data_, .length = sizeof(data_) - 1}
STRINGVIEW_API StringView string_view(const char *data, size_t length);
STRINGVIEW_API bool string_view_equals(StringView a, StringView b);
STRINGVIEW_API int string_view_compare(StringView a, StringView b);
STRINGVIEW_API bool string_view_starts_with(StringView a, StringView b);
STRINGVIEW_API bool string_view_ends_with(StringView a, StringView b);

#endif //STRINGVIEW_H
#ifdef STRINGVIEW_IMPLEMENTATION

struct StringView {
    const char *data;
    size_t length;
};

StringView string_view(const char *data, size_t length) {
    return (StringView){.data = data, .length = length};
}

bool string_view_equals(StringView a, StringView b) {
    return a.length == b.length && memcmp(a.data, b.data, a.length) == 0;
}

int string_view_compare(StringView a, StringView b) {
    const size_t min_length = a.length < b.length ? a.length : b.length;
    return memcmp(a.data, b.data, min_length);
}

bool string_view_starts_with(StringView a, StringView b) {
    if (a.length < b.length) {
        return false;
    }
    const StringView prefix = string_view(a.data, b.length);
    return string_view_equals(prefix, b);
}

bool string_view_ends_with(StringView a, StringView b) {
    if (a.length < b.length) {
        return false;
    }
    const StringView suffix = string_view(a.data + a.length - b.length, b.length);
    return string_view_equals(suffix, b);
}

#endif //STRINGVIEW_IMPLEMENTATION