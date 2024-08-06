#ifndef ARRAY_H
#define ARRAY_H

#ifndef ARRAY_API
    #define ARRAY_API
#endif

#define ARRAY_MALLOC malloc
#define ARRAY_FREE free
#define ARRAY_REALLOC realloc
#define ARRAY_CALLOC calloc

typedef struct ArrayHeader ArrayHeader;

#define CARRAY_LITERAL(type_, size_, ...) (type_[size_]){__VA_ARGS__}
#define array_stackalloc(type_, size_) ((ArrayHeader*)CARRAY_LITERAL(size_t, size_/sizeof(size_t) + sizeof(ArrayHeader)/sizeof(size_t), size_))->data
#define array_alloc(type_, size_) array_alloc_(sizeof(type_), size_)

#endif //ARRAY_H
#ifdef ARRAY_IMPLEMENTATION
#define ARRAY_ASSERT_NOTOUTOFBOUNDS(arr_, index_) assert("Acess out of bounds" && (index_) < len(arr_));

struct ArrayHeader {
    size_t size;
    char data[];
};

void *array_alloc_(const size_t element_size, const size_t length) {
    ArrayHeader *header = ARRAY_MALLOC(element_size * length + sizeof(ArrayHeader));
    memset(header, 0, element_size * length + sizeof(ArrayHeader));
    header->size = length;
    return &header[1];
}

#define arr_header_stack(type_, size_)((ArrayHeader){.size = size_, .data = (type_[size_]){} })

#endif //ARRAY_IMPLEMENTATION