#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static char input[1024];

enum Kind {
    NUMBER,
};

struct Cell {
    enum Kind kind;
    int val; // Kind が NUMBER の場合に使う
};

struct Cell *new_cell(enum Kind kind) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = kind;
    return c;
}

struct Cell *new_num_cell(int val) {
    struct Cell *c = new_cell(NUMBER);
    c->val = val;
    return c;
}

bool is_integer(char c) {
    return ('0' <= c) && (c <= '9');
}

int read_integer() {
    char *n_i = input;
    int n = 0;
    while (is_integer(*n_i)) {
        n = 10 * n + (*n_i - '0');
        n_i++;
    }

    return n;
}

struct Cell *read() {
    printf("tisp> ");

    // flash input
    for (int i = 0; i < 1024; i++) {
        input[i] = 0;
    }

    fgets(input, 1024, stdin);

    int num = read_integer();
    return new_num_cell(num);
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
