#include "tisp.h"

struct Cell *new_cell(enum CellKind kind) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = kind;
    return c;
}

struct Cell *new_prim_cell(enum PrimKind pk) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = CK_PRIM;
    c->pkind = pk;
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

bool is_atom(enum TokenKind tk) {
    return (tk == TK_NUM) || (tk == TK_T) || (tk == TK_NIL);
}

struct Cell *gen_atom_cell() {
    struct Cell *new;
    if (token->kind == TK_NUM) {
        new = new_num_cell(token->val);
        next_token();
    } else if (token->kind == TK_T) {
        new = new_cell(CK_T);
        next_token();
    } else if (token->kind == TK_NIL) {
        new = new_cell(CK_NIL);
        next_token();
    }
    return new;
}

struct Cell *gen_list_cells() {
    struct Cell head_c;
    struct Cell *cur = calloc(1, sizeof(struct Cell));
    head_c.next = cur;
    while (token->kind != TK_RPARENT) {
        struct Cell *new;
        if (token->kind == TK_PRIM) {
            if (equal_strings(token->str, "quote")) {
                new = new_prim_cell(PK_QUOTE);
            }
            next_token();
        } else if (token->kind == TK_LPARENT) {
            next_token();
            new = new_cell(CK_PRONG);
            new->data = gen_list_cells(token);
        } else if (is_atom(token->kind)) {
            new = gen_atom_cell();
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
    if (token->kind == TK_LPARENT) {
        next_token();
        cur = gen_list_cells();
    } else if (is_atom(token->kind)) {
        cur = gen_atom_cell();
    }
    return cur;
}
