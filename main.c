#include "tisp.h"

static char input[1024];

struct Cell *read() {
    printf("tisp> ");

    // flash input
    for (int i = 0; i < 1024; i++) {
        input[i] = 0;
    }

    fgets(input, 1024, stdin);
    struct Token *tokens = tokenize(input);
    return gen_cells(tokens);
}

struct Cell *read_from_stdin(char *input) {
    struct Token *tokens = tokenize(input);
    return gen_cells(tokens);
}

struct Cell *eval(struct Cell *c) {
    // atom
    if ((c->kind == CK_NUM || c->kind == CK_T || c->kind == CK_NIL)
      && c->next == NULL) {
        return c;
    }

    if (c->kind == CK_PRIM) {
        if (c->pkind == PK_QUOTE) {
            return eval(c->next);
        }
    }

    if (c->kind == CK_PRONG) {
        return c->data;
    }

    error("cannot evaluate");
    return NULL;
}

void print_atom(struct Cell *c) {
    bool is_last = (c->next == NULL);

    if (c->kind == CK_NUM) {
        printf("%d", c->val);
    } else if (c->kind == CK_T) {
        printf("T");
    } else if (c->kind == CK_NIL) {
        printf("NIL");
    }

    if (!is_last) {
        printf(" ");
    }
}

void print_list(struct Cell *c) {
    printf("(");
    for (struct Cell *c_i = c; c_i != NULL; c_i = c_i->next) {
        if (c_i->kind == CK_NUM || c_i->kind == CK_T || c_i->kind == CK_NIL) {
            print_atom(c_i);
        } else if (c_i->kind == CK_PRONG) {
            print_list(c_i->data);
            if (c_i->next != NULL) {
                printf(" ");
            }
        }
    }
    printf(")");
}

void print(struct Cell *c) {
    if (c->is_head) {
        print_list(c);
        printf("\n");
    } else if (c->kind == CK_PRONG) {
        print(c->data);
    } else {
        print_atom(c);
        printf("\n");
    }
}

void repl() {
    while (1) {
        struct Cell *c = read();
        c = eval(c);
        print(c);
    }
}

int main(int argc, char **argv) {
    if (argc == 2) {
        // for unittest
        struct Cell *c = read_from_stdin(argv[1]);
        c = eval(c);
        print(c);
    } else {
        repl();
    }
}
