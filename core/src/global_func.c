#include "global.h"

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