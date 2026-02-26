#include "mem_arena.h"

//byte_t mem_pool[MEM_SIZE] = {0};

void mem_arena_init(struct mem_arena *this, void *buf) {
    if (!this) return;
    this->mem_p = (byte_t*)buf;
    this->mem_cur = 0;
    this->mem_end = MEM_SIZE;
    this->mem_alloc_cnt = 0;
}

void *mem_arena_alloc(struct mem_arena *this, size32_t size) {
    if (!this || !size ||
        (unsigned)this->mem_cur + size > (unsigned)this->mem_end
    )
        return NULL;
    off32_t off = this->mem_cur;
    this->mem_cur += size;
    ++this->mem_alloc_cnt;
    return (byte_t*)this->mem_p + off;
}

void mem_arena_reset(struct mem_arena *this) {
    this->mem_cur = 0;
    this->mem_alloc_cnt = 0;
    memset((byte_t*)this->mem_p, 0, this->mem_end);
}


struct mem_arena *mem_arena_new(size32_t mem_size) {
    struct mem_arena *mem_arena = (struct mem_arena*)malloc(sizeof(struct mem_arena));
    if (!mem_arena) {
        printerrf("malloc failed");
        return NULL;
    }
    mem_arena->mem_p = (byte_t*)malloc(mem_size);
    if (!mem_arena->mem_p) {
        printerrf("malloc failed");
        free(mem_arena);
        return NULL;
    }
    mem_arena_init(mem_arena, mem_arena->mem_p);
    return mem_arena;
}

void mem_arena_delete(struct mem_arena *this) {
    if (!this) return;
    if (this->mem_p) free(this->mem_p);
    free(this);
    return;
}