#include "kvdb_lib.h"
#include "txt_tok_lib.h"

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

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <import csv path> <export db path> (<dbg file>)\n",
            argc, argv[0]
        );
        return -1;
    }

    FILE *fpcsv = fopen(argv[1], "r");
    if (!fpcsv) {
        perror("failed to open <import csv>");
        return EXIT_FAILURE;
    }
    int row_cnt = count_csv_rows(fpcsv);
    int col_cnt = count_csv_cols(fpcsv);

    char line_buf[BUFFER_SIZE] = {0};
    char *line = (char*)line_buf;
    ulong_t line_len = 0;

    DBObject* dbp = (DBObject*)DBInit(argv[2], DB_ENTRY_CAP);
    if (!dbp) return -1;

#define db (*dbp)
    Key key = {0};
    Val val = {0};

    rewind(fpcsv);

    ubyte_t data_blob[BUFFER_SIZE] = {0};
    ubyte_t *data_blob_p = (ubyte_t*)data_blob;
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
        key.type = INT;



        data_blob_p = (ubyte_t*)data_blob;

        for (int j = 0; j < col_cnt; ++j) {
            memcpy((ubyte_t*)data_blob_p, (ubyte_t*)&tok_info_arr[j].tok_len, sizeof(ulong_t));
            data_blob_p += sizeof(ulong_t); // [data_len] at idx j

            memcpy((ubyte_t*)data_blob_p, (ubyte_t*)line + tok_info_arr[j].tok_off, tok_info_arr[j].tok_len);
            data_blob_p += tok_info_arr[j].tok_len;
        }

        val.size = data_blob_p - (ubyte_t*)data_blob;
        val.data = (void*)data_blob;
        val.type = BLOB;

        InsertEntry(&db, key, val);

        free(tok_info_arr); tok_info_arr = NULL;
    }
    //PrintIndexTable(stdout, dbp);

    //WriteDBHeader(&db);

    fclose(fpcsv);

    //close_file_hash_table(&db);
    CloseDB(&db);
#undef db
    return 0;
}