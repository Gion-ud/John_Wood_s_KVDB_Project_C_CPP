#include "hash_table_lib.h"
#include <dirent.h>
//#include "dirent.h"

int main() {
    HashTableObject htObj;
    DIR* dir = opendir("../res/images");
    if (!dir) { perror("opendir failed"); return 1; }
    struct dirent* p_entry;

    int dir_entry_count = 0;
    while (LOOP_RUNNING) {
        p_entry = readdir(dir);
        if (!p_entry) break;
        dir_entry_count++;
    }
    rewinddir(dir);

    InitHashTableObject(&htObj, dir_entry_count + 1);

    struct dirent* entry_arr = (struct dirent*)malloc(dir_entry_count * sizeof(struct dirent));
    if (!entry_arr) { perror("malloc"); return 1; }


    for (int i = 0; i < dir_entry_count; i++) {
        p_entry = readdir(dir);
        if (!p_entry) break;
        memcpy(&entry_arr[i], p_entry, sizeof(struct dirent));
        insert_entry(&htObj, (byte_t*)entry_arr[i].d_name, strlen(entry_arr[i].d_name) + 1);
        printf("%s\n", entry_arr[i].d_name);
    }

    for (int i = 0; i < dir_entry_count; i++) {
        hidx_t idx = search_key(&htObj, (byte_t*)entry_arr[i].d_name, strlen(entry_arr[i].d_name) + 1);
        if (idx < 0) { print_err_msg("key not found\n"); continue; }
        printf("key: \"%*s\"\n", htObj.hash_entry_arr[idx].key_size, htObj.hash_entry_arr[idx].key);
    }

    delete_entry(&htObj, (byte_t*)entry_arr[23].d_name, strlen(entry_arr[23].d_name) + 1);
    delete_entry(&htObj, (byte_t*)entry_arr[1].d_name, strlen(entry_arr[1].d_name) + 1);
    delete_entry(&htObj, (byte_t*)entry_arr[14].d_name, strlen(entry_arr[14].d_name) + 1);
    delete_entry(&htObj, (byte_t*)entry_arr[17].d_name, strlen(entry_arr[17].d_name) + 1);

    for (int i = 0; i < dir_entry_count; i++) {
        hidx_t idx = search_key(&htObj, (byte_t*)entry_arr[i].d_name, strlen(entry_arr[i].d_name) + 1);
        if (idx < 0) { print_err_msg("key not found\n"); continue; }
        printf("key: \"%*s\"\n", htObj.hash_entry_arr[idx].key_size, htObj.hash_entry_arr[idx].key);
    }


    DestroyHashTableObject(&htObj);
    free(entry_arr);
    closedir(dir);
    return 0;
}