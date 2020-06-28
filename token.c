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

struct Token *new_str_token(struct Token *prev, enum TokenKind kind, char *str) {
    struct Token *t = new_token(prev, kind);
    t->str = str;
    return t;
}

bool is_primitive(char *str) {
    char *primitives[] = {
        "quote",
        "eq",
        "atom",
        NULL,
    };
    for (int i = 0; primitives[i] != NULL; i++) {
        if (equal_strings(primitives[i], str)) {
            return true;
        }
    }
    return false;
}

char *read_str(char *input, int *i) {
    char *input_org = input + *i;
    int len = 0;
    while (is_alpha(input[*i])) {
        len++;
        (*i)++;
    }
    // create null terminated string
    char *str = calloc(1, sizeof(char) * (len + 1));
    strncpy(str, input_org, len);
    return str;
}

int read_num(char *input, int *i) {
    int n = 0;
    do {
        n = 10 * n + (input[*i] - '0');
        (*i)++;
    } while (is_integer(input[*i]));
    return n;
}

struct Token *tokenize(char *input) {
    struct Token head;
    struct Token *cur = calloc(1, sizeof(struct Token));
    head.next = cur;

    int i = 0;
    while (input[i] != '\0') {
        if (is_integer(input[i])) {
            int num = read_num(input, &i);
            cur = new_num_token(cur, num);
            continue;
        } else if (is_alpha(input[i])) {
            char *str = read_str(input, &i);
            if (is_primitive(str)) {
                cur = new_str_token(cur, TK_PRIM, str);
            } else if (equal_strings(str, "T")) {
                cur = new_str_token(cur, TK_T, str);
            } else if (equal_strings(str, "NIL")) {
                cur = new_str_token(cur, TK_NIL, str);
            } else {
                cur = new_str_token(cur, TK_SYMBOL, str);
            }
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

