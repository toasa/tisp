#include "tisp.h"

static char input[1024];

struct Cell *read() {
    printf("tisp> ");

    // flash input
    for (int i = 0; i < 1024; i++) {
        input[i] = 0;
    }

    fgets(input, 1024, stdin);
    struct Token *tokens = tokenize(input);
    return gen_cells(tokens);
}

struct Cell *eval(struct Cell *c) {
    return c;
}

void print(struct Cell *c) {
    printf("%d\n", c->val);
}

int main() {
    while (1) {
        struct Cell *c = read();
        c = eval(c);
        print(c);
    }
}
