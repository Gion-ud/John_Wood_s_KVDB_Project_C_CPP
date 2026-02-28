extern "C" {
    #include <stddef.h>
    #include <errno.h>
    #include <zlib.h>
}
#include "kvdb.hpp"
#include "fileobj.hpp"
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <new>

const char *image_filepath = "C:/John_Wood/Coding_Projects/c/KVDB/res/images/Auckland-Itinerary.jpg.optimal.jpg";

int main() {
    KVDB db;
    if (!db.create("images.db", 4)) return -1;
    FILE_OBJECT f_in;
    if (!f_in.fopen(image_filepath, "rb")) return errno;

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
        .len = (size32_t)strlen(image_filepath),
        .data = (char*)image_filepath
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

    // [(dword_t)blob_type]
    // [(ulong_t)buf_uncompressed_len]
    // [(ulong_t)buf_compressed_len]
    // [(byte_t [])buf]
    dword_t buf_type = TYPE_BLOB;
    memcpy((byte_t*)val.data + val.len, (dword_t*)&buf_type, sizeof(dword_t));
    val.len += sizeof(dword_t);
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