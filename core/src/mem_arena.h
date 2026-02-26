#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include <global.h>

#define MEM_SIZE (64 * 1024)

struct mem_arena {
    void       *mem_p;
//  size32_t    mem_size;
    off32_t     mem_cur;
    off32_t     mem_end;
    size32_t    mem_alloc_cnt;
};

void mem_arena_init(struct mem_arena *this, void *buf);
void *mem_arena_alloc(struct mem_arena *this, size32_t size);
void mem_arena_reset(struct mem_arena *this);


#endif