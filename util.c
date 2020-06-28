#include "tisp.h"

bool is_integer(char c) {
    return ('0' <= c) && (c <= '9');
}

bool is_alpha(char c) {
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

bool equal_strings(char *str1, char *str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if (len1 != len2) {
        return false;
    }
    for (int i = 0; i < len1; i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
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
