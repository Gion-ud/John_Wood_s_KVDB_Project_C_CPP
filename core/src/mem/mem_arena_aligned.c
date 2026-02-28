#include <mem/mem_arena.h>

//byte_t mem_pool[MEM_SIZE] = {0};

void mem_arena_init_aligned(
    struct mem_arena   *this,
    void               *buf,
    size32_t            buf_size,
    size32_t            mem_align
) {
    if (!this) return;
    this->mem_p = (byte_t*)buf;
    this->mem_align = mem_align;
    this->mem_cur = 0;
    this->mem_end = buf_size;
    this->mem_alloc_cnt = 0;
}

void *mem_arena_alloc_aligned(struct mem_arena *this, size32_t size) {
    if (!this || !size) return NULL;
    uoff32_t off_aligned = 
        (this->mem_cur + this->mem_align - 1) &~
        (this->mem_align - 1);
    if (off_aligned + size > this->mem_end) return NULL;
    this->mem_cur = off_aligned + size;
    ++this->mem_alloc_cnt;
    return (byte_t*)this->mem_p + off_aligned;
}

void mem_arena_reset(struct mem_arena *this) {
    this->mem_cur = 0;
    this->mem_alloc_cnt = 0;
    memset((byte_t*)this->mem_p, 0, this->mem_end);
}

// mem arena on heap
struct mem_arena *mem_arena_create_aligned(size32_t mem_size, size32_t mem_align) {
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
    mem_arena_init_aligned(
        mem_arena,
        mem_arena->mem_p,
        mem_size,
        mem_align
    );
    return mem_arena;
}

struct mem_arena *mem_arena_heap_resize(struct mem_arena *this, size32_t new_size) {
    if (!this) return NULL;
    void *mem_tmp = realloc(this->mem_p, new_size);
    if (!mem_tmp) {
        printerrf("realloc failed");
        return NULL;
    }
    this->mem_p = mem_tmp;
    this->mem_end = new_size;
    return this;
}

void mem_arena_destroy(struct mem_arena *this) {
    if (!this) return;
    if (this->mem_p) free(this->mem_p);
    free(this);
    return;
}