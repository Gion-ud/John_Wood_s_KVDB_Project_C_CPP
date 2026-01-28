#include "hash_table_lib.h"
#include <dirent.h>

const char* filenames[] = {
    "res/images/121200433_p0.jpg",
    "res/images/121200433_p1.jpg",
    "res/images/121200433_p2.jpg",
    "res/images/126711989_p0.jpg",
    "res/images/126711989_p1.jpg",
    "res/images/126711989_p2.jpg",
    "res/images/14-most-iconic-anime-catgirls-a.png",
    "res/images/578971.jpg",
    "res/images/856a0eeb55eb3c5d26c1aa5918ea288d_high.webp",
    "res/images/Ange du Blanc Pur Bg.jpg",
    "res/images/Aoyama_Kotaha.png",
    "res/images/Auckland-Itinerary.jpg.optimal.jpg",
    "res/images/bb.jpg",
    "res/images/bg-blurred.png",
    "res/images/bg.jpg",
    "res/images/bg1.jpg",
    "res/images/d1nxnbk-483bc74d-4554-4365-b9b9-91a755040e09.png",
    "res/images/desktop-wallpaper-anime-girl-sleeping-ultra-backgrounds-for-u-tv-widescreen-ultrawide-laptop-tablet-smartphone.jpg",
    "res/images/Google_translate.png",
    "res/images/Google_translate1.png",
    "res/images/Hataraku_Saibou.jpeg",
    "res/images/image.png"
};

#define ENTRY_COUNT sizeof(filenames) / sizeof(char*)
#define MAX_ENTRY_COUNT (ENTRY_COUNT + 31) & ~31

struct Val {
    ulong_t val_len;
    byte_t *val_data;
};

int main() {
    HashTableObject htObj = {0};
    struct Val val[MAX_ENTRY_COUNT];

    InitHashTableObject(&htObj, MAX_ENTRY_COUNT);
    for (unsigned int i = 0; i < ENTRY_COUNT; i++) {
        hidx_t idx = insert_entry(&htObj, filenames[i], strlen(filenames[i]) + 1);
        if (idx < 0) continue;
    }
    for (unsigned int i = 0; i < ENTRY_COUNT; i++) {
        search_key(&htObj, filenames[i], strlen(filenames[i]) + 1);
    }

    DestroyHashTableObject(&htObj);
    return 0;
}