extern "C" {
    #include "kvdb.h"
}
#include <stdexcept>
#include <iostream>
#include <cstdlib>

class KVDB {
private:
    DBObject *dbp;
public:
    KVDB() {
        dbp = nullptr;
    }
    DBObject *create(const char *filepath, int EntryCapacity) {
        this->dbp = KVDB_DBObject_create(filepath, EntryCapacity);
        if (!this->dbp) {
            std::cerr << "KVDB::create failed\n";
            return nullptr;
        }
    }
    DBObject *open(const char *filepath) {
        this->dbp = KVDB_DBObject_open(filepath);
        if (!this->dbp) {
            std::cerr << "KVDB::open failed\n";
            return nullptr;
        }
    }
    
    ~KVDB() {
        if (this->dbp) {
            KVDB_DBObject_close(this->dbp);
            this->dbp = nullptr;
        }
    }
};


int main() {
    KVDB db_h;
    if (!db_h.create("images.db", 4)) return -1;


    std::FILE *fp = std::fopen("../res/res/images/121200433_p2.jpg", "rb");
    if (!fp) {
        std::cerr << "fopen failed\n";
        return errno;
    };



    std::fclose(fp);

    return 0;
}