#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define LOOP_RUNNING 1
#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: <prog name> <dir>\n");
        return 1;
    }
    DIR *dir_p = opendir(argv[1]);
    if (!dir_p) {
        perror("opendir failed");
        return 1;
    }
    struct dirent* p_entry;
    char sh[BUF_SIZE] = {0};
    while (LOOP_RUNNING) {
        p_entry = readdir(dir_p);
        if (!p_entry) break;
        snprintf(sh, BUF_SIZE, "rm \"%s\"\n", (*p_entry).d_name);
        system(sh);
    }
    closedir(dir_p);
    return 0;
}