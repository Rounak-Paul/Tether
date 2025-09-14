#pragma once
#include <stdint.h>

typedef enum {
    TOK_EOF,
    TOK_IDENT,
    TOK_INT,
    TOK_FLOAT,
    TOK_AT, // @
    TOK_DEF,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_FOR,
    TOK_WHILE,
    TOK_ARROW, // ->
    TOK_COLON,
    TOK_COMMA,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_SEMI,
    TOK_ASSIGN, // =
    TOK_OTHER,
} TokenKind;

typedef struct {
    TokenKind kind;
    char *text; // nul-terminated
    int line;
} Token;

typedef struct TokenStream TokenStream;

TokenStream *tokenstream_create(const char *src);
void tokenstream_free(TokenStream *ts);
Token tokenstream_peek(TokenStream *ts);
Token tokenstream_next(TokenStream *ts);
Token tokenstream_peek_n(TokenStream *ts, int n);
