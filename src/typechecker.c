#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/ast.h"

extern AST *parser_parse_ast(const char *src);

// Simple symbol table
typedef struct Sym {
    char *name;
    char *type;
} Sym;

typedef struct Scope {
    Sym *syms;
    int count;
    struct Scope *parent;
} Scope;

static Scope *scope_create(Scope *parent) {
    Scope *s = calloc(1, sizeof(Scope));
    s->parent = parent;
    s->syms = NULL;
    s->count = 0;
    return s;
}

static void scope_free(Scope *s) {
    if (!s) return;
    for (int i = 0; i < s->count; ++i) { free(s->syms[i].name); free(s->syms[i].type); }
    free(s->syms);
    free(s);
}

static Scope *scope_push(Scope *parent) {
    return scope_create(parent);
}

static Scope *scope_pop(Scope *s) {
    if (!s) return NULL;
    Scope *p = s->parent;
    scope_free(s);
    return p;
}

static void scope_put(Scope *s, const char *name, const char *type) {
    s->syms = realloc(s->syms, sizeof(Sym)*(s->count+1));
    s->syms[s->count].name = strdup(name);
    s->syms[s->count].type = strdup(type);
    s->count++;
}

// search current scope and parents for a name
static const char *scope_get(Scope *s, const char *name) {
    Scope *cur = s;
    while (cur) {
        for (int i = cur->count-1; i >= 0; --i) {
            if (strcmp(cur->syms[i].name, name) == 0) return cur->syms[i].type;
        }
        cur = cur->parent;
    }
    return NULL;
}
// forward decl to access current scope
static Scope *CUR_SCOPE = NULL;

// Note: `type_of_expr` was removed because `check_expr` provides expression
// type checking and reporting in a single place. Keeping both led to an
// unused static-function warning under -Wextra.

// Function signature table for call validation
typedef struct {
    char *name;
    char **param_types;
    int param_count;
    char *ret_type;
} FuncSig;

static FuncSig *g_sigs = NULL;
static int g_sig_count = 0;

static void build_func_sigs(AST *ast) {
    for (int i = 0; i < ast->func_count; ++i) {
        FuncDef *f = &ast->funcs[i];
        g_sigs = realloc(g_sigs, sizeof(FuncSig)*(g_sig_count+1));
        g_sigs[g_sig_count].name = strdup(f->name);
        g_sigs[g_sig_count].param_count = f->param_count;
        g_sigs[g_sig_count].param_types = NULL;
        for (int j = 0; j < f->param_count; ++j) {
            g_sigs[g_sig_count].param_types = realloc(g_sigs[g_sig_count].param_types, sizeof(char*)*(j+1));
            g_sigs[g_sig_count].param_types[j] = strdup(f->params[j].type);
        }
        g_sigs[g_sig_count].ret_type = f->ret_type ? strdup(f->ret_type) : NULL;
        g_sig_count++;
    }
}

static FuncSig *find_sig(const char *name) {
    for (int i = 0; i < g_sig_count; ++i) if (strcmp(g_sigs[i].name, name) == 0) return &g_sigs[i];
    return NULL;
}

static void free_func_sigs() {
    for (int i = 0; i < g_sig_count; ++i) {
        free(g_sigs[i].name);
        for (int j = 0; j < g_sigs[i].param_count; ++j) free(g_sigs[i].param_types[j]);
        free(g_sigs[i].param_types);
        if (g_sigs[i].ret_type) free(g_sigs[i].ret_type);
    }
    free(g_sigs);
    g_sigs = NULL; g_sig_count = 0;
}

// Recursively check an expression; returns its type and reports errors through rc
static const char *check_expr(Expr *e, FuncDef *fdef, AST *ast, const char *path, int *rc) {
    if (!e) return "unknown";
    switch (e->kind) {
        case EXPR_INT: return "i32";
        case EXPR_FLOAT: return "f32";
        case EXPR_IDENT: {
            const char *t = NULL;
            if (CUR_SCOPE) t = scope_get(CUR_SCOPE, e->text);
            if (t) return t;
            // might be a constant or global - check top-level vars
            for (int i = 0; i < ast->var_count; ++i) if (strcmp(ast->vars[i].name, e->text) == 0) return ast->vars[i].type;
            return "unknown";
        }
        case EXPR_BINARY: {
            const char *lt = check_expr(e->left, fdef, ast, path, rc);
            const char *rt = check_expr(e->right, fdef, ast, path, rc);
            if (strcmp(lt, rt) == 0) return lt;
            return "mismatch";
        }
        case EXPR_CALL: {
            FuncSig *sig = find_sig(e->text);
            // check each arg
            for (int i = 0; i < e->arg_count; ++i) {
                const char *at = check_expr(e->args[i], fdef, ast, path, rc);
                if (sig) {
                    if (i >= sig->param_count) {
                        printf("%s: CALL_ARG_COUNT_MISMATCH: function %s expected %d args but got %d\n", path, e->text, sig->param_count, e->arg_count);
                        *rc = 1;
                        return "mismatch";
                    }
                    const char *pt = sig->param_types[i];
                    if (strcmp(at, "unknown") == 0) {
                        // can't infer arg type; skip
                    } else if (strcmp(at, pt) != 0) {
                        printf("%s: CALL_ARG_TYPE_MISMATCH: in call to %s, param %d expected %s but found %s\n", path, e->text, i, pt, at);
                        *rc = 1;
                    }
                }
            }
            if (sig) return sig->ret_type ? sig->ret_type : "unknown";
            return "unknown";
        }
        default: return "unknown";
    }
}

int typecheck_file(const char *path) {
    // load file
    FILE *f = fopen(path, "r");
    if (!f) { perror("fopen"); return 2; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);

    AST *ast = parser_parse_ast(buf);
    int rc = 0;


    // build function signature table
    build_func_sigs(ast);

    // check top-level var inits
    for (int i = 0; i < ast->var_count; ++i) {
        VarDecl *v = &ast->vars[i];
        const char *et = check_expr(v->init, NULL, ast, path, &rc);
        if (strcmp(et, "mismatch") == 0) {
            printf("%s: TYPE_MISMATCH: binary operands have different types\n", path);
            rc = 1;
        } else if (strcmp(et, "unknown") == 0) {
            // can't deduce; assume OK for prototype
        } else {
            if (strcmp(et, v->type) != 0) {
                printf("%s: TYPE_MISMATCH: expected %s but found %s in assignment to %s\n", path, v->type, et, v->name);
                rc = 1;
            }
        }
    }

    // function param checks for generics and body checks
    for (int i = 0; i < ast->func_count; ++i) {
        FuncDef *fdef = &ast->funcs[i];
        for (int j = 0; j < fdef->param_count; ++j) {
            if (strstr(fdef->params[j].type, "Array[T]") != NULL) {
                printf("%s: KERNEL_INVALID_TYPE: generic type used in kernel param\n", path);
                rc = 1;
            }
        }
        // create scope for function and insert params (use proper stack)
        Scope *scope = scope_push(NULL);
        CUR_SCOPE = scope;
        for (int j = 0; j < fdef->param_count; ++j) scope_put(scope, fdef->params[j].name, fdef->params[j].type);
        // walk statements
        for (int s = 0; s < fdef->stmt_count; ++s) {
            Stmt *st = &fdef->body[s];
            if (st->kind == STMT_BLOCK) {
                // push new nested scope (child of current)
                Scope *inner = scope_push(CUR_SCOPE);
                CUR_SCOPE = inner;
                for (int k = 0; k < st->stmt_count; ++k) {
                    Stmt *ns = &st->stmts[k];
                    if (ns->kind == STMT_VARDECL) {
                        const char *et = check_expr(ns->var.init, fdef, ast, path, &rc);
                        if (strcmp(et, "mismatch") == 0) { printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name); rc = 1; }
                        else { scope_put(inner, ns->var.name, ns->var.type); }
                    } else if (ns->kind == STMT_RETURN) {
                        const char *et = check_expr(ns->expr, fdef, ast, path, &rc);
                        if (fdef->ret_type && strcmp(et, "unknown") != 0 && strcmp(et, "mismatch") != 0) {
                            if (strcmp(et, fdef->ret_type) != 0) { printf("%s: RETURN_MISMATCH: function %s declared %s but returned %s\n", path, fdef->name, fdef->ret_type, et); rc = 1; }
                        }
                    } else if (ns->kind == STMT_IF) {
                        // check condition
                        const char *ct = check_expr(ns->cond, fdef, ast, path, &rc);
                        (void)ct;
                        // push scope for if-body
                        Scope *ifs = scope_push(CUR_SCOPE);
                        CUR_SCOPE = ifs;
                        for (int bi = 0; bi < ns->stmt_count; ++bi) {
                            Stmt *bs = &ns->stmts[bi];
                            if (bs->kind == STMT_VARDECL) {
                                const char *et = check_expr(bs->var.init, fdef, ast, path, &rc);
                                if (strcmp(et, "mismatch") == 0) { printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name); rc = 1; }
                                else { scope_put(ifs, bs->var.name, bs->var.type); }
                            } else if (bs->kind == STMT_RETURN) {
                                const char *et = check_expr(bs->expr, fdef, ast, path, &rc);
                                if (fdef->ret_type && strcmp(et, "unknown") != 0 && strcmp(et, "mismatch") != 0) {
                                    if (strcmp(et, fdef->ret_type) != 0) { printf("%s: RETURN_MISMATCH: function %s declared %s but returned %s\n", path, fdef->name, fdef->ret_type, et); rc = 1; }
                                }
                            }
                        }
                        CUR_SCOPE = scope_pop(ifs);
                        // else branch
                        if (ns->else_stmts && ns->else_stmt_count > 0) {
                            Scope *els = scope_push(CUR_SCOPE);
                            CUR_SCOPE = els;
                            for (int ei = 0; ei < ns->else_stmt_count; ++ei) {
                                Stmt *es = &ns->else_stmts[ei];
                                if (es->kind == STMT_VARDECL) {
                                    const char *et = check_expr(es->var.init, fdef, ast, path, &rc);
                                    if (strcmp(et, "mismatch") == 0) { printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name); rc = 1; }
                                    else { scope_put(els, es->var.name, es->var.type); }
                                } else if (es->kind == STMT_RETURN) {
                                    const char *et = check_expr(es->expr, fdef, ast, path, &rc);
                                    if (fdef->ret_type && strcmp(et, "unknown") != 0 && strcmp(et, "mismatch") != 0) {
                                        if (strcmp(et, fdef->ret_type) != 0) { printf("%s: RETURN_MISMATCH: function %s declared %s but returned %s\n", path, fdef->name, fdef->ret_type, et); rc = 1; }
                                    }
                                }
                            }
                            CUR_SCOPE = scope_pop(els);
                        }
                    } else if (ns->kind == STMT_WHILE) {
                        const char *ct = check_expr(ns->cond, fdef, ast, path, &rc);
                        (void)ct;
                        Scope *ws = scope_push(CUR_SCOPE);
                        CUR_SCOPE = ws;
                        for (int bi = 0; bi < ns->stmt_count; ++bi) {
                            Stmt *bs = &ns->stmts[bi];
                            if (bs->kind == STMT_VARDECL) {
                                const char *et = check_expr(bs->var.init, fdef, ast, path, &rc);
                                if (strcmp(et, "mismatch") == 0) { printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name); rc = 1; }
                                else { scope_put(ws, bs->var.name, bs->var.type); }
                            }
                        }
                        CUR_SCOPE = scope_pop(ws);
                    } else if (ns->kind == STMT_FOR) {
                        // init
                        if (ns->for_init) {
                            if (ns->for_init->kind == STMT_VARDECL) {
                                const char *et = check_expr(ns->for_init->var.init, fdef, ast, path, &rc);
                                if (strcmp(et, "mismatch") == 0) { printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name); rc = 1; }
                            }
                        }
                        if (ns->cond) { const char *ct = check_expr(ns->cond, fdef, ast, path, &rc); (void)ct; }
                        if (ns->for_post) { const char *pt = check_expr(ns->for_post, fdef, ast, path, &rc); (void)pt; }
                        Scope *fs = scope_push(CUR_SCOPE);
                        CUR_SCOPE = fs;
                        for (int bi = 0; bi < ns->stmt_count; ++bi) {
                            Stmt *bs = &ns->stmts[bi];
                            if (bs->kind == STMT_VARDECL) {
                                const char *et = check_expr(bs->var.init, fdef, ast, path, &rc);
                                if (strcmp(et, "mismatch") == 0) { printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name); rc = 1; }
                                else { scope_put(fs, bs->var.name, bs->var.type); }
                            }
                        }
                        CUR_SCOPE = scope_pop(fs);
                    }
                }
                // pop inner scope
                CUR_SCOPE = scope_pop(inner);
                continue;
            }
            if (st->kind == STMT_VARDECL) {
                const char *et = check_expr(st->var.init, fdef, ast, path, &rc);
                if (strcmp(et, "mismatch") == 0) {
                    printf("%s: TYPE_MISMATCH: binary operands have different types in function %s\n", path, fdef->name);
                    rc = 1;
                } else if (strcmp(et, "unknown") == 0) {
                    scope_put(scope, st->var.name, st->var.type);
                } else {
                    if (strcmp(et, st->var.type) != 0) {
                        printf("%s: TYPE_MISMATCH: expected %s but found %s in assignment to %s (in %s)\n", path, st->var.type, et, st->var.name, fdef->name);
                        rc = 1;
                    }
                    scope_put(scope, st->var.name, st->var.type);
                }
            } else if (st->kind == STMT_RETURN) {
                const char *et = check_expr(st->expr, fdef, ast, path, &rc);
                if (fdef->ret_type && strcmp(et, "unknown") != 0 && strcmp(et, "mismatch") != 0) {
                    if (strcmp(et, fdef->ret_type) != 0) {
                        printf("%s: RETURN_MISMATCH: function %s declared %s but returned %s\n", path, fdef->name, fdef->ret_type, et);
                        rc = 1;
                    }
                }
            }
        }
        // pop and free function scope
        CUR_SCOPE = scope_pop(scope);
    }

    free_func_sigs();

    if (!rc) printf("%s: OK\n", path);
    ast_free(ast);
    free(buf);
    return rc;
}
