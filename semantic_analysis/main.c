#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symtab.h"
#include "syntax.tab.h"

extern FILE* yyin;
extern AST* root;
extern SymbolTable* symtab;

int main(int argc, char **argv) {
    FILE* input_file = NULL;
    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (!input_file) {
            perror("File open failed");
            return 1;
        }
        yyin = input_file;
    } else {
        yyin = stdin;
    }

    symtab = create_symtab();
    if (yyparse() == 0) {
        printf("Parsing finished successfully.\n");
        printf("\n");
        print_symtab(symtab);
    } else {
        printf("Parsing failed.\n");
    }

    if (root) free_ast(root);
    free_symtab(symtab);
    if (input_file) fclose(input_file);
    return 0;
}