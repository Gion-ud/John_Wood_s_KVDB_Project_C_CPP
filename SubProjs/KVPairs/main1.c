#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ByteArr Key;
typedef struct ByteArr Val;
typedef struct KVpair  KV;

typedef unsigned int  ulong_t;
typedef unsigned char byte_t;

struct ByteArr {
    unsigned char *data;  // must alloc
    unsigned int   len;    // data len
};

struct KVPair {
    Key key;
    Val val;
};

#define MAX_ENTRY_COUNT 32

void insert(KV *kv_arr, byte_t *key, byte_t *val, ulong_t key_len, ulong_t val_len);

int main() {
    KV *kv_arr = (KV*)calloc(MAX_ENTRY_COUNT, sizeof(struct KVPair));
    if (!kv_arr) { perror("calloc failed"); return 1; }




    return 0;
}