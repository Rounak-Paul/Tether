#ifndef TETHER_TOKENS_H
#define TETHER_TOKENS_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,

    // Data Types (examples)
    TOKEN_U8, TOKEN_U16, TOKEN_U32, TOKEN_U64,
    TOKEN_I8, TOKEN_I16, TOKEN_I32, TOKEN_I64,
    TOKEN_F32, TOKEN_F64,

    // Keywords
    TOKEN_FN, TOKEN_IF, TOKEN_ELSE, TOKEN_FOR,
    TOKEN_WHILE, TOKEN_RETURN, TOKEN_TRUE, TOKEN_FALSE,

    // Decorators
    TOKEN_AT,
    TOKEN_PARALLEL,
    TOKEN_VECTORIZE,
    TOKEN_INLINE,
    TOKEN_EXTERN,

    // Operators
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_ASSIGN, TOKEN_EQUAL, TOKEN_NOT_EQUAL,
    TOKEN_LESS, TOKEN_GREATER, TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL,

    // Delimiters
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_LBRACKET, TOKEN_RBRACKET,
    TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_COLON,

    // Special
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct Token {
    TokenType type;
    char* lexeme;      // nul-terminated string (heap-owned)
    size_t length;     // length of lexeme
    int line;          // 1-based line number
    int column;        // 1-based column number (character offset in line)
    int start_offset;  // byte offset from file start (optional)
} Token;

// Simple helpers
Token* token_create(TokenType type, const char* lexeme, size_t length, int line, int column, int start_offset);
void token_free(Token* tok);

#endif // TETHER_TOKENS_H
