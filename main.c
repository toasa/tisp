#include <stdio.h>

static char input[2048];

int main() {
    while (1) {
        fputs("tisp> ", stdout);
        fgets(input, 2048, stdin);
        printf("%s", input);
    }
}
