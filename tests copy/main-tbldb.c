#include "global.h"
#include "txt_tok_lib.h"
#include "tbldb.h"

//typedef unsigned long long offptr_t;

const ulong_t line_buf_size = 1024;
/*
45,
Sandslash (Alolan Sandslash),
28,
7,

Ice,
Steel,

Standard,
75,

100,
120,

25,
65,

65,
43.0,
44.0
COL_ID                 = TYPE_LONG,
COL_Name               = TYPE_TEXT,
COL_Number             = TYPE_INT,
COL_Generation         = TYPE_UCHAR,
COL_Type1              = TYPE_TEXT,
COL_Type2              = TYPE_TEXT,
COL_Category           = TYPE_TEXT,
COL_Health             = TYPE_INT,
COL_Attack             = TYPE_INT,
COL_Defence            = TYPE_INT,
COL_Special_Attack     = TYPE_INT,
COL_Special_Defence    = TYPE_INT,
COL_Speed              = TYPE_INT,
COL_Evolves_From       = TYPE_FLOAT,
COL_Alternate_Of       = TYPE_FLOAT
*/
enum ColTypes {
    COL0_ID_TYPE                = TYPE_LONG,
    COL1_Name_TYPE              = TYPE_TEXT,
    COL2_Number_TYPE            = TYPE_INT,
    COL3_Generation_TYPE        = TYPE_UCHAR,
    COL4_Type1_TYPE             = TYPE_TEXT,
    COL5_Type2_TYPE             = TYPE_TEXT,
    COL6_Category_TYPE          = TYPE_TEXT,
    COL7_Health_TYPE            = TYPE_INT,
    COL8_Attack_TYPE            = TYPE_INT,
    COL9_Defence_TYPE           = TYPE_INT,
    COL10_Special_Attack_TYPE   = TYPE_INT,
    COL11_Special_Defence_TYPE  = TYPE_INT,
    COL12_Speed_TYPE            = TYPE_INT,
    COL13_Evolves_From_TYPE     = TYPE_FLOAT,
    COL14_Alternate_Of_TYPE     = TYPE_FLOAT,
};


static inline void PrintTableHeader(DBInMemTableSchema *table_schema_p) {
    if (!table_schema_p) return;
    putchar('|');
    for (int i = 0; i < table_schema_p->col_cnt; i++) {
        printf("%.*s|", (*table_schema_p).col_hdr_arr[i].col_name_len, (*table_schema_p).col_hdr_arr[i].col_name_str);
    }
    putchar('\n');
}

int main(int argc, char *argv[]) {
    FILE *fpcsv = fopen("Pokemon.csv", "r");
    if (!fpcsv) { perror("[ERROR] fopen"); return errno; }

    char *line = (char*)alloca(line_buf_size);  // sub rsp, line_buf_size; ret when main ends
    char *fgets_ret = fgets((char*)line, line_buf_size, fpcsv); // read first line
    if (!fgets_ret) {
        printerrf("[ERROR] fgets((char*)line, line_buf_size, fpcsv) failed\n");
        return -1;
    }
    // feof(fp): EOF; ferror(fp): I/O error

    ulong_t line_len = strlen(line);    // line len of first csv line (aka the header)
    line_len = lstrip(&line, line_len); // lstrip SPC
    line_len = rstrip(&line, line_len); // rstrip SPC
    ulong_t col_cnt = CntTok(line, line_len, ',');  // count token separated by ','
    //rewind(fpcsv);  // rewind csv cur back to off 0

    tokmeta_t *tok_info_arr = MarkTokPosMeta(line, line_len, col_cnt, ','); // \
        create an arr with each item of the arr being metadata of each token relative to \
        src str (line), with tok_len and relative off from (char *)line tok_off;
    //  each token's ptr is (char *)line_ptr + tok_info_arr[i].tok_off, len being \
        tok_info_arr[i].tok_len
    //  This method doesnt require dynamic allocation for each tok thus more efficient and easier to write
    //  copying tok is done by memcpy((char *)tok_p, (char *)src_p + tok_off, tok_len);
    //  (Note: each tok is NOT NUL terminated because i dont trust NUL instead i just access with \
        both tok_p and tok_len)
    if (!tok_info_arr) return -1;
    ulong_t col_type_arr[] = {
        COL0_ID_TYPE,
        COL1_Name_TYPE,
        COL2_Number_TYPE,
        COL3_Generation_TYPE,
        COL4_Type1_TYPE,
        COL5_Type2_TYPE,
        COL6_Category_TYPE,
        COL7_Health_TYPE,
        COL8_Attack_TYPE,
        COL9_Defence_TYPE,
        COL10_Special_Attack_TYPE,
        COL11_Special_Defence_TYPE,
        COL12_Speed_TYPE,
        COL13_Evolves_From_TYPE,
        COL14_Alternate_Of_TYPE
    };

    DBInMemTableSchema *table_schema_p = TBLDB_MakeDBInMemTableSchema(
        line, line_len, col_cnt, tok_info_arr, (ulong_t*)col_type_arr // I might have to add col_type_arr_len as well to the func param
    );
//\
ID,Name,Number,Generation,Type1,Type2,Category,Health,Attack,Defence,Special Attack,Special Defence,Speed,Evolves From,Alternate Of


    free(tok_info_arr); // tok_info_arr was only used for TBLDB_MakeDBInMemTableSchema;

    // Print table header
    PrintTableHeader(table_schema_p);

    // OpenDB
    //unlink("table.db"); // delete the old one
    TBLDBHandle dbp = TBLDB_OpenDB("table.db");
    if (!dbp) {
        printerrf("OpenDB failed");
        return -1;
    }

    // InsertDBTableSchema
    if (dbp->is_new_db) {
        TBLDB_InsertDBTableSchema(dbp, table_schema_p);

        TBLDB_DestroyDBInMemTableSchema(table_schema_p);


        while (fgets(line, line_buf_size, fpcsv)) {
            print_dbg_msg("EntryID=%.4u\n", dbp->db_file_header.EntryCount);

            line_len = strlen(line);
            line_len = lstrip(&line, line_len);
            line_len = rstrip(&line, line_len);

            tok_info_arr = MarkTokPosMeta(line, line_len, col_cnt, ',');
            if (!tok_info_arr) {
                printerrf("[ERROR] MarkTokPosMeta(line, line_len, col_cnt, ',') failed\n");
                break;
            }

            if (
                TBLDB_InsertDBEntry(dbp, line, line_len, tok_info_arr, col_cnt) != col_cnt
            ) {
                printerrf("TBLDB_InsertDBEntry(dbp, line, line_len, tok_info_arr, col_cnt) failed\n");
                break;
            }
            if (tok_info_arr) { free(tok_info_arr); tok_info_arr = NULL; }
        }
    }

    // CloseDB
    TBLDB_CloseDB(dbp);

    fclose(fpcsv);


    return 0;
}
