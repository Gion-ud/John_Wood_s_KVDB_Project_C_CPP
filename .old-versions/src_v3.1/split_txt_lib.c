#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define TOKEN_SIZE  64
#define EXIT_VOID   return

#define printerrf(...) fprintf(stderr, __VA_ARGS__)


int is_blank(char c) {
    const char space_char_arr[] = { ' ', '\n', '\r', '\t', '\v', '\f' };
    size_t size = sizeof(space_char_arr);
    for (size_t i = 0; i < size; i++) {
        if (space_char_arr[i] == c) {
            return 1;
        }
    }
    return 0;
}

char* strip(char* str) {
    if (str == NULL) {
        return NULL;
    }
    if (*str == (char)0) {
        return (char*)str;
    }
    char* str_start = (char*)str;
    while (is_blank((char)*str_start) != 0) {
        str_start++;
    }
    char* str_end = (char*)str + strlen(str) - 1;
    while (is_blank((char)*str_end) != 0) {
        *str_end = (char)0;
        str_end--;
    }
    return (char*)str_start;
}

int count_token(
    const char* str,
    const char  delim
) {
    if (str == (char*)0 || *str == (char)0) {
        return 0;
    }
    char* pStr = (char*)str;
    int token_count = 1;
    while (*pStr != (char)0) {
        if (*pStr == (char)delim) {
            token_count++;
        };
        pStr++;
    }
    printf("token_count=%d\n", token_count);
    return (int)token_count;
}

/*
char** split_txt(const char* src, const char delim, const int tok_count, const int max_tok_size) {
    if (!src || *src == '\0') return NULL;

    char** tokens = (char**)malloc(tok_count * sizeof(char*));
    if (!tokens) { printerrf("(char**)malloc(tok_count * sizeof(char*)) failed\n"); exit(EXIT_FAILURE); }
    {
        char** tok_arr_ptr = (char**)tokens;
        while (tok_arr_ptr < (char**)tokens + tok_count) {
            *tok_arr_ptr = (char*)malloc(max_tok_size * sizeof(char));
            if (!*tok_arr_ptr) {
                printerrf("(char*)malloc(max_tok_size * sizeof(char)) failed\n");
                exit(EXIT_FAILURE);
            }
            *tok_arr_ptr;
        }
    }

    char*  src_ptr      = (char*)src;
    char** tok_arr_ptr  = (char**)tokens;
    char*  tok          = (char*)*tok_arr_ptr;

    while (*src_ptr != '\0') {
        if (*src_ptr == (char)delim) {
            *tok = '\0';    // NUL terminate token
            tok_arr_ptr++;  // move to next token
            tok = (char*)*tok_arr_ptr;  // move tok forward
        } else {
            *tok = (char)*src_ptr;
            tok++;
        }
        src_ptr++;
    }
    *tok = '\0';
    return (char**)tokens;
}
*/

char** split_txt(const char* src, const char delim, const int tok_count, const int max_tok_size) {
    if (!src || *src == '\0') return NULL;

    char** tokens = (char**)malloc(tok_count * sizeof(char*));
    if (!tokens) { printerrf("(char**)malloc(tok_count * sizeof(char*)) failed\n"); exit(EXIT_FAILURE); }
{
    char** tok_arr_ptr = (char**)tokens;
    while (tok_arr_ptr < (char**)tokens + tok_count) {
        *tok_arr_ptr = (char*)malloc(max_tok_size * sizeof(char));
        if (!*tok_arr_ptr) {
            printerrf("(char*)malloc(max_tok_size * sizeof(char)) failed\n");
            exit(EXIT_FAILURE);
        }
        *tok_arr_ptr++;
    }
}

    char*  src_ptr      = (char*)src;
    char** tok_arr_ptr  = (char**)tokens;
    char*  tok          = (char*)*tok_arr_ptr;

    while (*src_ptr != '\0') {
        if (*src_ptr == (char)delim) {
            *tok = '\0';    // NUL terminate token
            tok_arr_ptr++;  // move to next token
            tok = (char*)*tok_arr_ptr;  // move tok forward
        } else {
            *tok = (char)*src_ptr;
            tok++;
        }
        src_ptr++;
    }
    *tok = '\0';
    return (char**)tokens;
}


void free_tokens(char** tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens); 
}

/*

int main() {
    char buffer[BUFFER_SIZE];
    printf("Enter some text:\n");
    fgets((char*)buffer, BUFFER_SIZE, stdin);

    printf("You entered: \"%s\"\n", (char*)buffer);
    char *stripped = (char*)strip((char*)buffer);
    memcpy(buffer, stripped, BUFFER_SIZE);

    printf("After strip: \"%s\"\n", (char*)buffer);

    int token_count = count_token(buffer, ' ');
    char** tokens = (char**)split_txt(buffer, ' ', token_count, TOKEN_SIZE);

    for (int i = 0; i < token_count; i++) {
        printf("\"%s\"", (char*)tokens[i]);
        if (i < token_count - 1) {
            printf(", ");
        } else {
            printf("\n");
        }
    }
    free_tokens(tokens, token_count);

    return 0;
}
*/