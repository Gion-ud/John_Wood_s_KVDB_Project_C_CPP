#include <tlv/tlv.h>
#include <mem/mem_arena.h>


int TLVDataObject_init_arena_alloc(
    TLVDataObject      *this,
    struct mem_arena   *mem_arena_p,
    uchar_t             type,
    size32_t            len,
    void               *data
) {
// this mush be initialised
    if (!this) return -1;
    this->type = type;
    this->len = len;
    this->data = mem_arena_alloc(mem_arena_p, len);
    if (!this->data) {
        printerrf("mem_arena_alloc failed\n");
        return -1;
    }
    memcpy(this->data, data, len);
    return (int)len;
}

void TLVDataObject_print(TLVDataObject *this) {
    switch(this->type) {
        case (TYPE_CHAR):
        {
            if (this->len != sizeof(byte_t)) break;
            printf("%c", *(byte_t*)this->data);
            break;
        }
        case (TYPE_SHORT):
        {
            if (this->len != sizeof(short_t)) break;
            printf("%hd", *(short_t*)this->data);
            break;
        }
        case (TYPE_USHORT):
        {
            if (this->len != sizeof(word_t)) break;
            printf("%hu", *(word_t*)this->data);
            break;
        }
        case (TYPE_LONG):
        {
            if (this->len != sizeof(long_t)) break;
            printf("%d", *(long_t*)this->data);
            break;
        }
        case (TYPE_ULONG):
        {
            if (this->len != sizeof(ulong_t)) break;
            printf("%u", *(ulong_t*)this->data);
            break;
        }
        case (TYPE_LONGLONG):
        {
            if (this->len != sizeof(longlong_t)) break;
            printf("%lld", *(longlong_t*)this->data);
            break;
        }
        case (TYPE_ULONGLONG):
        {
            if (this->len != sizeof(ulonglong_t)) break;
            printf("%llu", *(ulonglong_t*)this->data);
            break;
        }
        case (TYPE_FLOAT):
        {
            if (this->len != sizeof(float)) break;
            printf("%f", *(float*)this->data);
            break;
        }
        case (TYPE_DOUBLE):
        {
            if (this->len != sizeof(float)) break;
            printf("%lf", *(double*)this->data);
            break;
        }
        case (TYPE_BOOL):
        {
            if (this->len != sizeof(byte_t)) break;
            if (*(byte_t*)this->data) {
                fputs("true", stdout);
            } else {
                fputs("false", stdout);
            }
            break;
        }
        case (TYPE_TEXT):
        {
            printf("%.*s", (int)this->len, (byte_t*)this->data);
            break;
        }
        default:
        {
            char *hexbytestr = conv_bytes_hex(this->data, this->len);
            if (!hexbytestr) break;
            fputs(hexbytestr, stdout);
            free(hexbytestr);
            break;
        }
    }
}
void TLVDataObject_deinit(TLVDataObject *this) {
    if (!this) return;
    this->type = 0;
    this->len = 0;
    //if (this->data) { free(this->data); this->data = NULL; }
    this->data = NULL;
    return;
}
