#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define LOOP_RUNNING 1

int main() {
    DIR *dir_p = opendir("../res/images");
    if (!dir_p) {
        perror("opendir failed");
        return 1;
    }
    struct dirent* p_entry;
    struct dirent* entry_arr = (struct dirent*)calloc(32, sizeof(struct dirent));
    if (!entry_arr) { perror("malloc"); return 1; }
    int i = 0;
    while (LOOP_RUNNING) {
        p_entry = readdir(dir_p);
        if (!p_entry) break;
        memcpy(&entry_arr[i], p_entry, sizeof(struct dirent));
        printf("%s\n", entry_arr[i].d_name);
        i++;
    }
    free(entry_arr);
    closedir(dir_p);
    return 0;
}