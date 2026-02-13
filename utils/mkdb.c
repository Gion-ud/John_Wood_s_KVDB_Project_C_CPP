#include "kvdb.h"
#include "txt_tok_lib.h"

#define DB_ENTRY_CAP 4096

#define prog_name argv[0]
#define input_filepath argv[1]
#define output_filepath argv[2]
#define db_cap_str argv[3]

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <import kv file path> <export db path> <db cap>\n",
            argc, prog_name
        );
        return -1;
    }

    FILE *if_fp = fopen(input_filepath, "r");
    if (!if_fp) {
        perror("failed to open <input file>");
        return errno;
    }

    char line_buf[BUFFER_SIZE] = {0};
    char *line = (char*)line_buf;
    ulong_t line_len = 0;

    int db_cap = atoi(db_cap_str);
    DBObject* dbp = (DBObject*)KVDB_DBObject_create(output_filepath, db_cap);
    if (!dbp) return -1;

#define db (*dbp)
    TLVDataObject key = {0}, val = {0};

    while (1) {
        if (!fgets(line, BUFFER_SIZE, if_fp)) break;
        line_len = strlen(line);
        line_len = lstrip(&line, line_len);
        line_len = rstrip(&line, line_len);
        if (line_len == 0 || line[0] == '#') continue;
    
        tokmeta_t *tok_info_arr = MarkTokPosMeta(line, line_len, 2, '=');
        if (!tok_info_arr) {
            printerrf("MarkTokPosMeta failed\n");
            break;
        }

        key.type = TYPE_TEXT;
        key.len = tok_info_arr[0].tok_len;
        key.data = line + tok_info_arr[0].tok_off;

        val.type = TYPE_TEXT;
        val.len = tok_info_arr[1].tok_len;
        val.data = line + tok_info_arr[1].tok_off;


        KVDB_DBObject_put(&db, key, val);

        free(tok_info_arr); tok_info_arr = NULL;
    }
    //PrintIndexTable(stdout, dbp);

    /*
    KVDB_DBObject_delete(&db, 1);
    KVDB_DBObject_delete(&db, 10);
    KVDB_DBObject_delete(&db, 11);
    KVDB_DBObject_delete(&db, 63);
    KVDB_DBObject_delete(&db, 14);
    KVDB_DBObject_delete(&db, 7);
    KVDB_DBObject_delete(&db, 12000);
    */


    fclose(if_fp);

    //close_file_hash_table(&db);
    KVDB_DBObject_close(&db);
#undef db
    return 0;
}