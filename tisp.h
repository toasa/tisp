#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

// token.c
enum TokenKind {
    TK_NUM,
    TK_LPARENT, // (
    TK_RPARENT, // )
    TK_PRIM,
    TK_T,
    TK_NIL,
    TK_SYMBOL,
    TK_EOF,
};

struct Token {
    enum TokenKind kind;
    struct Token *next;

    int val; // kind が TK_NUM の場合に使う

    char *str; // king が TK_PRIM, TK_SYMBOL の場合に使う
};

struct Token *tokenize(char *input);

// cell.c
enum CellKind {
    CK_NUM,
    CK_T,
    CK_NIL,
    CK_PRONG,
    CK_PRIM,
    CK_SYMBOL,
};

enum PrimKind {
    PK_NONE,
    PK_QUOTE,
    PK_EQ,
};

struct Cell {
    enum CellKind kind;
    enum PrimKind pkind;

    int val; // kind が CK_NUM の場合に使う

    bool is_head;

    struct Cell *data; // kind が CK_PRONG の場合に使う
    struct Cell *next;
};

struct Cell *new_cell(enum CellKind kind);
struct Cell *gen_cells(struct Token *tokens);

// util.c
bool is_integer(char c);
bool is_alpha(char c);
bool equal_strings(char *str1, char *str2);
void assert(int result, char *fmt, ...);
void error(char *fmt, ...);
