#include "tisp.h"

static struct Cell *bool_to_atom(bool b) {
    if (b) {
        return new_cell(CK_T);
    }
    return new_cell(CK_NIL);
}

static bool is_atom(struct Cell *c) {
    return (c->kind == CK_NUM)
        || (c->kind == CK_T)
        || (c->kind == CK_NIL);
}

static bool is_list(struct Cell *c) {
    return (c->kind == CK_LIST);
}

struct Cell *eval(struct Cell *c);

static struct Cell *eval_eq(struct Cell *c) {
    struct Cell *op1 = eval(c->next);
    struct Cell *op2 = eval(c->next->next);

    if (op1->kind == CK_NUM && op2->kind == CK_NUM) {
        return bool_to_atom(op1->val == op2->val);
    } else if ((op1->kind == CK_T || op1->kind == CK_NIL)
            && (op2->kind == CK_T || op2->kind == CK_NIL)) {
        return bool_to_atom(op1->kind == op2->kind);
    }
    return bool_to_atom(false);
}

static struct Cell *eval_atom(struct Cell *c) {
    struct Cell *op = eval(c->next);
    if (op->kind == CK_NUM || op->kind == CK_T || op->kind == CK_NIL) {
        return bool_to_atom(true);
    }
    return bool_to_atom(false);
}

struct Cell *eval_car(struct Cell *c) {
    struct Cell *op = eval(c->next)->data;
    op->next = NULL;
    return op;
}

static struct Cell *eval_cdr(struct Cell *c) {
    struct Cell *op = eval(c->next);

    // A result of cdr for only one element list is NIL.
    if (op->data->next == NULL) {
        return bool_to_atom(false);
    }

    op->data = op->data->next;
    return op;
}

static struct Cell *eval_cons(struct Cell *c) {
    struct Cell *op1 = eval(c->next);
    struct Cell *op2 = eval(c->next->next);

    if (op2->kind == CK_NIL) {
        op2 = new_list_cell(NULL);
    }

    if (is_atom(op1) && is_list(op2)) {
        op1->next = op2->data;
        op2->data = op1;
        return op2;
    } else if (is_list(op1) && is_list(op2)) {
        op1->next = op2->data;
        return new_list_cell(op1);
    }

    // dotted pair
    struct Cell *dot = new_cell(CK_DOT);
    dot->car = op1;
    dot->cdr = op2;
    return dot;
}

struct Cell *eval(struct Cell *c) {
    // list
    if (c->kind == CK_LIST) {
        return eval(c->data);
    }

    // atom
    if (c->kind == CK_NUM || c->kind == CK_T || c->kind == CK_NIL) {
        return c;
    }
    
    // primitive
    if (c->kind == CK_PRIM) {
        if (c->pkind == PK_QUOTE) {
            return c->next;
        } else if (c->pkind == PK_EQ) {
            return eval_eq(c);
        } else if (c->pkind == PK_ATOM) {
            return eval_atom(c);
        } else if (c->pkind == PK_CAR) {
            return eval_car(c);
        } else if (c->pkind == PK_CDR) {
            return eval_cdr(c);
        } else if (c->pkind == PK_CONS) {
            return eval_cons(c);
        }
    }

    error("cannot evaluate");
    return NULL;
}
