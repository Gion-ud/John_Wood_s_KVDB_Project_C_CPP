#ifndef FILEOBJ_HPP
#define FILEOBJ_HPP

#include <cstdlib>
#include <cstdio>

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
            fputs("fread failed\n", stderr);
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
            fputs("fread failed\n", stderr);
        }
        return fwrite_cnt;
    }
};


#endif