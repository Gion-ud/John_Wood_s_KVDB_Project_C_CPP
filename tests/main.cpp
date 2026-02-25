extern "C" {
    #include "kvdb.h"
    #include "kvdb_defs.h"
    #include <stddef.h>
    #include <errno.h>
    #include <zlib.h>
}
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <new>

class KVDB {
private:
    DBObject *dbp;
public:
    KVDB() {
        dbp = nullptr;
    }
    bool create(const char *filepath, int EntryCapacity) {
        this->dbp = KVDB_DBObject_create(filepath, EntryCapacity);
        if (!this->dbp) {
            std::cerr << "KVDB::create failed\n";
            return false;
        }
        return true;
    }
    bool open(const char *filepath) {
        this->dbp = KVDB_DBObject_open(filepath);
        if (!this->dbp) {
            std::cerr << "KVDB::open failed\n";
            return false;
        }
        return true;
    }
    int put(Key& key, Val& val) {
        if (!this->dbp) return -1;
        int ret = KVDB_DBObject_put(this->dbp, &key, &val);
        if (ret < 0) {
            std::cerr << "KVDB::put failed\n";
            return -1;
        }
        return ret;
    }
    Val *get(Key& key) {
        if (!this->dbp) return nullptr;
        Val *val = KVDB_DBObject_get_by_key(this->dbp, &key);
        if (!val) {
            std::cerr << "KVDB::get failed\n";
            return nullptr;
        }
        return val;
    }
    void destroy_val_p(Val *val) {
        if (!val) return;
        KVDB_TLVDataObject_destroy(val);
    }
    DBObject *get_this() {
        return this->dbp;
    }
    ~KVDB() {
        if (this->dbp) {
            KVDB_DBObject_close(this->dbp);
            this->dbp = nullptr;
        }
    }
};

class FILE_OBJECT {
private:
    std::FILE *fp;
public:
    FILE_OBJECT() {
        this->fp = nullptr;
    }
    ~FILE_OBJECT() {
        if (this->fp) std::fclose(this->fp);
    }
    std::FILE *fopen(const char *filepath, const char *mode) {
        if (this->fp) std::fclose(this->fp);
        this->fp = std::fopen(filepath, mode);
        if (!this->fp) {
            std::perror("fopen failed");
        }
        return this->fp;
    }
    int fseek(long off, int origin) {
        if (!this->fp) return -1;
        return std::fseek(this->fp, off, origin);
    }
    long ftell() {
        if (!this->fp) return -1;
        return std::ftell(this->fp);
    }
    size_t fread(void *buf, size_t size, size_t cnt) {
        if (!this->fp) return 0;
        size_t fread_cnt = std::fread(buf, size, cnt, this->fp);
        if (fread_cnt != cnt) {
            std::cerr << "fread failed\n";
        }
        return fread_cnt;
    }
    long f_get_size() {
        if (!this->fp) return -1;
        long pos = std::ftell(this->fp);
        std::fseek(this->fp, 0, SEEK_END);
        long f_size = std::ftell(this->fp);
        std::fseek(this->fp, pos, SEEK_SET);
        return f_size;
    }
    size_t fwrite(void *buf, size_t size, size_t cnt) {
        if (!this->fp) return 0;
        size_t fwrite_cnt = std::fwrite(buf, size, cnt, this->fp);
        if (fwrite_cnt != cnt) {
            std::cerr << "fwrite failed\n";
        }
        return fwrite_cnt;
    }
};

const char *db_filepath = "C:/John_Wood/Coding_Projects/c/KVDB/res/images/Auckland-Itinerary.jpg.optimal.jpg";

int main() {
    KVDB db;
    if (!db.create("images.db", 4)) return -1;
    FILE_OBJECT f_in;
    if (!f_in.fopen(db_filepath, "rb")) return errno;

    long buf_len = f_in.f_get_size();
    byte_t *buf = (byte_t*)std::malloc(buf_len);
    if (!buf) {
        std::cerr << "std::malloc failed\n";
        return -1;
    };
    f_in.fseek(0, SEEK_SET);
    f_in.fread(buf, 1, buf_len);

    Key key = {
        .type = TYPE_TEXT,
        .len = (size32_t)strlen(db_filepath),
        .data = (char*)db_filepath
    };
    Val val = {
        .type = TYPE_BLOB,
        .len = 0,
        .data = nullptr,
    };

    uLongf buf_compressed_len = compressBound(buf_len);

    val.data = (byte_t*)std::malloc(buf_compressed_len + 32);
    if (!val.data) {
        std::cerr << "std::malloc failed\n";
        free(buf);
        return -1;
    }

    memcpy((byte_t*)val.data + val.len, (ulong_t*)&buf_len, sizeof(ulong_t));
    val.len += sizeof(ulong_t);
    ulong_t *buf_compressed_len_p = (ulong_t*)((byte_t*)val.data + val.len);
    val.len += sizeof(ulong_t);
    int ret = compress((Bytef*)val.data + val.len, &buf_compressed_len, buf, buf_len);
    
    if (ret != Z_OK) {
        std::cerr << "compress failed\n";
        std::free(buf);
        std::free(val.data);
        return -1;
    }
    memcpy(buf_compressed_len_p, &buf_compressed_len, sizeof(ulong_t));
    val.len += buf_compressed_len;

    ret = db.put(key, val);
    if (ret < 0) {
        std::cerr << "db.put failed\n";
        std::free(val.data);
        std::free(buf);
        return -1;
    }
    db.get_this()->IndexTable[ret].Flags |= FLAG_COMPRESSED; //temporary solution


    std::free(val.data);
    std::free(buf);

    //f_in.~FILE_OBJECT();
    //db.~KVDB();

    return 0;
}