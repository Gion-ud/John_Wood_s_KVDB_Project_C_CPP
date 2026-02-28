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


int main() {
    KVDB db;
    if (!db.open("images.db")) return -1;
    DBObject *dbp = db.get_this();

    Val *val = KVDB_DBObject_get(dbp, 0);
    if (!val) {
        std::cerr << "KVDB_DBObject_get failed\n";
        return -1;
    }

    ulong_t data_type               = *(ulong_t*)((byte_t*)val->data);
    ulong_t data_decompressed_size  = *(ulong_t*)((byte_t*)val->data + sizeof(ulong_t));
    ulong_t data_compressed_size    = *(ulong_t*)((byte_t*)val->data + 2 * sizeof(ulong_t));
    byte_t *data_compressed_p       = (byte_t*)val->data + 3 * sizeof(ulong_t);
    byte_t *buf = new (std::nothrow) byte_t[data_decompressed_size];
    if (!buf) {
        std::cerr << "new failed\n";
        KVDB_TLVDataObject_destroy(val);
        return -1;
    }

    int ret = uncompress(
        buf,
        (uLongf*)&data_decompressed_size,
        data_compressed_p,
        data_compressed_size
    );

    if (ret != Z_OK) {
        std::cerr << "uncompress failed\n";
        delete[] buf;
        KVDB_TLVDataObject_destroy(val);
        return -1;
    }

    KVDB_TLVDataObject_destroy(val); val = nullptr;

    FILE_OBJECT f_out;
    if (
        !f_out.fopen(
            "C:/John_Wood/Coding_Projects/c/KVDB/tests/of.jpeg",
            "wb"
        )
    ) {
        delete[] buf;
        return errno;
    }

    f_out.fwrite(buf, 1, data_decompressed_size);

    delete[] buf;

    //f_in.~FILE_OBJECT();
    //db.~KVDB();

    return 0;
}