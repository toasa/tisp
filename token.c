#include "tisp.h"

struct Token *new_token(struct Token *prev, enum TokenKind kind) {
    struct Token *t = calloc(1, sizeof(struct Token));
    t->kind = kind;
    prev->next = t;
    return t;
}

struct Token *new_num_token(struct Token *prev, int val) {
    struct Token *t = new_token(prev, TK_NUM);
    t->val = val;
    return t;
}

struct Token *tokenize(char *input) {
    struct Token head;
    struct Token *cur = calloc(1, sizeof(struct Token));
    head.next = cur;

    int i = 0;
    while (input[i] != '\0') {
        if (is_integer(input[i])) {
            int n = 0;
            do {
                n = 10 * n + (input[i] - '0');
                i++;
            } while (is_integer(input[i]));
            cur = new_num_token(cur, n);
            continue;
        } else if (input[i] == '(') {
            cur = new_token(cur, TK_LPARENT);
        } else if (input[i] == ')') {
            cur = new_token(cur, TK_RPARENT);
        } else if (input[i] == ' ') {
            // skip
        }
        i++;
    }

    cur = new_token(cur, TK_EOF);
    return head.next->next;
}

