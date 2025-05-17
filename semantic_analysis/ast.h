#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AST {
    char* node_type;
    char* value;
    struct AST* left;
    struct AST* right;
} AST;

AST* create_node(const char* node_type, const char* value, AST* left, AST* right);
void print_ast(AST* node, int level);
void free_ast(AST* node);

#endif
