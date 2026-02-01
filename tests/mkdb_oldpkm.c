#include "../core/src/global.h"
#include "../core/src/kvdb_lib.h"
#include "../core/src/txt_tok_lib.h"

#define DB_ENTRY_CAP 4096

static inline int count_csv_rows(FILE *fp) {
    rewind(fp);
    if (!fp) return -1;
    char line[BUFFER_SIZE] = {0};
    ulong_t line_len = 0;
    int row_cnt = 0;
    while (fgets(line, BUFFER_SIZE, fp)) {
        line_len = strlen(line);
        if (line[line_len - 1] = '\n') ++row_cnt;
    }
    rewind(fp);
    return row_cnt;
}

static inline int count_csv_cols(FILE *fp) {
    if (!fp) return -1;
    rewind(fp);
    char line[BUFFER_SIZE] = {0};
    if (!fgets(line, BUFFER_SIZE, fp)) {
        print_err_msg("fgets failed\n");
        return -1;
    }
    ulong_t line_len = strlen(line);
    int col_cnt = (int)CntTok((char*)line, line_len, ',');
    rewind(fp);
    return col_cnt;
}

int main() {
    FILE *fpcsv = fopen("res/Pokemon.csv", "r");
    if (!fpcsv) {
        perror("fopen res/Pokemon.csv failed");
        return EXIT_FAILURE;
    }
    int row_cnt = count_csv_rows(fpcsv);
    int col_cnt = count_csv_cols(fpcsv);

    char line_buf[BUFFER_SIZE] = {0};
    char *line = (char*)line_buf;
    ulong_t line_len = 0;

    DIR *dir = opendir("database");
    if (!dir) {
        perror("opendir failed");
        return 1;
    }
    closedir(dir);
    DBObject* dbp = (DBObject*)DBInit("database/table0002.db", DB_ENTRY_CAP);
    if (!dbp) return -1;
#define db (*dbp)
    Key key = {0};
    Val val = {0};

    rewind(fpcsv);
    ulong_t ulong_t_size = sizeof(ulong_t);
    for (int i = 0; i < row_cnt; ++i) {
        if (!fgets(line, BUFFER_SIZE, fpcsv)) break;
        line_len = strlen(line);
        line_len = lstrip(&line, line_len);
        line_len = rstrip(&line, line_len);
        tokmeta_t *tok_info_arr = MarkTokPosMeta(line, line_len, col_cnt, ',');
        if (!tok_info_arr) {
            printerrf("MarkTokPosMeta(line, line_len, col_cnt, ',') failed\n");
            break;
        }

        key.size = 4;
        key.data = &i;
        //key.data = (qword_t*)alloca(sizeof(qword_t));
        //*(qword_t*)key.data = i;
        key.type = INT;


        val.size = 0;
        ubyte_t data_blob[BUFFER_SIZE] = {0};
        ubyte_t *data_blob_p = (ubyte_t*)data_blob;
        val.data = data_blob_p;
        //ptrdiff_t data_blob_cur = 0;
        for (int j = 0; j < col_cnt; ++j) {
            memcpy((ubyte_t*)data_blob_p, (ubyte_t*)&tok_info_arr[j].tok_off, ulong_t_size);
            data_blob_p += ulong_t_size;
            memcpy((ubyte_t*)data_blob_p, (ubyte_t*)&tok_info_arr[j].tok_len, ulong_t_size);
            data_blob_p += ulong_t_size;
            val.size += 2 * ulong_t_size;
        }
        memcpy((ubyte_t*)data_blob_p, (ubyte_t*)line, line_len);
        data_blob_p += line_len;
        val.size += line_len;

        val.data = (void*)data_blob;
        val.type = BLOB;

        InsertEntry(&db, key, val);

        free(tok_info_arr); tok_info_arr = NULL;
    }


    //WriteDBHeader(&db);

    fclose(fpcsv);

    //close_file_hash_table(&db);
    CloseDB(&db);
#undef db
    return 0;
}