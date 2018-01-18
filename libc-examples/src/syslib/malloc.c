#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

/*
	From: 
			https://danluu.com/malloc-tutorial/

 */

void *global_base = NULL;

struct block_meta *get_block_ptr(void *ptr) {
    return (struct block_meta*)ptr - 1;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
    struct block_meta *block;
	void *request = NULL;
    block = sbrk(0);
    printf("block = 0x%04x\n", block);
   // printf("META_SIZE = 0x%04x\n", META_SIZE);

    printf("calling sbrk(%u)\n", size+ META_SIZE);
    request = sbrk(size + META_SIZE);

    printf("request = 0x%04x\n", request);

    //assert((void*)block == request); // Not thread safe.
    if (request == (void*) -1) {
        printf("sbrk() failed\n");
        return NULL; // sbrk failed.
    }

    if (last) { // NULL on first request.
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = (uint32_t) 0x12345678;
    printf("magic set, returning block\n");
    return block;
}

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
    struct block_meta *current = global_base;
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

void *_z80malloc(size_t size) {
    struct block_meta *block;
    // TODO: align size?
    //
    printf("malloc(%u)\n", size);

    if (size <= 0) {
        printf("size too small!\n");
        return NULL;
    }

    if (!global_base) { // First call.
        printf("first call\n");
        block = request_space(NULL, size);
        if (!block) {
            printf("no block from request_space()\n");
            exit(1);
            return NULL;
        }
        printf("1. global_base = 0x%04x\n", global_base);
        global_base = block;
        printf("2. global_base = 0x%04x\n", global_base);
    } else {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block) { // Failed to find free block.
            block = request_space(last, size);
            if (!block) {
                printf("runned out of space? no block!\n"); 
                return NULL;
            }
        } else {      // Found free block
            // TODO: consider splitting block here.
            printf("found free block\n"); 
            block->free = 0;
            block->magic = 0x77777777;
        }
    }

    printf("returning block + 1\n"); 
    return(block+1);
}

void _z80free(void *ptr) {
		struct block_meta* block_ptr = NULL;
    if (!ptr) {
        return;
    }

    // TODO: consider merging blocks once splitting blocks is implemented.
    block_ptr = get_block_ptr(ptr);
   // assert(block_ptr->free == 0);
   // assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x55555555;
}

void *_z80realloc(void *ptr, size_t size) {
    void *new_ptr = NULL;
    struct block_meta* block_ptr = NULL;
    if (!ptr) {
        // NULL ptr. realloc should act like malloc.
        return malloc(size);
    }

    block_ptr = get_block_ptr(ptr);
    if (block_ptr->size >= size) {
        // We have enough space. Could free some once we implement split.
        return ptr;
    }

    // Need to really realloc. Malloc new space and free old space.
    // Then copy old data to new space.
    new_ptr = malloc(size);
    if (!new_ptr) {
        return NULL; // TODO: set errno on failure.
    }
    memcpy(new_ptr, ptr, block_ptr->size);
    free(ptr);
    return new_ptr;
}

void *_z80calloc(size_t nelem, size_t elsize) {
    size_t size = nelem * elsize; // TODO: check for overflow.
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}
