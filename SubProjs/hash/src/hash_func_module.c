#include "hash_func_module.h"

hash_t fnv_1a_hash(const ubyte_t* key, ulong_t size) {
#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME        1099511628211ULL
    hash_t h = FNV_OFFSET_BASIS;
    ulong_t i = 0;
    while (i < size) {
        h ^= (ubyte_t)key[i];
        h *= FNV_PRIME;
        i++;
    }
    return h;
#undef FNV_OFFSET_BASIS
#undef FNV_PRIME
}