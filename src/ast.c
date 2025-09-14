#include <stdlib.h>
#include <string.h>
#include "../include/ast.h"

AST *ast_create() {
    AST *a = calloc(1, sizeof(AST));
    return a;
}

void ast_free(AST *a) {
    if (!a) return;
    for (int i = 0; i < a->func_count; ++i) {
        free(a->funcs[i].name);
        for (int j = 0; j < a->funcs[i].param_count; ++j) {
            free(a->funcs[i].params[j].name);
            free(a->funcs[i].params[j].type);
        }
        free(a->funcs[i].params);
        free(a->funcs[i].ret_type);
        // free statements
        for (int s = 0; s < a->funcs[i].stmt_count; ++s) {
            Stmt *st = &a->funcs[i].body[s];
            if (st->kind == STMT_VARDECL) {
                free(st->var.name);
                free(st->var.type);
                expr_free(st->var.init);
            } else if (st->kind == STMT_RETURN) {
                expr_free(st->expr);
            } else if (st->kind == STMT_EXPR) {
                expr_free(st->sexpr);
            } else if (st->kind == STMT_BLOCK) {
                // free nested statements
                for (int k = 0; k < st->stmt_count; ++k) {
                    Stmt *ns = &st->stmts[k];
                    // recurse by freeing fields similar to above
                    if (ns->kind == STMT_VARDECL) {
                        free(ns->var.name);
                        free(ns->var.type);
                        expr_free(ns->var.init);
                    } else if (ns->kind == STMT_RETURN) expr_free(ns->expr);
                    else if (ns->kind == STMT_EXPR) expr_free(ns->sexpr);
                    else if (ns->kind == STMT_IF || ns->kind == STMT_WHILE || ns->kind == STMT_FOR) {
                        expr_free(ns->cond);
                        if (ns->for_init) {
                            // free single init stmt
                            if (ns->for_init->kind == STMT_VARDECL) {
                                free(ns->for_init->var.name);
                                free(ns->for_init->var.type);
                                expr_free(ns->for_init->var.init);
                            } else if (ns->for_init->kind == STMT_EXPR) expr_free(ns->for_init->sexpr);
                            free(ns->for_init);
                        }
                        expr_free(ns->for_post);
                        if (ns->else_stmts) {
                            for (int ei = 0; ei < ns->else_stmt_count; ++ei) {
                                Stmt *es = &ns->else_stmts[ei];
                                if (es->kind == STMT_VARDECL) { free(es->var.name); free(es->var.type); expr_free(es->var.init); }
                                else if (es->kind == STMT_RETURN) expr_free(es->expr);
                                else if (es->kind == STMT_EXPR) expr_free(es->sexpr);
                            }
                            free(ns->else_stmts);
                        }
                    }
                }
                free(st->stmts);
            } else if (st->kind == STMT_IF || st->kind == STMT_WHILE || st->kind == STMT_FOR) {
                expr_free(st->cond);
                if (st->for_init) {
                    if (st->for_init->kind == STMT_VARDECL) { free(st->for_init->var.name); free(st->for_init->var.type); expr_free(st->for_init->var.init); }
                    else if (st->for_init->kind == STMT_EXPR) expr_free(st->for_init->sexpr);
                    free(st->for_init);
                }
                expr_free(st->for_post);
                if (st->else_stmts) {
                    for (int ei = 0; ei < st->else_stmt_count; ++ei) {
                        Stmt *es = &st->else_stmts[ei];
                        if (es->kind == STMT_VARDECL) { free(es->var.name); free(es->var.type); expr_free(es->var.init); }
                        else if (es->kind == STMT_RETURN) expr_free(es->expr);
                        else if (es->kind == STMT_EXPR) expr_free(es->sexpr);
                    }
                    free(st->else_stmts);
                }
            }
        }
        free(a->funcs[i].body);
    }
    free(a->funcs);
    for (int i = 0; i < a->var_count; ++i) {
        free(a->vars[i].name);
        free(a->vars[i].type);
        free(a->vars[i].init);
    }
    free(a->vars);
    // TODO: free expression nodes if added to AST
    free(a);
}

Expr *expr_new_int(const char *txt) {
    Expr *e = calloc(1, sizeof(Expr));
    e->kind = EXPR_INT;
    e->text = strdup(txt);
    return e;
}

Expr *expr_new_float(const char *txt) {
    Expr *e = calloc(1, sizeof(Expr));
    e->kind = EXPR_FLOAT;
    e->text = strdup(txt);
    return e;
}

Expr *expr_new_ident(const char *txt) {
    Expr *e = calloc(1, sizeof(Expr));
    e->kind = EXPR_IDENT;
    e->text = strdup(txt);
    e->args = NULL;
    e->arg_count = 0;
    return e;
}

Expr *expr_new_call(const char *name) {
    Expr *e = calloc(1, sizeof(Expr));
    e->kind = EXPR_CALL;
    e->text = strdup(name);
    e->args = NULL;
    e->arg_count = 0;
    return e;
}

Expr *expr_new_binary(Expr *l, const char *op, Expr *r) {
    Expr *e = calloc(1, sizeof(Expr));
    e->kind = EXPR_BINARY;
    e->left = l;
    e->right = r;
    strncpy(e->op, op, sizeof(e->op)-1);
    e->args = NULL;
    e->arg_count = 0;
    return e;
}

void expr_free(Expr *e) {
    if (!e) return;
    if (e->text) free(e->text);
    expr_free(e->left);
    expr_free(e->right);
    if (e->kind == EXPR_CALL) {
        for (int i = 0; i < e->arg_count; ++i) expr_free(e->args[i]);
        free(e->args);
    }
    free(e);
}
