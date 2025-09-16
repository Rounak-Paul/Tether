#ifndef TETHER_AST_H
#define TETHER_AST_H

#include "../lexer/tokens.h"
#include <stdlib.h>

typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DECL,
    AST_VARIABLE_DECL,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_FUNCTION_CALL,
    AST_IF_STMT,
    AST_FOR_STMT,
    AST_BLOCK_STMT,
    AST_RETURN_STMT,
    AST_DECORATOR,
    AST_LITERAL,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode** children;
    int child_count;
    Token* token; // associated token for position info
    void* data; // node-specific data (caller manages allocation)
} ASTNode;

ASTNode* ast_node_create(ASTNodeType type, Token* token);
void ast_node_add_child(ASTNode* parent, ASTNode* child);
void ast_node_free(ASTNode* node);

#endif // TETHER_AST_H
