#include "tisp.h"

struct Env *env;

void init_env() {
    env = calloc(1, sizeof(struct Env));
};

struct FuncNode *new_func_node(struct Cell *fn) {
    struct FuncNode *f_node = calloc(1, sizeof(struct FuncNode));
    f_node->fn = fn;
    return f_node;
}

struct SymbolNode *new_symbol_node(struct Cell *sn) {
    struct SymbolNode *s_node = calloc(1, sizeof(struct SymbolNode));
    s_node->sy = sn;
    return s_node;
}

void add_new_func(struct FuncNode *fn) {
    fn->next = env->funcs;
    env->funcs = fn;
}

void add_new_symbol(struct SymbolNode *sn) {
    sn->next = env->symbols;
    env->symbols = sn;
}

// Remove n symbols from the head of symbol list.
void remove_symbols(int n) {
    struct SymbolNode *s_node = env->symbols;
    for (int i = 0; i < n; i++) {
        s_node = s_node->next;
    }
    env->symbols = s_node;
}

struct Cell *look_up_symbol(char *name) {
    struct SymbolNode *s_iter = env->symbols;
    while (s_iter != NULL) {
        if (equal_strings(s_iter->sy->str, name)) {
            return s_iter->sy;
        }
        s_iter = s_iter->next;
    }
    return NULL;
}

struct Cell *look_up_func(char *name) {
    struct FuncNode *f_iter = env->funcs;
    while (f_iter != NULL) {
        if (equal_strings(f_iter->fn->str, name)) {
            return f_iter->fn;
        }
        f_iter = f_iter->next;
    }
    return NULL;
}

static struct Cell *bool_to_atom(bool b) {
    if (b) {
        return new_cell(CK_T);
    }
    return new_cell(CK_NIL);
}

static bool is_atom(struct Cell *c) {
    return (c->kind == CK_NUM)
        || (c->kind == CK_T)
        || (c->kind == CK_NIL)
        || (c->kind == CK_SYMBOL);
}

static bool is_list(struct Cell *c) {
    return (c->kind == CK_LIST);
}

struct Cell *eval_(struct Cell *c);

static struct Cell *eval_eq(struct Cell *c) {
    struct Cell *op1 = eval_(c->next);
    struct Cell *op2 = eval_(c->next->next);

    if (op1->kind == CK_NUM && op2->kind == CK_NUM) {
        return bool_to_atom(op1->val == op2->val);
    } else if ((op1->kind == CK_T || op1->kind == CK_NIL)
            && (op2->kind == CK_T || op2->kind == CK_NIL)) {
        return bool_to_atom(op1->kind == op2->kind);
    }
    return bool_to_atom(false);
}

static struct Cell *eval_atom(struct Cell *c) {
    struct Cell *op = eval_(c->next);
    if (op->kind == CK_NUM || op->kind == CK_T || op->kind == CK_NIL) {
        return bool_to_atom(true);
    }
    return bool_to_atom(false);
}

struct Cell *eval_car(struct Cell *c) {
    struct Cell *op = eval_(c->next)->data;
    op->next = NULL;
    return op;
}

static struct Cell *eval_cdr(struct Cell *c) {
    struct Cell *op = eval_(c->next);

    // A result of cdr for only one element list is NIL.
    if (op->data->next == NULL) {
        return bool_to_atom(false);
    }

    op->data = op->data->next;
    return op;
}

static struct Cell *eval_cons(struct Cell *c) {
    struct Cell *op1 = eval_(c->next);
    struct Cell *op2 = eval_(c->next->next);

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
    struct Cell *dot = new_dot_cell(op1, op2);
    return dot;
}

static struct Cell *eval_cond(struct Cell *c) {
    struct Cell *c_i = c->next;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i->data);
        if (res->kind != CK_NIL) {
            return c_i->data->next;
        }
        c_i = c_i->next;
    }
    return bool_to_atom(false);
}

static struct Cell *eval_append(struct Cell *c) {
    struct Cell *op1 = eval_(c->next);
    struct Cell *op2 = eval_(c->next->next);

    assert(op1->kind == CK_LIST, "first operand of append must be list");
    if (is_atom(op2)) {
        return new_dot_cell(op1, op2);
    }
    struct Cell *op1_tail = op1->data;
    while (op1_tail->next != NULL) {
        op1_tail = op1_tail->next;
    }
    op1_tail->next = op2->data;
    return op1;
}

static struct Cell *eval_defun(struct Cell *c) {
    struct Cell *fn = c->next;
    assert(fn->kind == CK_SYMBOL, "first operand of defun must be symbol");
    struct FuncNode *f_node = new_func_node(fn);
    add_new_func(f_node);
    return fn;
}

static struct Cell *eval_func(struct Cell *fn, struct Cell *call) {
    struct Cell *formal_arg = fn->next->data;
    struct Cell *actual_arg = call->next;
    int args_count = 0;
    while (formal_arg != NULL) {
        struct Cell *arg = new_symbol_cell(formal_arg->str);
        arg->val = eval_(actual_arg)->val;
        struct SymbolNode *arg_node = new_symbol_node(arg);
        // tentative addition of argument symbol
        add_new_symbol(arg_node);

        args_count++;
        formal_arg = formal_arg->next;
        actual_arg = actual_arg->next;
    }
    struct Cell *func_body = fn->next->next;
    struct Cell *res = eval_(func_body);

    // remove all tentative added symbols
    remove_symbols(args_count);
    return res;
}

static struct Cell *eval_add(struct Cell *c) {
    struct Cell *c_i = c->next;
    int sum = 0;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i);
        assert(res->kind == CK_NUM, "add operand must be number");
        sum += res->val;
        c_i = c_i->next;
    }
    return new_num_cell(sum);
}

static struct Cell *eval_lt(struct Cell *c) {
    assert(c->pkind == PK_LT || c->pkind == PK_GT, "invalid lt kind");
    struct Cell *lhs = eval_(c->next);
    struct Cell *rhs = eval_(c->next->next);
    if (c->pkind == PK_LT) {
        return bool_to_atom(lhs->val < rhs->val);
    }
    // case of PK_GT
    return bool_to_atom(lhs->val > rhs->val);
}

struct Cell *eval_(struct Cell *c) {
    // list
    if (c->kind == CK_LIST) {
        // the head of list is function.
        struct Cell *fn = c->data;

        // primitive
        if (fn->kind == CK_PRIM) {
            if (fn->pkind == PK_QUOTE) {
                return fn->next;
            } else if (fn->pkind == PK_EQ) {
                return eval_eq(fn);
            } else if (fn->pkind == PK_ATOM) {
                return eval_atom(fn);
            } else if (fn->pkind == PK_CAR) {
                return eval_car(fn);
            } else if (fn->pkind == PK_CDR) {
                return eval_cdr(fn);
            } else if (fn->pkind == PK_CONS) {
                return eval_cons(fn);
            } else if (fn->pkind == PK_COND) {
                return eval_cond(fn);
            } else if (fn->pkind == PK_APPEND) {
                return eval_append(fn);
            } else if (fn->pkind == PK_DEFUN) {
                return eval_defun(fn);
            } else if (fn->pkind == PK_ADD) {
                return eval_add(fn);
            } else if (fn->pkind == PK_LT || fn->pkind == PK_GT) {
                return eval_lt(fn);
            }
        }

        if (fn->kind == CK_SYMBOL) {
            struct Cell *defined_fn = look_up_func(fn->str);
            if (defined_fn != NULL) {
                return eval_func(defined_fn, fn);
            }
        }
        error("invalid function called: %s", fn->str);
    }

    // atom
    if (c->kind == CK_NUM || c->kind == CK_T || c->kind == CK_NIL) {
        return c;
    }

    if (c->kind == CK_SYMBOL) {
        struct Cell *symbol = look_up_symbol(c->str);
        if (symbol != NULL) {
            // TODO: result of symbol evaluation is NUMBER only (currently).
            return new_num_cell(symbol->val);
        }
    }

    error("cannot evaluate");
    return NULL;
}

struct Cell *eval(struct Cell *c) {
    struct Cell *c_iter = c;
    struct Cell *result;
    while (c_iter != NULL) {
        result = eval_(c_iter);
        c_iter = c_iter->next;
    }
    return result;
}
