
// A small recursive-descent / Pratt parser for the prototype language.
// A small recursive-descent / Pratt parser for the prototype language.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/token.h"
#include "../include/ast.h"

static TokenStream *G_ts = NULL;

static Token peek_tok() { return tokenstream_peek(G_ts); }
static Token peek_n(int n) { return tokenstream_peek_n(G_ts, n); }
static Token next_tok() { return tokenstream_next(G_ts); }

// forward
static Expr *parse_primary();
static Expr *parse_expression(int prec);

// operator precedence
static int op_prec(const char *op) {
    if (!op) return 0;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 20;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 10;
    return 0;
}

static Expr *parse_primary() {
    Token t = next_tok();
    if (t.kind == TOK_INT) return expr_new_int(t.text);
    if (t.kind == TOK_FLOAT) return expr_new_float(t.text);
    if (t.kind == TOK_LPAREN) {
        Expr *e = parse_expression(0);
        Token r = next_tok(); // expect RPAREN
        (void)r;
        return e;
    }
    if (t.kind == TOK_IDENT) {
        Token p = peek_tok();
        if (p.kind == TOK_LPAREN) {
            // call
            tokenstream_next(G_ts); // consume LPAREN
            Expr *call = expr_new_call(t.text);
            Token it = peek_tok();
            if (it.kind == TOK_RPAREN) { tokenstream_next(G_ts); return call; }
            while (1) {
                Expr *arg = parse_expression(0);
                call->arg_count++;
                call->args = realloc(call->args, sizeof(Expr*)*call->arg_count);
                call->args[call->arg_count-1] = arg;
                Token sep = peek_tok();
                if (sep.kind == TOK_COMMA) { tokenstream_next(G_ts); continue; }
                if (sep.kind == TOK_RPAREN) { tokenstream_next(G_ts); break; }
                break;
            }
            return call;
        }
        return expr_new_ident(t.text);
    }
    // unknown primary
    return NULL;
}

static Expr *parse_expression(int prec) {
    Expr *left = parse_primary();
    while (1) {
        Token p = peek_tok();
        if (p.kind != TOK_OTHER && p.kind != TOK_IDENT && p.kind != TOK_INT && p.kind != TOK_FLOAT) {
            // some operators are returned as TOK_OTHER (single-char), but tokenstream emits ops as TOK_OTHER
        }
        // treat the token text as operator for single-char tokens
        const char *op = p.text;
        int pprec = op ? op_prec(op) : 0;
        // also handle '+' '-' '*' '/'
        if (!p.text) break;
        // tokenstream may return operators as TOK_OTHER; check first char
        if (p.kind == TOK_OTHER) {
            if (p.text[0] == '+' || p.text[0] == '-' || p.text[0] == '*' || p.text[0] == '/') {
                char sop[2] = { p.text[0], '\0' };
                pprec = op_prec(sop);
                op = sop;
            } else break;
        } else if (p.kind == TOK_ASSIGN || p.kind == TOK_COLON || p.kind == TOK_COMMA || p.kind == TOK_SEMI) {
            break;
        } else if (p.kind == TOK_RPAREN || p.kind == TOK_RBRACE || p.kind == TOK_EOF) {
            break;
        } else if (p.kind == TOK_IDENT && p.text && strlen(p.text) == 1 && (p.text[0]=='+'||p.text[0]=='-')) {
            // unlikely
            char sop[2] = { p.text[0], '\0' };
            pprec = op_prec(sop);
            op = sop;
        }

        if (pprec <= prec) break;
        // consume operator
        Token opt = next_tok();
        char opbuf[4] = "";
        if (opt.text) strncpy(opbuf, opt.text, sizeof(opbuf)-1);
        Expr *right = parse_expression(pprec);
        left = expr_new_binary(left, opbuf, right);
    }
    return left;
}

// forward-declare helper used below
static Stmt *parse_statements_preconsumed(int *out_count); 

// parse a single statement (used by parse_statements_preconsumed)
static Stmt parse_statement() {
    Stmt stmt = {0};
    Token st = peek_tok();
    if (st.kind == TOK_IDENT) {
        // For either a top-level var decl (ident :) or an expression starting with an ident
        // we need to lookahead to distinguish. If it's a var decl, parse that; otherwise
        // parse a full expression (this ensures calls like print(x) are parsed correctly).
        Token n = peek_tok();
        Token maybe_colon = peek_n(1);
        if (maybe_colon.kind == TOK_COLON) {
            // variable declaration
            next_tok(); // consume name
            next_tok(); // consume ':'
            Token typ = next_tok(); Token assign = next_tok();
            if (assign.kind == TOK_ASSIGN) {
                Expr *e = parse_expression(0);
                next_tok(); // consume semi
                stmt.kind = STMT_VARDECL;
                stmt.var.name = strdup(n.text);
                stmt.var.type = strdup(typ.text);
                stmt.var.init = e;
                return stmt;
            }
        }
        // Not a var decl -> parse full expression (handles calls)
        Expr *e = parse_expression(0);
        next_tok(); // consume trailing semicolon
        stmt.kind = STMT_EXPR; stmt.sexpr = e; return stmt;
    }
    if (st.kind == TOK_RETURN) {
        next_tok(); Expr *e = parse_expression(0); next_tok(); stmt.kind = STMT_RETURN; stmt.expr = e; return stmt;
    }
    if (st.kind == TOK_IF) {
        next_tok(); next_tok(); // consume 'if' and LPAREN
        Expr *cond = parse_expression(0); next_tok(); // consume RPAREN
        stmt.kind = STMT_IF; stmt.cond = cond; stmt.stmts = NULL; stmt.stmt_count = 0; stmt.else_stmts = NULL; stmt.else_stmt_count = 0;
        if (peek_tok().kind == TOK_LBRACE) {
            next_tok(); int c=0; Stmt *s = parse_statements_preconsumed(&c); stmt.stmts = s; stmt.stmt_count = c;
        }
        if (peek_tok().kind == TOK_ELSE) {
            next_tok(); if (peek_tok().kind == TOK_LBRACE) { next_tok(); int c=0; Stmt *s = parse_statements_preconsumed(&c); stmt.else_stmts = s; stmt.else_stmt_count = c; }
        }
        return stmt;
    }
    if (st.kind == TOK_WHILE) {
        next_tok(); next_tok(); // consume 'while' and LPAREN
        Expr *cond = parse_expression(0); next_tok(); // consume RPAREN
        stmt.kind = STMT_WHILE; stmt.cond = cond; stmt.stmts = NULL; stmt.stmt_count = 0;
        if (peek_tok().kind == TOK_LBRACE) { next_tok(); int c=0; Stmt *s = parse_statements_preconsumed(&c); stmt.stmts = s; stmt.stmt_count = c; }
        return stmt;
    }
    if (st.kind == TOK_FOR) {
        next_tok(); next_tok(); // consume 'for' and LPAREN
        Stmt *init_stmt = NULL; Token pk = peek_tok();
        if (pk.kind == TOK_IDENT) { Token n = next_tok(); Token maybe_colon = peek_tok(); if (maybe_colon.kind == TOK_COLON) { next_tok(); Token typ = next_tok(); Token assign = next_tok(); if (assign.kind == TOK_ASSIGN) { Expr *e = parse_expression(0); next_tok(); init_stmt = calloc(1, sizeof(Stmt)); init_stmt->kind = STMT_VARDECL; init_stmt->var.name = strdup(n.text); init_stmt->var.type = strdup(typ.text); init_stmt->var.init = e; } } }
        next_tok(); // consume semicolon
        Expr *cond = NULL; if (peek_tok().kind != TOK_SEMI) cond = parse_expression(0);
        next_tok(); // consume semicolon
        Expr *post = NULL; if (peek_tok().kind != TOK_RPAREN) post = parse_expression(0);
        next_tok(); // consume RPAREN
        stmt.kind = STMT_FOR; stmt.for_init = init_stmt; stmt.cond = cond; stmt.for_post = post; stmt.stmts = NULL; stmt.stmt_count = 0;
        if (peek_tok().kind == TOK_LBRACE) { next_tok(); int c=0; Stmt *s = parse_statements_preconsumed(&c); stmt.stmts = s; stmt.stmt_count = c; }
        return stmt;
    }
    if (st.kind == TOK_LBRACE) {
        next_tok(); int c=0; Stmt *s = parse_statements_preconsumed(&c);
        stmt.kind = STMT_BLOCK; stmt.stmts = s; stmt.stmt_count = c; return stmt;
    }
    // expr stmt starting tokens
    if (st.kind == TOK_INT || st.kind == TOK_FLOAT || st.kind == TOK_LPAREN) {
        Expr *e = parse_expression(0); next_tok(); stmt.kind = STMT_EXPR; stmt.sexpr = e; return stmt;
    }
    // fallback: consume token and return empty stmt
    next_tok(); stmt.kind = STMT_EXPR; stmt.sexpr = NULL; return stmt;
}

// parse statements until a closing RBRACE; assumes the opening LBRACE has already been consumed
static Stmt *parse_statements_preconsumed(int *out_count) {
    Stmt *arr = NULL; int cnt = 0;
    while (peek_tok().kind != TOK_RBRACE && peek_tok().kind != TOK_EOF) {
        Stmt s = parse_statement();
        cnt++; arr = realloc(arr, sizeof(Stmt)*cnt); arr[cnt-1] = s;
    }
    next_tok(); // consume RBRACE
    *out_count = cnt; return arr;
}

// parse top-level AST
AST *parser_parse_ast(const char *src) {
    AST *ast = calloc(1, sizeof(AST));
    ast->funcs = NULL; ast->func_count = 0; ast->vars = NULL; ast->var_count = 0;
    G_ts = tokenstream_create(src);

    Token tok = next_tok();
    while (tok.kind != TOK_EOF) {
        if (tok.kind == TOK_DEF) {
            Token name = next_tok();
            FuncDef fd = {0};
            fd.name = strdup(name.text);
            fd.params = NULL; fd.param_count = 0; fd.ret_type = NULL; fd.body = NULL; fd.stmt_count = 0;
            // expect LPAREN
            next_tok(); // LPAREN
            // parse params
            Token p = peek_tok();
            if (p.kind != TOK_RPAREN) {
                while (1) {
                    Token pname = next_tok(); // ident
                    next_tok(); // COLON
                    Token ptype = next_tok(); // type ident
                    fd.param_count++;
                    fd.params = realloc(fd.params, sizeof(Param)*fd.param_count);
                    fd.params[fd.param_count-1].name = strdup(pname.text);
                    fd.params[fd.param_count-1].type = strdup(ptype.text);
                    Token sep = peek_tok();
                    if (sep.kind == TOK_COMMA) { next_tok(); continue; }
                    if (sep.kind == TOK_RPAREN) break;
                }
            }
            next_tok(); // consume RPAREN
            // optional return arrow
            Token ar = peek_tok();
            if (ar.kind == TOK_ARROW) { next_tok(); Token rtype = next_tok(); fd.ret_type = strdup(rtype.text); }
            // expect LBRACE
            next_tok();
            // parse function body statements until closing RBRACE
            while (peek_tok().kind != TOK_RBRACE && peek_tok().kind != TOK_EOF) {
                Stmt s = parse_statement();
                fd.stmt_count++; fd.body = realloc(fd.body, sizeof(Stmt)*fd.stmt_count); fd.body[fd.stmt_count-1] = s;
            }
            // consume closing RBRACE
            next_tok();

            ast->funcs = realloc(ast->funcs, sizeof(FuncDef)*(ast->func_count+1)); ast->funcs[ast->func_count++] = fd;
            tok = next_tok();
            continue;
        }

        // top-level var decl
        if (tok.kind == TOK_IDENT) {
            Token n = tok;
            Token colon = peek_tok();
            if (colon.kind == TOK_COLON) {
                next_tok(); // consume colon
                Token typ = next_tok();
                Token assign = next_tok();
                if (assign.kind == TOK_ASSIGN) {
                    Expr *e = parse_expression(0);
                    next_tok(); // consume semicolon
                    VarDecl vd = {0}; vd.name = strdup(n.text); vd.type = strdup(typ.text); vd.init = e; ast->vars = realloc(ast->vars, sizeof(VarDecl)*(ast->var_count+1)); ast->vars[ast->var_count++] = vd; tok = next_tok(); continue;
                }
            }
        }

        tok = next_tok();
    }

    tokenstream_free(G_ts);
    G_ts = NULL;
    return ast;
}



