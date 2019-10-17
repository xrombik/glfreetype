#ifndef TPRINT
#define TPRINT

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>

#define TPRINT_SYMBOLS_MAX 1024

/** */
int tprint (const char *format, ...);

/** */
int tprint (const wchar_t *format, ...);

#endif // TPRINT
