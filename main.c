#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "include/allocator.h"

int main() {
    mem_init(3000);
    void *first_addr = mem_alloc(150);
    void *second_addr = mem_alloc(200);
    void *third_addr = mem_alloc(400);
    mem_dump();
    mem_free(first_addr);
    printf("\nFree first address:\n");
    mem_dump();
    second_addr = mem_realloc(second_addr, 150);
    mem_dump();
    mem_realloc(second_addr, 500);
    mem_dump();
    mem_free(third_addr);
    mem_dump();
}