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
    return gen_cell(tokens);
}

struct Cell *read_from_stdin(char *input) {
    struct Token *tokens = tokenize(input);
    return gen_cell(tokens);
}

static void print_cell(struct Cell *c);

static void print_atom(struct Cell *c) {
    if (c->kind == CK_NUM) {
        printf("%d", c->val);
    } else if (c->kind == CK_T) {
        printf("T");
    } else if (c->kind == CK_NIL) {
        printf("NIL");
    } else if (c->kind == CK_SYMBOL) {
        printf("%s", c->str);
    }
}

static void print_list(struct Cell *c) {
    printf("(");
    for (struct Cell *c_i = c; c_i != NULL; c_i = c_i->next) {
        print_cell(c_i);
        if (c_i->next != NULL) {
            printf(" ");
        }
    }
    printf(")");
}

static void print_dot(struct Cell *c) {
    printf("(");
    print_cell(c->car);
    printf(" . ");
    print_cell(c->cdr);
    printf(")");
}

static void print_cell(struct Cell *c) {
    if (c->kind == CK_LIST) {
        if (c->data->pkind == PK_QUOTE) {
            printf("'");
            print_cell(c->data->next);
            return;
        }
        print_list(c->data);
    } else if (c->kind == CK_DOT) {
        print_dot(c);
    } else if (c->kind == CK_PRIM) {
        printf("%s", c->str);
    } else {
        print_atom(c);
    }
}

static void print(struct Cell *c) {
    print_cell(c);
    printf("\n");
}

static void repl() {
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
