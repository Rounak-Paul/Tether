#pragma once
#include "token.h"

// Forward declarations so Expr and Stmt can reference each other
typedef struct Expr Expr;
typedef struct Stmt Stmt;

typedef struct Param {
    char *name;
    char *type;
} Param;

typedef struct FuncDef {
    char *name;
    Param *params;
    int param_count;
    char *ret_type;
    // body
    Stmt *body;
    int stmt_count;
} FuncDef;

typedef struct VarDecl {
    char *name;
    char *type;
    Expr *init; // expression initializer
} VarDecl;

// Expression AST
typedef enum {
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_IDENT,
    EXPR_BINARY,
    EXPR_CALL,
} ExprKind;

typedef struct Expr {
    ExprKind kind;
    char *text; // for ident or literal
    struct Expr *left;
    struct Expr *right;
    char op[4];
    // call args
    struct Expr **args;
    int arg_count;
} Expr;

// statements
typedef enum { STMT_VARDECL, STMT_RETURN, STMT_EXPR, STMT_BLOCK, STMT_IF, STMT_WHILE, STMT_FOR } StmtKind;

typedef struct Stmt {
    StmtKind kind;
    // var decl
    VarDecl var;
    // return expr
    Expr *expr;
    // expr stmt
    Expr *sexpr;
    // block (or generic body)
    Stmt *stmts;
    int stmt_count;
    // control-flow: condition (if/while/for)
    Expr *cond;
    // if: optional else body
    Stmt *else_stmts;
    int else_stmt_count;
    // for-loop components
    Stmt *for_init; // single-statement init (var decl or expr stmt)
    Expr *for_post; // expression for post step
} Stmt;

typedef struct AST {
    FuncDef *funcs;
    int func_count;
    VarDecl *vars;
    int var_count;
} AST;

AST *ast_create();
void ast_free(AST *a);

Expr *expr_new_int(const char *txt);
Expr *expr_new_float(const char *txt);
Expr *expr_new_ident(const char *txt);
Expr *expr_new_call(const char *name);
Expr *expr_new_binary(Expr *l, const char *op, Expr *r);
void expr_free(Expr *e);
