#include "txt_tok_lib.h"

ulong_t CntTok(const char *src, ulong_t src_len, const char delim) {
    if (!src || !src[0]) return 0;
    ulong_t tok_cnt = 1;
    for (ulong_t i = 0; i < src_len; ++i) {
        if (src[i] == delim) ++tok_cnt;
    }
    return tok_cnt;
}


tokmeta_t *MarkTokPosMeta(const char* src, ulong_t src_len, ulong_t tok_cnt, const char delim) {
    if (!src || !src[0]) return NULL;

    tokmeta_t *tok_info_arr = (tokmeta_t*)calloc(tok_cnt, sizeof(tokmeta_t));
    if (!tok_info_arr) {
        perror("(tokmeta_t*)calloc(tok_cnt, sizeof(tokmeta_t))\n");
        return NULL;
    }
    //off_t tok_off_ptr = 0;
    uint_t tok_len = 0;
    ulong_t tok_idx = 0;

    for (ulong_t i = 0; i < src_len + 1; ++i) {
        if (src[i] == delim || i == src_len) {
            tok_info_arr[tok_idx].tok_off = i - tok_len;
            tok_info_arr[tok_idx].tok_len = tok_len;
            tok_len = 0;
            ++tok_idx;
        } else {
            ++tok_len;
        }
    }
    return tok_info_arr;
    // Must be freed!
}

char *CpyTokSrc(const char* src, ulong_t src_len, tokmeta_t tok_info) {
    if (tok_info.tok_off + tok_info.tok_len > src_len) return NULL;
    char *tok = (char*)malloc(tok_info.tok_len + 1);
    if (!tok) { perror("(char*)malloc(tok_info.tok_len + 1)"); return NULL; }
    strncpy(tok, src + tok_info.tok_off, tok_info.tok_len);
    tok[tok_info.tok_len] = '\0';
    return tok;
}

ulong_t lstrip(char **pLine, ulong_t len) {
    char *line_ptr = *pLine;
    while (len > 0) {
        if (!isspace(*line_ptr)) break;
        ++line_ptr;
        --len;
    }
    *pLine = line_ptr;
    return len;
}




ulong_t rstrip(char *const *pLine, ulong_t len) {
#define line (*pLine)
    char *line_ptr = line;
    while (len > 0) {
        if (!isspace(line[len - 1])) break;
        line[(len--) - 1] = '\0';
    }
#undef line
    return len;
}

