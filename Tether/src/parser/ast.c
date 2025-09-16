#include "ast.h"
#include <string.h>

ASTNode* ast_node_create(ASTNodeType type, Token* token) {
    ASTNode* n = (ASTNode*)malloc(sizeof(ASTNode));
    if (!n) return NULL;
    n->type = type;
    n->children = NULL;
    n->child_count = 0;
    n->token = token;
    n->data = NULL;
    return n;
}

void ast_node_add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    ASTNode** new_children = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * (parent->child_count + 1));
    if (!new_children) return; // OOM: drop silently
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
}

void ast_node_free(ASTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; ++i) {
        ast_node_free(node->children[i]);
    }
    if (node->children) free(node->children);
    // Note: token ownership is not assumed here; caller should free tokens
    free(node);
}
