#include "tisp.h"

struct Cell *new_cell(enum CellKind kind) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = kind;
    return c;
}

static struct Cell *new_prim_cell(enum PrimKind pk) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = CK_PRIM;
    c->pkind = pk;
    return c;
}

static struct Cell *new_num_cell(int val) {
    struct Cell *c = new_cell(CK_NUM);
    c->val = val;
    return c;
}

struct Cell *new_list_cell(struct Cell *data) {
    struct Cell *c = new_cell(CK_LIST);
    c->data = data;
    return c;
}

struct Token *token;

static void next_token() {
    token = token->next;
}

static bool cur_token_is(char *str) {
    return equal_strings(token->str, str);
}

static void expect(enum TokenKind tk) {
    if (token->kind != tk) {
        error("expect %d but got %d\n", tk, token->kind);
    }
    next_token();
}

static bool is_atom(enum TokenKind tk) {
    return (tk == TK_NUM) || (tk == TK_T) || (tk == TK_NIL);
}

static struct Cell *gen_atom_cell() {
    struct Cell *new;
    if (token->kind == TK_NUM) {
        new = new_num_cell(token->val);
    } else if (token->kind == TK_T) {
        new = new_cell(CK_T);
    } else if (token->kind == TK_NIL) {
        new = new_cell(CK_NIL);
    }
    return new;
}

static struct Cell *gen_list_cells() {
    struct Cell head;
    struct Cell *cur = calloc(1, sizeof(struct Cell));
    head.next = cur;

    // The '()' means 'NIL'.
    if (token->kind == TK_RPARENT) {
        next_token();
        return new_cell(CK_NIL);
    }

    while (token->kind != TK_RPARENT) {
        struct Cell *new;
        if (token->kind == TK_PRIM) {
            if (cur_token_is("quote")) {
                new = new_prim_cell(PK_QUOTE);
            } else if (cur_token_is("eq")) {
                new = new_prim_cell(PK_EQ);
            } else if (cur_token_is("atom")) {
                new = new_prim_cell(PK_ATOM);
            } else if (cur_token_is("car")) {
                new = new_prim_cell(PK_CAR);
            } else if (cur_token_is("cdr")) {
                new = new_prim_cell(PK_CDR);
            } else if (cur_token_is("cons")) {
                new = new_prim_cell(PK_CONS);
            } else if (cur_token_is("cond")) {
                new = new_prim_cell(PK_COND);
            }
            next_token();
        } else if (token->kind == TK_LPARENT) {
            next_token();
            new = gen_list_cells(token);
        } else if (is_atom(token->kind)) {
            new = gen_atom_cell();
            next_token();
        }
        cur->next = new;
        cur = new;
    }

    expect(TK_RPARENT);
    return new_list_cell(head.next->next);
}

struct Cell *gen_cells(struct Token *tokens) {
    token = tokens;

    struct Cell *cur;
    if (token->kind == TK_LPARENT) {
        next_token();
        cur = gen_list_cells();
    } else if (is_atom(token->kind)) {
        cur = gen_atom_cell();
    } else {
        error("unexpected token: %d", token->kind);
    }
    return cur;
}
