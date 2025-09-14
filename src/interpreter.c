#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "interpreter.h"

// Very small interpreter: supports i32 integers only for now, variables stored per-function

typedef struct VarSlot {
    char *name;
    int32_t val;
} VarSlot;

// global locals for the currently-executing function (simple approach)
static VarSlot *g_locals = NULL;
static int g_local_cnt = 0;

// Forward
static int32_t eval_expr(Expr *e, FuncDef *current, int *ret_flag, int32_t *ret_val, AST *ast);

static VarSlot *alloc_slot(const char *name) {
    for (int i = 0; i < g_local_cnt; ++i) if (strcmp(g_locals[i].name, name) == 0) return &g_locals[i];
    g_locals = realloc(g_locals, sizeof(VarSlot)*(g_local_cnt+1));
    g_locals[g_local_cnt].name = strdup(name);
    g_locals[g_local_cnt].val = 0;
    g_local_cnt = g_local_cnt + 1;
    return &g_locals[g_local_cnt-1];
}

static FuncDef *find_func(AST *ast, const char *name) {
    for (int i = 0; i < ast->func_count; ++i) if (strcmp(ast->funcs[i].name, name) == 0) return &ast->funcs[i];
    return NULL;
}

static int32_t call_func(AST *ast, const char *name, Expr **args, int arg_count, int *ret_flag_out, int32_t *ret_val_out) {
    FuncDef *f = find_func(ast, name);
    if (!f) { fprintf(stderr, "runtime error: function '%s' not found\n", name); *ret_flag_out = 1; *ret_val_out = 0; return 0; }

    // reset global locals for this call
    if (g_locals) {
        for (int i = 0; i < g_local_cnt; ++i) free(g_locals[i].name);
        free(g_locals);
        g_locals = NULL;
        g_local_cnt = 0;
    }

    // initialize params (evaluate args if provided)
    for (int i = 0; i < f->param_count; ++i) {
        VarSlot *s = alloc_slot(f->params[i].name);
        int df = 0;
        int32_t v = 0;
        if (args && i < arg_count && args[i]) v = eval_expr(args[i], f, &df, NULL, ast);
        s->val = v;
    }

    // execute function body
    for (int si = 0; si < f->stmt_count; ++si) {
        Stmt *st = &f->body[si];
        if (st->kind == STMT_VARDECL) {
            VarSlot *s = alloc_slot(st->var.name);
            int df = 0; int32_t v = eval_expr(st->var.init, f, &df, NULL, ast);
            s->val = v;
        } else if (st->kind == STMT_EXPR) {
            int df = 0; (void)eval_expr(st->sexpr, f, &df, NULL, ast);
        } else if (st->kind == STMT_RETURN) {
            int df = 0; int32_t v = eval_expr(st->expr, f, &df, NULL, ast);
            *ret_flag_out = 1; *ret_val_out = v;
            for (int i = 0; i < g_local_cnt; ++i) free(g_locals[i].name);
            free(g_locals); g_locals = NULL; g_local_cnt = 0;
            return v;
        } else if (st->kind == STMT_IF) {
            int df = 0; int32_t cv = eval_expr(st->cond, f, &df, NULL, ast);
            if (cv) {
                for (int k = 0; k < st->stmt_count; ++k) {
                    Stmt *ns = &st->stmts[k];
                    if (ns->kind == STMT_RETURN) {
                        int df2 = 0; int32_t v = eval_expr(ns->expr, f, &df2, NULL, ast);
                        *ret_flag_out = 1; *ret_val_out = v;
                        for (int i = 0; i < g_local_cnt; ++i) free(g_locals[i].name);
                        free(g_locals); g_locals = NULL; g_local_cnt = 0;
                        return v;
                    } else if (ns->kind == STMT_VARDECL) {
                        VarSlot *s = alloc_slot(ns->var.name);
                        int df2 = 0; s->val = eval_expr(ns->var.init, f, &df2, NULL, ast);
                    } else if (ns->kind == STMT_EXPR) {
                        int df2 = 0; eval_expr(ns->sexpr, f, &df2, NULL, ast);
                    }
                }
            } else {
                for (int ei = 0; ei < st->else_stmt_count; ++ei) {
                    Stmt *es = &st->else_stmts[ei];
                    if (es->kind == STMT_RETURN) {
                        int df2 = 0; int32_t v = eval_expr(es->expr, f, &df2, NULL, ast);
                        *ret_flag_out = 1; *ret_val_out = v;
                        for (int i = 0; i < g_local_cnt; ++i) free(g_locals[i].name);
                        free(g_locals); g_locals = NULL; g_local_cnt = 0;
                        return v;
                    } else if (es->kind == STMT_VARDECL) {
                        VarSlot *s = alloc_slot(es->var.name);
                        int df2 = 0; s->val = eval_expr(es->var.init, f, &df2, NULL, ast);
                    } else if (es->kind == STMT_EXPR) {
                        int df2 = 0; eval_expr(es->sexpr, f, &df2, NULL, ast);
                    }
                }
            }
        }
        // while/for not implemented in interpreter for simplicity
    }

    for (int i = 0; i < g_local_cnt; ++i) free(g_locals[i].name);
    free(g_locals); g_locals = NULL; g_local_cnt = 0;
    *ret_flag_out = 0; *ret_val_out = 0; return 0;
}

static int32_t eval_expr(Expr *e, FuncDef *current, int *ret_flag, int32_t *ret_val, AST *ast) {
    if (!e) return 0;
    if (e->kind == EXPR_INT) {
        return atoi(e->text);
    } else if (e->kind == EXPR_IDENT) {
        for (int i = 0; i < g_local_cnt; ++i) if (strcmp(g_locals[i].name, e->text) == 0) return g_locals[i].val;
        return 0;
    } else if (e->kind == EXPR_BINARY) {
        int df = 0; int32_t l = eval_expr(e->left, current, &df, NULL, ast);
        int df2 = 0; int32_t r = eval_expr(e->right, current, &df2, NULL, ast);
        if (strcmp(e->op, "+") == 0) return l + r;
        if (strcmp(e->op, "-") == 0) return l - r;
        if (strcmp(e->op, "*") == 0) return l * r;
        if (strcmp(e->op, "/") == 0) return r == 0 ? 0 : l / r;
        return 0;
    } else if (e->kind == EXPR_CALL) {
        if (strcmp(e->text, "print") == 0) {
            for (int i = 0; i < e->arg_count; ++i) {
                int df = 0; int32_t v = eval_expr(e->args[i], current, &df, NULL, ast);
                printf("%d\n", v);
            }
            return 0;
        }
        int rf = 0; int32_t rv = 0;
        call_func(ast, e->text, e->args, e->arg_count, &rf, &rv);
        if (ret_flag) *ret_flag = rf; if (ret_val) *ret_val = rv;
        return rv;
    }
    return 0;
}

int interp_execute(AST *ast) {
    FuncDef *mainf = find_func(ast, "main");
    if (!mainf) { fprintf(stderr, "runtime: no main function found\n"); return 2; }
    int rf=0; int32_t rv=0; call_func(ast, "main", NULL, 0, &rf, &rv);
    return (int)rv;
}
