#ifndef LEN_H
#define LEN_H

__forceinline size_t len(const void *collection);

#endif //LEN_H
#ifdef LEN_IMPLEMENTATION

struct len_header {
    size_t size;
    char data[];
};

size_t len(const void *collection)
{
    if (collection == NULL) return 0;
    const struct len_header *header = collection;
    return header[-1].size;
}
#endif //LEN_IMPLEMENTATION
