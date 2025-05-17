#include "ast.h"

AST* create_node(const char* node_type, const char* value, AST* left, AST* right) {
    AST* new_node = (AST*)malloc(sizeof(AST));
    new_node->node_type = strdup(node_type);
    new_node->value = value ? strdup(value) : NULL;
    new_node->left = left;
    new_node->right = right;
    return new_node;
}

void print_ast(AST* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("%s", node->node_type);
    if (node->value) printf(" (%s)", node->value);
    printf("\n");
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

void free_ast(AST* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node->node_type);
    if (node->value) free(node->value);
    free(node);
}
