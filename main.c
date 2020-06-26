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

enum TokenKind {
    TK_NUM,
    TK_EOF,
};

struct Token {
    enum TokenKind kind;
    int val;
    struct Token *next;
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

struct Token *new_token(struct Token *prev, enum TokenKind kind) {
    struct Token *t = calloc(1, sizeof(struct Token));
    t->kind = kind;
    prev->next = t;
    return t;
}

struct Token *new_num_token(struct Token *prev, int val) {
    struct Token *t = new_token(prev, TK_NUM);
    t->val = val;
    return t;
}

bool is_integer(char c) {
    return ('0' <= c) && (c <= '9');
}

struct Token *tokenize() {
    struct Token head;
    struct Token *cur = calloc(1, sizeof(struct Token));
    head.next = cur;

    int i = 0;
    while (input[i] != '\0') {
        if (is_integer(input[i])) {
            int n = 0;
            do {
                n = 10 * n + (input[i] - '0');
                i++;
            } while (is_integer(input[i]));
            cur = new_num_token(cur, n);
            continue;
        } else if (input[i] == ' ') {
            // skip
        }
        i++;
    }

    cur = new_token(cur, TK_EOF);
    return head.next->next;
}

struct Cell *gen_cells(struct Token *tokens) {
    struct Token *token = tokens;

    struct Cell *cur;
    while (token->kind != TK_EOF) {
        if (token->kind == TK_NUM) {
            cur = new_num_cell(token->val);
        }
        token = token->next;
    }
    return cur;
}

struct Cell *read() {
    printf("tisp> ");

    // flash input
    for (int i = 0; i < 1024; i++) {
        input[i] = 0;
    }

    fgets(input, 1024, stdin);
    struct Token *tokens = tokenize();
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
