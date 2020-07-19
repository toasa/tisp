#include "tisp.h"

struct Env *new_env() {
    return calloc(1, sizeof(struct Env));
}

struct VarNode *new_var_node(struct Cell *var) {
    struct VarNode *v_node = calloc(1, sizeof(struct VarNode));
    v_node->var = var;
    return v_node;
}

struct Cell *look_up_vars(struct VarNode *vars, char *name) {
    struct VarNode *var_iter = vars;
    while (var_iter != NULL) {
        if (equal_strings(var_iter->var->str, name)) {
            return var_iter->var;
        }
        var_iter = var_iter->next;
    }
    return NULL;
}

struct EnvAndCell {
    struct Env *env;
    struct Cell *cell;
};

struct EnvAndCell *look_up(struct Env *env, char *name) {
    struct EnvAndCell *e_c = calloc(1, sizeof(struct EnvAndCell));

    struct Env *env_iter = env;
    while (env_iter != NULL) {
        struct Cell *c = look_up_vars(env_iter->vars, name);
        if (c != NULL) {
            e_c->env = env_iter;
            e_c->cell = c;
            return e_c;
        }
        env_iter = env_iter->up;
    }
    return NULL;
}

// append a var node to head of the linked list.
void add_var(struct Cell *var, struct Env *env) {
    struct VarNode *vn = new_var_node(var);
    vn->next = env->vars;
    env->vars = vn;
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

struct Cell *eval_(struct Cell *c, struct Env *env);

static struct Cell *eval_eq(struct Cell *c, struct Env *env) {
    struct Cell *op1 = eval_(c->next, env);
    struct Cell *op2 = eval_(c->next->next, env);

    if (op1->kind == CK_NUM && op2->kind == CK_NUM) {
        return bool_to_atom(op1->val == op2->val);
    } else if ((op1->kind == CK_T || op1->kind == CK_NIL)
            && (op2->kind == CK_T || op2->kind == CK_NIL)) {
        return bool_to_atom(op1->kind == op2->kind);
    }
    return bool_to_atom(false);
}

static struct Cell *eval_atom(struct Cell *c, struct Env *env) {
    struct Cell *op = eval_(c->next, env);
    if (op->kind == CK_NUM || op->kind == CK_T || op->kind == CK_NIL) {
        return bool_to_atom(true);
    }
    return bool_to_atom(false);
}

struct Cell *eval_car(struct Cell *c, struct Env *env) {
    struct Cell *op = eval_(c->next, env)->data;
    op->next = NULL;
    return op;
}

static struct Cell *eval_cdr(struct Cell *c, struct Env *env) {
    struct Cell *op = eval_(c->next, env);

    // A result of cdr for only one element list is NIL.
    if (op->data->next == NULL) {
        return bool_to_atom(false);
    }

    op->data = op->data->next;
    return op;
}

static struct Cell *eval_cons(struct Cell *c, struct Env *env) {
    struct Cell *op1 = eval_(c->next, env);
    struct Cell *op2 = eval_(c->next->next, env);

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

static struct Cell *eval_cond(struct Cell *c, struct Env *env) {
    struct Cell *c_i = c->next;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i->data, env);
        if (res->kind != CK_NIL) {
            return eval_(c_i->data->next, env);
        }
        c_i = c_i->next;
    }
    return bool_to_atom(false);
}

static struct Cell *eval_append(struct Cell *c, struct Env *env) {
    struct Cell *op1 = eval_(c->next, env);
    struct Cell *op2 = eval_(c->next->next, env);

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

static struct Cell *eval_defun(struct Cell *c, struct Env *env) {
    struct Cell *fn = c->next;
    assert(fn->kind == CK_SYMBOL, "first operand of defun must be symbol");
    add_var(fn, env);
    return fn;
}

static struct Cell *eval_func(struct Cell *fn, struct Cell *call, struct Env *env) {
    struct Env *tmp_env = new_env();
    tmp_env->up = env;
    struct Cell *formal_arg = fn->next->data;
    struct Cell *actual_arg = call->next;
    int args_count = 0;
    while (formal_arg != NULL) {
        struct Cell *bind = new_symbol_cell(formal_arg->str);
        bind->next = eval_(actual_arg, env);

        add_var(bind, tmp_env);

        args_count++;
        formal_arg = formal_arg->next;
        actual_arg = actual_arg->next;
    }

    struct Cell *func_body = fn->next->next;
    struct Cell *res = eval_(func_body, tmp_env);

    return res;
}

static struct Cell *eval_length(struct Cell *c, struct Env *env) {
    struct Cell *list = eval_(c->next, env);
    assert((list->kind == CK_LIST) || (list->kind == CK_NIL), "invalid operand of length");
    if (list->kind == CK_NIL) {
        return new_num_cell(0);
    }
    int count = 0;
    for (struct Cell *elem = list->data; elem != NULL; elem = elem->next) {
        count++;
    }
    return new_num_cell(count);
}

static struct Cell *eval_if(struct Cell *c, struct Env *env) {
    struct Cell *cond = eval_(c->next, env);
    if (cond->kind == CK_NIL) {
        if (c->next->next->next == NULL) {
            return bool_to_atom(false);
        }
        return eval_(c->next->next->next, env);
    }
    return eval_(c->next->next, env);
}

static struct Cell *eval_list(struct Cell *c, struct Env *env) {
    if (c->next == NULL) {
        return bool_to_atom(false);
    }
    struct Cell head;
    struct Cell *cur = calloc(1, sizeof(struct Cell));
    head.next = cur;
    for (struct Cell *c_i = c->next; c_i != NULL; c_i = c_i->next) {
        struct Cell *tmp = eval_(c_i, env);
        cur->next = tmp;
        cur = tmp;
    }
    return new_list_cell(head.next->next);
}

static struct Cell *eval_setq(struct Cell *c, struct Env *env) {
    assert(c->next->kind == CK_SYMBOL, "first operand of setq must be symbol");
    struct Cell *val = c->next;
    struct Cell *evaled_val = eval_(val->next, env);
    val->next = evaled_val;

    // TODO: bug
    //
    // (setq x 10)(defun inc () (setq x (+ x 1)))(inc)(inc)(inc)x
    // => expected 13, but got 10.
    struct EnvAndCell *e_c = look_up(env, val->str);
    if (e_c != NULL) {
        add_var(val, e_c->env);
    } else {
        add_var(val, env);
    }
    return evaled_val;
}

static struct Cell *eval_add(struct Cell *c, struct Env *env) {
    struct Cell *c_i = c->next;
    int sum = 0;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i, env);
        assert(res->kind == CK_NUM, "add operand must be number");
        sum += res->val;
        c_i = c_i->next;
    }
    return new_num_cell(sum);
}

static struct Cell *eval_sub(struct Cell *c, struct Env *env) {
    struct Cell *c_i = c->next;
    int diff = eval_(c_i, env)->val;
    if (c_i->next == NULL) {
        return new_num_cell(-1 * diff);
    }

    c_i = c_i->next;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i, env);
        assert(res->kind == CK_NUM, "add operand must be number");
        diff -= res->val;
        c_i = c_i->next;
    }
    return new_num_cell(diff);
}

static struct Cell *eval_mul(struct Cell *c, struct Env *env) {
    struct Cell *c_i = c->next;
    int mul = 1;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i, env);
        assert(res->kind == CK_NUM, "add operand must be number");
        mul *= res->val;
        c_i = c_i->next;
    }
    return new_num_cell(mul);
}

static struct Cell *eval_div(struct Cell *c, struct Env *env) {
    struct Cell *c_i = c->next;
    int quot = eval_(c_i, env)->val;
    if (c_i->next == NULL) {
        return new_num_cell(1 / quot);
    }

    c_i = c_i->next;
    while (c_i != NULL) {
        struct Cell *res = eval_(c_i, env);
        assert(res->kind == CK_NUM, "add operand must be number");
        quot /= res->val;
        c_i = c_i->next;
    }
    return new_num_cell(quot);
}

static struct Cell *eval_mod(struct Cell *c, struct Env *env) {
    struct Cell *op1 = eval_(c->next, env);
    struct Cell *op2 = eval_(c->next->next, env);
    return new_num_cell(op1->val % op2->val);
}

static struct Cell *eval_lt(struct Cell *c, struct Env *env) {
    assert(c->pkind == PK_LT || c->pkind == PK_GT, "invalid lt kind");
    struct Cell *lhs = eval_(c->next, env);
    struct Cell *rhs = eval_(c->next->next, env);
    if (c->pkind == PK_LT) {
        return bool_to_atom(lhs->val < rhs->val);
    }
    // case of PK_GT
    return bool_to_atom(lhs->val > rhs->val);
}

struct Cell *eval_(struct Cell *c, struct Env *env) {
    // list
    if (c->kind == CK_LIST) {
        // the head of list is function.
        struct Cell *fn = c->data;

        // primitive
        if (fn->kind == CK_PRIM) {
            if (fn->pkind == PK_QUOTE) {
                return fn->next;
            } else if (fn->pkind == PK_EQ) {
                return eval_eq(fn, env);
            } else if (fn->pkind == PK_ATOM) {
                return eval_atom(fn, env);
            } else if (fn->pkind == PK_CAR) {
                return eval_car(fn, env);
            } else if (fn->pkind == PK_CDR) {
                return eval_cdr(fn, env);
            } else if (fn->pkind == PK_CONS) {
                return eval_cons(fn, env);
            } else if (fn->pkind == PK_COND) {
                return eval_cond(fn, env);
            } else if (fn->pkind == PK_APPEND) {
                return eval_append(fn, env);
            } else if (fn->pkind == PK_DEFUN) {
                return eval_defun(fn, env);
            } else if (fn->pkind == PK_LENGTH) {
                return eval_length(fn, env);
            } else if (fn->pkind == PK_IF) {
                return eval_if(fn, env);
            } else if (fn->pkind == PK_LIST) {
                return eval_list(fn, env);
            } else if (fn->pkind == PK_SETQ) {
                return eval_setq(fn, env);
            } else if (fn->pkind == PK_ADD) {
                return eval_add(fn, env);
            } else if (fn->pkind == PK_SUB) {
                return eval_sub(fn, env);
            } else if (fn->pkind == PK_MUL) {
                return eval_mul(fn, env);
            } else if (fn->pkind == PK_DIV) {
                return eval_div(fn, env);
            } else if (fn->pkind == PK_MOD) {
                return eval_mod(fn, env);
            } else if (fn->pkind == PK_LT || fn->pkind == PK_GT) {
                return eval_lt(fn, env);
            }
        }

        if (fn->kind == CK_SYMBOL) {
            struct EnvAndCell *defined_fn = look_up(env, fn->str);
            if (defined_fn != NULL) {
                return eval_func(defined_fn->cell, fn, env);
            }
        }
        error("invalid function called: %s", fn->str);
    }

    // atom
    if (c->kind == CK_NUM || c->kind == CK_T || c->kind == CK_NIL) {
        return c;
    }

    if (c->kind == CK_SYMBOL) {
        struct EnvAndCell *symbol = look_up(env, c->str);
        if (symbol != NULL) {
            return eval_(symbol->cell->next, env);
        }
    }

    error("cannot evaluate");
    return NULL;
}

struct Cell *eval(struct Cell *c, struct Env *g_env) {
    struct Cell *c_iter = c;
    struct Cell *result;
    while (c_iter != NULL) {
        result = eval_(c_iter, g_env);
        c_iter = c_iter->next;
    }
    return result;
}
