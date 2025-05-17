#include "symtab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SymbolTable* create_symtab() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed for symbol table\n");
        exit(1);
    }
    table->head = NULL;
    return table;
}

void insert_var(SymbolTable* table, const char* name, VarType type) {
    Symbol* sym = lookup_var(table, name);
    if (sym) {
        fprintf(stderr, "Error: Redeclaration of variable '%s'\n", name);
        return;
    }
    Symbol* new_sym = malloc(sizeof(Symbol));
    if (!new_sym) {
        fprintf(stderr, "Memory allocation failed for symbol\n");
        exit(1);
    }
    new_sym->name = strdup(name);
    new_sym->type = type;
    new_sym->next = table->head;
    table->head = new_sym;
}

Symbol* lookup_var(SymbolTable* table, const char* name) {
    for (Symbol* sym = table->head; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

void print_symtab(SymbolTable* table) {
    printf("Symbol Table:\n");
    printf("Name\tType\n");
    printf("----\t----\n");
    for (Symbol* sym = table->head; sym; sym = sym->next) {
        printf("%s\t%s\n", 
               sym->name, 
               sym->type == TYPE_INT ? "int" : "float");
    }
}

void free_symtab(SymbolTable* table) {
    while (table->head) {
        Symbol* temp = table->head;
        table->head = temp->next;
        free(temp->name);
        free(temp);
    }
    free(table);
}