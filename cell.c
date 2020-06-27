#include "tisp.h"

struct Cell *new_cell(enum CellKind kind) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = kind;
    return c;
}

struct Cell *new_num_cell(int val) {
    struct Cell *c = new_cell(CK_NUM);
    c->val = val;
    return c;
}

struct Token *token;

void next_token() {
    token = token->next;
}

void expect(enum TokenKind tk) {
    if (token->kind != tk) {
        error("expect %d but got %d\n", tk, token->kind);
    }
    next_token();
}

struct Cell *gen_list_cells() {
    struct Cell head_c;
    struct Cell *cur = calloc(1, sizeof(struct Cell));
    head_c.next = cur;
    while (token->kind != TK_RPARENT) {
        struct Cell *new;
        if (token->kind == TK_LPARENT) {
            next_token();
            new = new_cell(CK_PRONG);
            new->data = gen_list_cells(token);
        } else if (token->kind == TK_NUM) {
            new = new_num_cell(token->val);
            next_token();
        }
        cur->next = new;
        cur = new;
    }

    expect(TK_RPARENT);

    head_c.next->next->is_head = true;
    return head_c.next->next;
}

struct Cell *gen_cells(struct Token *tokens) {
    token = tokens;

    struct Cell *cur;
    if (token->kind == TK_NUM) {
        cur = new_num_cell(token->val);
        next_token();
    } else if (token->kind == TK_LPARENT) {
        next_token();
        cur = gen_list_cells();
    }
    return cur;
}
