#include "tlv.h"

int TLV_TLVDataObject_init(TLVDataObject *this, uchar_t type, size32_t len, void *data) {
// this mush be initialised
    if (!this) return -1;
    this->type = type;
    this->len = len;
    this->data = malloc(len);
    if (!this->data) {
        printerrf("malloc failed\n");
        return -1;
    }
    memcpy(this->data, data, len);
    return (int)len;
}
void TLV_TLVDataObject_print(TLVDataObject *this) {
    switch(this->type) {
        case (TYPE_CHAR):
        {
            if (this->len != sizeof(byte_t)) break;
            printf("%c", *(byte_t*)this->data);
            break;
        }
        case (TYPE_SHORT):
        {
            if (this->len != sizeof(word_t)) break;
            printf("%hd", *(word_t*)this->data);
            break;
        }
        case (TYPE_USHORT):
        {
            if (this->len != sizeof(uword_t)) break;
            printf("%hu", *(uword_t*)this->data);
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
            if (this->len != sizeof(qword_t)) break;
            printf("%lld", *(qword_t*)this->data);
            break;
        }
        case (TYPE_ULONGLONG):
        {
            if (this->len != sizeof(uqword_t)) break;
            printf("%llu", *(uqword_t*)this->data);
            break;
        }
        case (TYPE_FLOAT):
        {
            if (this->len != sizeof(float)) break;
            printf("%f", *(uqword_t*)this->data);
            break;
        }
        case (TYPE_DOUBLE):
        {
            if (this->len != sizeof(float)) break;
            printf("%lf", *(uqword_t*)this->data);
            break;
        }
        case (TYPE_BOOL):
        {
            if (this->len != sizeof(ubyte_t)) break;
            if (*(ubyte_t*)this->data) {
                fputs("true", stdout);
            } else {
                fputs("false", stdout);
            }
            break;
        }
    }
}
void TLV_TLVDataObject_deinit(TLVDataObject *this) {
    if (!this) return;
    this->type = 0;
    this->len = 0;
    if (this->data) { free(this->data); this->data = NULL; }
    return;
}
