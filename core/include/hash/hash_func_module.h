#pragma once
#include "global.h"

typedef unsigned long long  hash_t;
typedef signed int          hidx_t;

hash_t fnv_1a_hash(const byte_t* key, ulong_t size);

