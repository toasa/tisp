#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// token.c
enum TokenKind {
    TK_NUM,
    TK_LPARENT, // (
    TK_RPARENT, // )
    TK_EOF,
};

struct Token {
    enum TokenKind kind;
    int val; // kind が TK_NUM の場合に使う
    struct Token *next;
};

struct Token *tokenize(char *input);

// cell.c
enum CellKind {
    CK_NUM,
    CK_PRONG,
};

struct Cell {
    enum CellKind kind;
    int val; // Kind が CK_NUM の場合に使う

    bool is_head;

    struct Cell *data; // 入れ子の LIST の場合に使う
    struct Cell *next;
};

struct Cell *gen_cells(struct Token *tokens);

// util.c
bool is_integer(char c);
