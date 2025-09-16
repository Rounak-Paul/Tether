#include "tokens.h"
#include <string.h>
#include <stdlib.h>

Token* token_create(TokenType type, const char* lexeme, size_t length, int line, int column, int start_offset) {
    Token* t = (Token*)malloc(sizeof(Token));
    if (!t) return NULL;
    t->type = type;
    t->length = length;
    t->line = line;
    t->column = column;
    t->start_offset = start_offset;
    if (lexeme) {
        t->lexeme = (char*)malloc(length + 1);
        if (!t->lexeme) { free(t); return NULL; }
        memcpy(t->lexeme, lexeme, length);
        t->lexeme[length] = '\0';
    } else {
        t->lexeme = NULL;
    }
    return t;
}

void token_free(Token* tok) {
    if (!tok) return;
    if (tok->lexeme) free(tok->lexeme);
    free(tok);
}
