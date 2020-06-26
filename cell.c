#include "tisp.h"

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
