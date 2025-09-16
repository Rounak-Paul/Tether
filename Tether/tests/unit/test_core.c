#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/lexer/tokens.h"
#include "../../src/parser/ast.h"
#include "../../src/semantic/symbol_table.h"
#include "../../src/runtime/value.h"
#include "../../src/utils/error.h"

int main(void) {
    // Token
    const char* txt = "hello";
    Token* t = token_create(TOKEN_IDENTIFIER, txt, strlen(txt), 1, 1, 0);
    if (!t || strcmp(t->lexeme, "hello") != 0) { fprintf(stderr, "token test failed\n"); return 1; }

    // AST
    ASTNode* root = ast_node_create(AST_PROGRAM, NULL);
    ASTNode* ident = ast_node_create(AST_IDENTIFIER, t);
    ast_node_add_child(root, ident);
    if (root->child_count != 1) { fprintf(stderr, "ast children test failed\n"); return 1; }

    // Symbol table
    SymbolTable* st = symbol_table_create(16, NULL);
    if (!symbol_table_insert(st, "x", SYMBOL_VARIABLE, NULL)) { fprintf(stderr, "symbol insert failed\n"); return 1; }
    Symbol* s = symbol_table_lookup(st, "x");
    if (!s || strcmp(s->name, "x") != 0) { fprintf(stderr, "symbol lookup failed\n"); return 1; }

    // Value
    Value vs = value_string("world");
    if (vs.type != VAL_STRING || strcmp(vs.as.string, "world") != 0) { fprintf(stderr, "value string failed\n"); return 1; }
    value_free(&vs);

    // Error
    TetherError* err = tether_error_create("something bad", 2, 3);
    if (!err) { fprintf(stderr, "error create failed\n"); return 1; }
    tether_error_print(err, stdout);
    tether_error_free(err);

    // Cleanup
    ast_node_free(root); // this will not free token - free manually
    token_free(t);
    symbol_table_free(st);

    printf("All core tests passed.\n");
    return 0;
}
