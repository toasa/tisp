#include "tisp.h"

struct Cell *bool_to_atom(bool b) {
    if (b) {
        return new_cell(CK_T);
    }
    return new_cell(CK_NIL);
}

struct Cell *eval(struct Cell *c);

struct Cell *eval_eq(struct Cell *c) {
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


struct Cell *eval_atom(struct Cell *c) {
    struct Cell *op = eval(c->next);
    if (op->kind == CK_NUM || op->kind == CK_T || op->kind == CK_NIL) {
        return bool_to_atom(true);
    }
    return bool_to_atom(false);
}

struct Cell *eval(struct Cell *c) {
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
        }
    }

    if (c->kind == CK_PRONG) {
        // return c->data;
        return eval(c->data);
    }

    error("cannot evaluate");
    return NULL;
}
