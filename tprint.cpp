#include "tprint.h"


int tprint (const char *format, ...)
{
    static char str[TPRINT_SYMBOLS_MAX];
    static wchar_t w_str[TPRINT_SYMBOLS_MAX];
    if (format == NULL)
        return 0;
    va_list args;
    va_start(args, format);
    vsnprintf(str, sizeof(str) / sizeof(str[0]), format, args);
    va_end(args);
    mbstowcs(w_str, str, sizeof(w_str) / sizeof(w_str[0]));
    return wprintf(w_str);
}


int tprint (const wchar_t *format, ...)
{
    static wchar_t w_str[TPRINT_SYMBOLS_MAX];
    if (format == NULL)
        return 0;
    va_list args;
    va_start(args, format);
    vswprintf(w_str, sizeof(w_str) / sizeof(w_str[0]), format, args);
    va_end(args);
    return wprintf(w_str);
}
