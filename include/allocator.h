#ifndef ALLOCATOR_ALLOCATOR_H
#define ALLOCATOR_ALLOCATOR_H


#include <stddef.h>

void mem_init(size_t size);

void *mem_alloc(size_t size);

void *mem_realloc(void *addr, size_t size);

void mem_free(void *addr);

void mem_dump();

#endif //ALLOCATOR_ALLOCATOR_H
