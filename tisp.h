#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// token.c
enum TokenKind {
    TK_NUM,
    TK_EOF,
};

struct Token {
    enum TokenKind kind;
    int val;
    struct Token *next;
};

struct Token *tokenize(char *input);

// cell.c
enum Kind {
    NUMBER,
};

struct Cell {
    enum Kind kind;
    int val; // Kind が NUMBER の場合に使う
};

struct Cell *gen_cells(struct Token *tokens);

// util.c
bool is_integer(char c);
