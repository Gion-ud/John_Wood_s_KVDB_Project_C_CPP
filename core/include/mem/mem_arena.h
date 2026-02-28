#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include <global.h>
#include <export.h>

#define MEM_SIZE (64 * 1024)
#define MEM_ALIGN_DEFAULT 8

#define MEM_ARENA_API LIB_API

struct mem_arena {
    void       *mem_p;
//  size32_t    mem_size;
    size32_t    mem_align;
    uoff32_t    mem_cur;
    uoff32_t    mem_end;
    size32_t    mem_alloc_cnt;
};

#include <mem/mem_arena.h>

//byte_t mem_pool[MEM_SIZE] = {0};

void mem_arena_init_aligned(
    struct mem_arena   *this,
    void               *buf,
    size32_t            buf_size,
    size32_t            mem_align
);
MEM_ARENA_API void *mem_arena_alloc_aligned(struct mem_arena *this, size32_t size);
MEM_ARENA_API void mem_arena_reset(struct mem_arena *this);
MEM_ARENA_API struct mem_arena *mem_arena_create_aligned(size32_t mem_size, size32_t mem_align);
MEM_ARENA_API void mem_arena_destroy(struct mem_arena *this);


static inline void mem_arena_init(struct mem_arena *this, void *buf, size32_t buf_size) {
    mem_arena_init_aligned(this, buf, buf_size, MEM_ALIGN_DEFAULT);
}
static inline void mem_arena_init_no_alignment(struct mem_arena *this, void *buf, size32_t buf_size) {
    mem_arena_init_aligned(this, buf, buf_size, 1);
}
static inline void *mem_arena_alloc(struct mem_arena *this, size32_t size) {
    return mem_arena_alloc_aligned(this, size);
}
struct mem_arena *mem_arena_create(size32_t mem_size) {
    return mem_arena_create_aligned(mem_size, MEM_ALIGN_DEFAULT);
}
struct mem_arena *mem_arena_create_no_alignment(size32_t mem_size) {
    return mem_arena_create_aligned(mem_size, 1);
}


#endif