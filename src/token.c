#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../include/token.h"

struct TokenStream {
    char *buf;
    size_t pos;
    int line;
    Token cur;
};

static int is_ident_start(char c) { return isalpha((unsigned char)c) || c == '_'; }
static int is_ident_char(char c) { return isalnum((unsigned char)c) || c == '_'; }

TokenStream *tokenstream_create(const char *src) {
    TokenStream *ts = calloc(1, sizeof(TokenStream));
    ts->buf = strdup(src);
    ts->pos = 0;
    ts->line = 1;
    ts->cur.kind = TOK_OTHER;
    ts->cur.text = NULL;
    return ts;
}

void token_free_text(Token *t) { if (t->text) free(t->text); t->text = NULL; }

void tokenstream_free(TokenStream *ts) {
    if (!ts) return;
    token_free_text(&ts->cur);
    free(ts->buf);
    free(ts);
}

static void emit_token(TokenStream *ts, TokenKind k, const char *start, size_t len) {
    token_free_text(&ts->cur);
    ts->cur.kind = k;
    ts->cur.text = malloc(len + 1);
    memcpy(ts->cur.text, start, len);
    ts->cur.text[len] = '\0';
    ts->cur.line = ts->line;
    // debug removed for cleaner output
}

static void skip_space(TokenStream *ts) {
    char *b = ts->buf;
    while (b[ts->pos]) {
        char c = b[ts->pos];
        if (c == ' ' || c == '\t' || c == '\r') { ts->pos++; continue; }
        if (c == '\n') { ts->line++; ts->pos++; continue; }
        if (c == '/' && b[ts->pos+1] == '/') {
            // skip line comment
            ts->pos += 2;
            while (b[ts->pos] && b[ts->pos] != '\n') ts->pos++;
            continue;
        }
        break;
    }
}

Token tokenstream_next(TokenStream *ts) {
    skip_space(ts);
    char *b = ts->buf;
    size_t p = ts->pos;
    if (!b[p]) { emit_token(ts, TOK_EOF, "", 0); Token out = ts->cur; out.text = strdup(ts->cur.text ? ts->cur.text : ""); return out; }
    char c = b[p];
    if (is_ident_start(c)) {
        size_t s = p;
        p++;
        while (is_ident_char(b[p])) p++;
        size_t len = p - s;
    // check keywords
        if (len == 3 && strncmp(b+s, "def", 3) == 0) { emit_token(ts, TOK_DEF, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        if (len == 6 && strncmp(b+s, "return", 6) == 0) { emit_token(ts, TOK_RETURN, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
    if (len == 2 && strncmp(b+s, "if", 2) == 0) { emit_token(ts, TOK_IF, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
    if (len == 4 && strncmp(b+s, "else", 4) == 0) { emit_token(ts, TOK_ELSE, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        if (len == 3 && strncmp(b+s, "for", 3) == 0) { emit_token(ts, TOK_FOR, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        if (len == 5 && strncmp(b+s, "while", 5) == 0) { emit_token(ts, TOK_WHILE, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        emit_token(ts, TOK_IDENT, b+s, len); ts->pos = p; { Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
    }
    if (isdigit((unsigned char)c)) {
        size_t s = p;
        p++;
        int has_dot = 0;
        while (isdigit((unsigned char)b[p]) || (!has_dot && b[p]=='.')) {
            if (b[p] == '.') has_dot = 1;
            p++;
        }
        size_t len = p - s;
        if (has_dot) { emit_token(ts, TOK_FLOAT, b+s, len); ts->pos = p; Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        emit_token(ts, TOK_INT, b+s, len); ts->pos = p; { Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
    }
    // single-char tokens
    switch (c) {
        case '@': emit_token(ts, TOK_AT, "@", 1); ts->pos++; { Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        case '-':
            if (b[p+1] == '>') { emit_token(ts, TOK_ARROW, "->", 2); ts->pos += 2; return ts->cur; }
            break;
        case ':': emit_token(ts, TOK_COLON, ":", 1); ts->pos++; return ts->cur;
        case ',': emit_token(ts, TOK_COMMA, ",", 1); ts->pos++; return ts->cur;
        case '(': emit_token(ts, TOK_LPAREN, "(", 1); ts->pos++; return ts->cur;
        case ')': emit_token(ts, TOK_RPAREN, ")", 1); ts->pos++; return ts->cur;
        case '{': emit_token(ts, TOK_LBRACE, "{", 1); ts->pos++; return ts->cur;
        case '}': emit_token(ts, TOK_RBRACE, "}", 1); ts->pos++; return ts->cur;
        case '[': emit_token(ts, TOK_LBRACKET, "[", 1); ts->pos++; return ts->cur;
        case ']': emit_token(ts, TOK_RBRACKET, "]", 1); ts->pos++; return ts->cur;
        case ';': emit_token(ts, TOK_SEMI, ";", 1); ts->pos++; { Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
        case '=': emit_token(ts, TOK_ASSIGN, "=", 1); ts->pos++; { Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
    }
    // fallback: single other char
    emit_token(ts, TOK_OTHER, b+p, 1); ts->pos++; { Token out = ts->cur; out.text = strdup(ts->cur.text?ts->cur.text:"" ); return out; }
}

Token tokenstream_peek(TokenStream *ts) {
    // Create a temporary TokenStream copy to call tokenstream_next without affecting the original
    TokenStream tmp = {0};
    tmp.buf = ts->buf;
    tmp.pos = ts->pos;
    tmp.line = ts->line;
    tmp.cur.kind = TOK_OTHER;
    tmp.cur.text = NULL;
    Token t = tokenstream_next(&tmp);
    Token copy = t;
    copy.text = strdup(t.text ? t.text : "");
    // clean up tmp.cur.text
    token_free_text(&tmp.cur);
    return copy;
}

Token tokenstream_peek_n(TokenStream *ts, int n) {
    TokenStream tmp = {0};
    tmp.buf = ts->buf;
    tmp.pos = ts->pos;
    tmp.line = ts->line;
    tmp.cur.kind = TOK_OTHER;
    tmp.cur.text = NULL;
    Token t = {0};
    for (int i = 0; i <= n; ++i) {
        t = tokenstream_next(&tmp);
    }
    return t;
}
