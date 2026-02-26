#ifndef MEM_H
#define MEM_H

#include "mem_arena.h"

//static byte_t mem_pool[MEM_SIZE] = {0};

// init mem_arena
static struct mem_arena mem_arena = {
    .mem_p = NULL,
    .mem_cur = 0,
    .mem_end = MEM_SIZE,
    .mem_alloc_cnt = 0,
};

static inline void mem_init(void *mem) {
    mem_arena_init(&mem_arena, mem);
}

static inline void *mem_alloc(size32_t size) {
    return mem_arena_alloc(&mem_arena, size);
}

static inline void mem_reset() {
    mem_arena_reset(&mem_arena);
}

#endif