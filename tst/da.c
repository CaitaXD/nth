#include <stdio.h>
#define DYNAMICARRAY_IMPLEMENTATION
#include "DynamicArray.h"

#define ASSERT_EQ(actual, expected) if ((actual) != (expected)) { printf("Assertion failed at line %d: Expected %s, got %s\n", __LINE__, expected, actual); return 1; }

int main(void) {
    char **strings = {};
    da_add_range(strings, ", ", "World");
    da_add(strings, "!");
    da_insert(strings, 0, "Hello");

    ASSERT_EQ(strings[0], "Hello");
    ASSERT_EQ(strings[1], ", ");
    ASSERT_EQ(strings[2], "World");
    ASSERT_EQ(strings[3], "!");
    ASSERT_EQ(da_len(strings), 4);

    for (size_t i = 0; i < da_len(strings); i++) {
        printf("%s\n", strings[i]);
    }

    return 0;
}
