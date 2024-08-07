#include <stdio.h>
#define DYNAMICARRAY_IMPLEMENTATION
#include "DynamicArray.h"
#define ARRAY_IMPLEMENTATION
#include "Array.h"
#define STRINGVIEW_IMPLEMENTATION
#include "StringView.h"
#define STRINGBUILDER_IMPLEMENTATION
#include "StringBuilder.h"

#define ASSERT_TRUE(actual) if (!(actual)) { printf("Assertion failed at line %d", __LINE__); return 1; }
#define ASSERT_EQ(actual, expected, fmt) if ((actual) != (expected)) { printf("Assertion failed at line %d Expected "fmt", got "fmt, __LINE__, expected, actual); return 1; }
#define ASSERT_STR_EQ(actual, expected) if (strcmp(actual, expected) != 0) { printf("Assertion failed at line %d Expected %s, got %s", __LINE__, expected, actual); return 1; }

int main(void) {
    char **strings = {};
    da_add_range(strings, ", ", "World");
    da_add(strings, "!");
    da_insert(strings, 0, "Hello");

    ASSERT_STR_EQ(strings[0], "Hello");
    ASSERT_STR_EQ(strings[1], ", ");
    ASSERT_STR_EQ(strings[2], "World");
    ASSERT_STR_EQ(strings[3], "!");
    ASSERT_EQ(len(strings), 4UL, "%zu");

    da_remove_at(strings, 1);
    ASSERT_STR_EQ(strings[0], "Hello");
    ASSERT_STR_EQ(strings[1], "World");
    ASSERT_STR_EQ(strings[2], "!");
    ASSERT_EQ(len(strings), 3UL, "%zu");

    const char *person = "John Doe";
    char *stack_person = array_stackalloc(char, 32);
    char *heap_person = array_alloc(char, 32);
    strcpy(stack_person, person);
    strcpy(heap_person, person);
    ASSERT_STR_EQ(stack_person, "John Doe");
    ASSERT_EQ(len(stack_person), 32UL, "%zu");

    ASSERT_STR_EQ(heap_person, "John Doe");
    ASSERT_EQ(len(heap_person), 32UL, "%zu");

    const StringView first_name = string_view(person, 4);
    ASSERT_TRUE(string_view_starts_with(string_view_literal("John Doe"), first_name));

    const StringView last_name = string_view(person + 5, 3);
    ASSERT_TRUE(string_view_ends_with(string_view_literal("John Doe"), last_name));

    ASSERT_EQ(string_view_index_of(string_view_literal("Hello Hello"), string_view_literal("Hello")), 0, "%d");
    ASSERT_EQ(string_view_last_index_of(string_view_literal("Hello Hello"), string_view_literal("Hello")), 6, "%d");
    ASSERT_EQ(string_view_token(string_view_literal("Hello;-Hello"), ";-").length, 5UL, "%zu");

    StringView space_view = string_view_literal(" ");
    const char* bang_cstr = "!";

    StringBuilder sb = string_builder(1024);

    string_builder_append(&sb, "Hello");
    string_builder_append(&sb, string_view_literal(","));
    string_builder_append(&sb, space_view);
    string_builder_append(&sb, "World");
    string_builder_append(&sb, bang_cstr);

    const char* collected = string_builder_cstr(&sb);
    string_builder_free(&sb);

    ASSERT_STR_EQ(collected, "Hello, World!");

    return 0;
}
