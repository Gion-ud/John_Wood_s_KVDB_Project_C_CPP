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
        if (line[line_len - 1] == '\n') ++row_cnt;
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
    if (argc == 1) {
        printf(
            "Usage: %s <import csv path> <primary key col idx> <export db path> (<dbg file>)\n",
            argv[0]
        );
        return 0;
    }

    if (argc < 4 || argc > 5) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <import csv path> <primary key col idx> <export db path> (<dbg file>)\n",
            argc, argv[0]
        );
        return -1;
    }

    FILE *fpcsv = fopen(argv[1], "r");
    if (!fpcsv) {
        perror("failed to open <import csv>");
        return errno;
    }
    int row_cnt = count_csv_rows(fpcsv);
    int col_cnt = count_csv_cols(fpcsv);

    int pk_col_idx = atoi(argv[2]);
    if (pk_col_idx < 0 || pk_col_idx > col_cnt - 1) {
        print_err_msg("primary key col idx out of bound\n");
        fclose(fpcsv);
        return -1;
    }

    char line_buf[BUFFER_SIZE] = {0};
    char *line = (char*)line_buf;
    ulong_t line_len = 0;

    DBObject* dbp = (DBObject*)KVDB_DBObject_open(argv[3]);
    if (!dbp) return -1;

#define db (*dbp)
    Key key = {0};
    Val val = {0};

    rewind(fpcsv);

    size_t db_filename_len = strlen(argv[3]);
    char *filename_keys = (char*)malloc(db_filename_len + 16);
    if (!filename_keys) {
        printerrf("malloc failed\n");
        goto section_insert_records;
    }
    memcpy(filename_keys, argv[3], db_filename_len);
    memcpy(filename_keys + db_filename_len, "_keys.txt", 10);

    FILE *fp_keys_txt = NULL;
    fp_keys_txt = fopen(filename_keys, "wb");
    if (!fp_keys_txt) {
        print_err_msg("fopen failed: %s\nusing stdout instead\n", strerror(errno));
        fp_keys_txt = stdout;
    }

section_insert_records:
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

        key.type = BLOB;
        key.len = tok_info_arr[pk_col_idx].tok_len;
        key.data = line + tok_info_arr[pk_col_idx].tok_off;

        fprintf(fp_keys_txt, "%.4d:", i);
        fwrite(key.data, 1, key.len, fp_keys_txt);
        fputc('\n', fp_keys_txt);

        data_blob_p = (ubyte_t*)data_blob;

        for (int j = 0; j < col_cnt; ++j) {
            memcpy((ubyte_t*)data_blob_p, (ubyte_t*)&tok_info_arr[j].tok_len, sizeof(ulong_t));
            data_blob_p += sizeof(ulong_t); // [data_len] at idx j

            memcpy((ubyte_t*)data_blob_p, (ubyte_t*)line + tok_info_arr[j].tok_off, tok_info_arr[j].tok_len);
            data_blob_p += tok_info_arr[j].tok_len;
        }

        val.len = data_blob_p - (ubyte_t*)data_blob;
        val.data = (void*)data_blob;
        val.type = BLOB;

        KVDB_DBObject_insert(&db, key, val);

        free(tok_info_arr); tok_info_arr = NULL;
    }
    //PrintIndexTable(stdout, dbp);

    KVDB_DBObject_delete(&db, 1);
    KVDB_DBObject_delete(&db, 10);
    KVDB_DBObject_delete(&db, 11);
    KVDB_DBObject_delete(&db, 63);
    KVDB_DBObject_delete(&db, 189);
    KVDB_DBObject_delete(&db, 32);
    KVDB_DBObject_delete(&db, 12000);
    KVDB_DBObject_delete(&db, 32);
    KVDB_DBObject_delete(&db, 1200);
    KVDB_DBObject_delete(&db, 848);
    KVDB_DBObject_delete(&db, 934);


    //WriteDBHeader(&db);

    fclose(fpcsv);
    if (fp_keys_txt && fp_keys_txt != stdout) fclose(fp_keys_txt);

    //close_file_hash_table(&db);
    KVDB_DBObject_close(&db);
#undef db
    return 0;
}