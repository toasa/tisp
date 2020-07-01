#include "tisp.h"

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
struct Cell *new_cell(enum CellKind kind) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = kind;
    return c;
}

static struct Cell *new_prim_cell(enum PrimKind pk) {
    struct Cell *c = calloc(1, sizeof(struct Cell));
    c->kind = CK_PRIM;
    c->pkind = pk;
    c->str = token->str;
    return c;
}

struct Cell *new_num_cell(int val) {
    struct Cell *c = new_cell(CK_NUM);
    c->val = val;
    return c;
}

struct Cell *new_list_cell(struct Cell *data) {
    struct Cell *c = new_cell(CK_LIST);
    c->data = data;
    return c;
}

static bool is_atom(enum TokenKind tk) {
    return (tk == TK_NUM) || (tk == TK_T)
        || (tk == TK_NIL) || (tk == TK_SYMBOL);
}

struct Cell *gen_cells();

static struct Cell *gen_atom_cell() {
    struct Cell *new;
    if (token->kind == TK_NUM) {
        new = new_num_cell(token->val);
    } else if (token->kind == TK_T) {
        new = new_cell(CK_T);
    } else if (token->kind == TK_NIL) {
        new = new_cell(CK_NIL);
    } else if (token->kind == TK_SYMBOL) {
        new = new_cell(CK_SYMBOL);
        new->str = token->str;
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
            } else if (cur_token_is("+")) {
                new = new_prim_cell(PK_ADD);
            }
            next_token();
        } else if (token->kind == TK_LPARENT) {
            next_token();
            new = gen_list_cells(token);
        } else if (is_atom(token->kind)) {
            new = gen_atom_cell();
            next_token();
        } else {
            new = gen_cells();
        }
        cur->next = new;
        cur = new;
    }

    expect(TK_RPARENT);
    return new_list_cell(head.next->next);
}

struct Cell *gen_cells() {
    struct Cell *cur;
    if (token->kind == TK_LPARENT) {
        next_token();
        cur = gen_list_cells();
    } else if (is_atom(token->kind)) {
        cur = gen_atom_cell();
        next_token();
    } else if (token->kind == TK_QUOTE) {
        next_token();
        struct Cell *quote = new_prim_cell(PK_QUOTE);
        quote->next = gen_cells();
        cur = new_list_cell(quote);
    } else {
        error("unexpected token: %d", token->kind);
    }
    return cur;
}

struct Cell *gen_cell(struct Token *tokens) {
    token = tokens;
    return gen_cells();
}
