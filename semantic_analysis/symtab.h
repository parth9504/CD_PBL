#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum { TYPE_INT, TYPE_FLOAT } VarType;

typedef struct Symbol {
    char* name;
    VarType type;
    struct Symbol* next;
} Symbol;

typedef struct SymbolTable {
    Symbol* head;
} SymbolTable;

SymbolTable* create_symtab();
void insert_var(SymbolTable* table, const char* name, VarType type);
Symbol* lookup_var(SymbolTable* table, const char* name);
void print_symtab(SymbolTable* table);
void free_symtab(SymbolTable* table);

#endif