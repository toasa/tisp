#include "tisp.h"

bool is_integer(char c) {
    return ('0' <= c) && (c <= '9');
}

void assert(int result, char *fmt, ...) {
    if (!result) {
        fprintf(stderr, "assertion failed\n");
        error(fmt);
    }
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
