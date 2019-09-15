#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/allocator.h"


void *heap_ptr = NULL;
size_t heap_size = 0;
int block_count = 0;

typedef struct mem_block_header {
    size_t prev_block_size;
    size_t curr_block_size;
    bool available;
} mem_block_header;

size_t align(size_t size);

void *find_free_block(size_t size);

void split_block(void *block_addr, size_t split_size);


void mem_init(size_t size) {
    size = align(size);

    heap_ptr = malloc(size + sizeof(struct mem_block_header));
    mem_block_header init_header = {
            .prev_block_size = 0,
            .curr_block_size = size,
            .available = true
    };
    heap_size = size;
    block_count++;

    mem_block_header *heap_header = heap_ptr;

    *heap_header = init_header;
}

void *mem_alloc(size_t size) {
    size = align(size);

    if (size > heap_size) {
        return NULL;
    }

    mem_block_header *block_addr = find_free_block(size);

    if (block_addr == NULL) {
        return block_addr;
    }

    block_addr->available = false;

    split_block(block_addr, size);

    return block_addr;
}

void *mem_realloc(void *addr, size_t size) {
    size = align(size);
    if (addr == NULL) {
        return mem_alloc(size);
    }

    if (size == 0) {
        mem_free(addr);
        return NULL;
    }

    mem_block_header *old_header = (mem_block_header *) addr;

    void *old_addr = addr;
    size_t old_size = old_header->curr_block_size;

    if (old_size == size) {
        return old_addr;
    }

    if (old_size > size) {
        split_block(old_addr, size);
        mem_free(old_addr + ((mem_block_header *) old_addr)->curr_block_size + sizeof(mem_block_header));
        return old_addr;
    }

    mem_block_header *new_block = find_free_block(size);

    if (new_block == NULL) {
        return new_block;
    }

    new_block->available = false;
    split_block(new_block, size);
    memcpy(new_block + sizeof(mem_block_header), old_addr + sizeof(mem_block_header), size);
    mem_free(old_addr);

    return new_block;
}

void mem_free(void *addr) {
    mem_block_header *header = (mem_block_header *) addr;
    if (header->available) {
        return;
    }
    header->available = true;

    void *prev_block_addr = addr - header->prev_block_size - sizeof(mem_block_header);
    mem_block_header *prev_block_header = (mem_block_header *) prev_block_addr;

    void *next_block_addr = addr + header->curr_block_size + sizeof(mem_block_header);
    mem_block_header *next_block_header = (mem_block_header *) next_block_addr;

    if (prev_block_header->available && header->prev_block_size != 0) {
        block_count--;
        prev_block_header->curr_block_size =
                prev_block_header->curr_block_size + header->curr_block_size + sizeof(mem_block_header);
        next_block_header->prev_block_size = prev_block_header->curr_block_size;
    }
}

void mem_dump() {
    printf("====================MEMORY DUMP====================\n");
    void *curr_heap_ptr = heap_ptr;
    for (int i = 0; i < block_count; ++i) {
        mem_block_header *curr_block_header = (mem_block_header *) curr_heap_ptr;
        printf("Block #%d:\n", i + 1);
        printf("Address: %p\n", curr_block_header);
        printf("Size: %zu\n", curr_block_header->curr_block_size);
        printf("Previous block size: %zu\n", curr_block_header->prev_block_size);
        printf("Available: %s\n", curr_block_header->available ? "true" : "false");
        printf("\n");

        curr_heap_ptr = curr_heap_ptr + curr_block_header->curr_block_size + sizeof(mem_block_header);
    }

    printf("Total size: %zu\n", heap_size);
    printf("Total blocks count: %d\n", block_count);
}


size_t align(size_t size) {
    if (size % 4 == 0) {
        return size;
    } else {
        return size + (4 - (size % 4));
    }
}

void *find_free_block(size_t size) {
    void *curr_ptr = heap_ptr;
    for (int i = 0; i < block_count; ++i) {
        mem_block_header *curr_header = (mem_block_header *) curr_ptr;
        if (curr_header->curr_block_size > size + sizeof(mem_block_header) && curr_header->available) {
            return curr_ptr;
        }
        curr_ptr = curr_ptr + sizeof(mem_block_header) + curr_header->curr_block_size;
    }
    return NULL;
}

void split_block(void *block_addr, size_t split_size) {
    void *curr_addr = block_addr;
    mem_block_header *block_header = (mem_block_header *) curr_addr;

    size_t full_size = block_header->curr_block_size;

    block_header->curr_block_size = split_size;
    curr_addr = curr_addr + block_header->curr_block_size + sizeof(mem_block_header);

    mem_block_header second_block_header = {
            .prev_block_size = split_size,
            .curr_block_size = full_size - split_size - sizeof(mem_block_header),
            .available = true
    };
    *(mem_block_header *) curr_addr = second_block_header;

    curr_addr = curr_addr + (*(mem_block_header *) curr_addr).curr_block_size + sizeof(mem_block_header);
    (*(mem_block_header *) curr_addr).prev_block_size = second_block_header.curr_block_size;

    block_count++;
}
