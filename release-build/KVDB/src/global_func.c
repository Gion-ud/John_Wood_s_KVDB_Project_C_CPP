#include "global.h"

char* conv_bytes_hex(const unsigned char* bytes, size_t size) {
//  This function converts raw bytes into str "0x??,0x??,0x??,...,0x??"
    char* buffer = (char*)malloc(size * 8ull);
    if (!buffer) {
        perror("malloc failed");
        return NULL;
    }
    buffer[0] = '\0';
    char ByteHex[8];
    int len;
    for (int i = 0; i < size; i++) {
        sprintf(ByteHex, "0x%02X", (unsigned char)bytes[i]);
        if ((i + 1) % 16 != 0 && i < size - 1) {
            // 16 bytes in a row
            len = strlen(ByteHex);
            ByteHex[len++] = ',';
            ByteHex[len] = '\0';
        } else {
            if (i < size - 1) {
                len = strlen(ByteHex);
                ByteHex[len++] = ',';
                ByteHex[len] = '\0';

                len = strlen(ByteHex);
                ByteHex[len++] = '\\'; // '\\' used to cancel out LF
                ByteHex[len] = '\0';
            }

            len = strlen(ByteHex);
            ByteHex[len++] = '\n';
            ByteHex[len] = '\0';
        }
        strcat(buffer, ByteHex);
    }

    return (char*)buffer;
    // needs to be freed!
}

char *conv_time_str(time_t time_var) {
    char *timestr = (char*)malloc(TIME_STR_SIZE);
    if (!timestr) {
        perror("[ERROR] (char*)malloc(TIME_STR_SIZE)");
        return NULL;
    };
    struct tm *t_local = localtime(&time_var);

    snprintf(
        timestr, TIME_STR_SIZE,
        "%04u-%02u-%02u %02d:%02d:%02d",
        (uword_t)t_local->tm_year + 1900,
        (uchar_t)t_local->tm_mon + 1,
        (uchar_t)t_local->tm_mday,
        (uchar_t)t_local->tm_hour,
        (uchar_t)t_local->tm_min,
        (uchar_t)t_local->tm_sec
    );
    return timestr;
    // It Must be freed
}

void conv_time_str_modptr(char (*timestr_p_p)[TIME_STR_SIZE], time_t time_var) {
#define timestr (*timestr_p_p)
    // ptr MUST BE INITIALISED
    struct tm *t_local = localtime(&time_var);
    snprintf(
        (char*)timestr, TIME_STR_SIZE,
        "%04u-%02u-%02u %02d:%02d:%02d",
        (uword_t)t_local->tm_year + 1900,
        (uchar_t)t_local->tm_mon + 1,
        (uchar_t)t_local->tm_mday,
        (uchar_t)t_local->tm_hour,
        (uchar_t)t_local->tm_min,
        (uchar_t)t_local->tm_sec
    );
#undef timestr
}