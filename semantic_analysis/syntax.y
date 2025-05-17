%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"

extern int yylex();
extern int yylineno;
extern FILE* yyin;
extern char* yytext;

AST* root = NULL;
SymbolTable* symtab = NULL;

void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s (near token '%s')\n", yylineno, s, yytext);
}

void free_ast(AST* node);
%}

%union {
    int num;
    double fnum;
    char* id;
    AST* node;
}

%token <num> NUMBER
%token <fnum> FLOAT
%token <id> ID
%token IF ELSE WHILE FOR
%token EQ NEQ LEQ GEQ LT GT
%token INT FLOAT_T RETURN MAIN
%token '%' INC DEC

%type <node> program stmt_list stmt expr block func_decl param_list param

%left EQ NEQ LEQ GEQ LT GT
%left '+' '-'
%left '*' '/' '%'
%left INC DEC
%left UMINUS UPLUS

%start program

%%

program:
      func_decl
    {
        root = $1;
        $$ = $1;
    }
  | program func_decl
    {
        $$ = create_node("program", NULL, $1, $2);
        root = $$;
    }
    ;

func_decl:
      INT MAIN '(' ')' block
    {
        $$ = create_node("func_main", NULL, $5, NULL);
    }
  | INT ID '(' param_list ')' block
    {
        $$ = create_node("func_decl", $2, $4, $6);
        free($2);
    }
    ;

param_list:
      param
    {
        $$ = create_node("param_list", NULL, $1, NULL);
    }
  | param_list ',' param
    {
        $$ = create_node("param_list", NULL, $1, $3);
    }
  | /* empty */
    {
        $$ = NULL;
    }
    ;

param:
      INT ID
    {
        insert_var(symtab, $2, TYPE_INT);
        $$ = create_node("param", $2, NULL, NULL);
        free($2);
    }
  | FLOAT_T ID
    {
        insert_var(symtab, $2, TYPE_FLOAT);
        $$ = create_node("param_float", $2, NULL, NULL);
        free($2);
    }
    ;

stmt_list:
      stmt_list stmt
    {
        $$ = create_node("stmt_list", NULL, $1, $2);
    }
  | stmt
    {
        $$ = $1;
    }
    ;

stmt:
      IF '(' expr ')' stmt
    {
        $$ = create_node("if_stmt", NULL, $3, $5);
    }
  | IF '(' expr ')' stmt ELSE stmt
    {
        $$ = create_node("if_else_stmt", NULL, $3, create_node("branch", NULL, $5, $7));
    }
  | WHILE '(' expr ')' stmt
    {
        $$ = create_node("while_loop", NULL, $3, $5);
    }
  | FOR '(' expr ';' expr ';' expr ')' stmt
    {
        $$ = create_node("for_loop", NULL,
                         create_node("for_init", NULL, $3, NULL),
                         create_node("for_cond", NULL, $5,
                                     create_node("for_update", NULL, $7, $9)));
    }
  | ID '=' expr ';'
    {
        if (!lookup_var(symtab, $1)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $1);
        }
        $$ = create_node("assign_stmt", $1, $3, NULL);
        free($1);
    }
  | INC ID ';'
    {
        if (!lookup_var(symtab, $2)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $2);
        }
        $$ = create_node("inc_stmt", $2, NULL, NULL);
        free($2);
    }
  | DEC ID ';'
    {
        if (!lookup_var(symtab, $2)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $2);
        }
        $$ = create_node("dec_stmt", $2, NULL, NULL);
        free($2);
    }
  | RETURN expr ';'
    {
        $$ = create_node("return_stmt", NULL, $2, NULL);
    }
  | expr ';'
    {
        $$ = create_node("expr_stmt", NULL, $1, NULL);
    }
  | block
    {
        $$ = $1;
    }
  | INT ID ';'
    {
        insert_var(symtab, $2, TYPE_INT);
        $$ = create_node("var_decl_int", $2, NULL, NULL);
        free($2);
    }
  | INT ID '=' expr ';'
    {
        insert_var(symtab, $2, TYPE_INT);
        $$ = create_node("var_decl_int_init", $2, $4, NULL);
        free($2);
    }
  | FLOAT_T ID ';'
    {
        insert_var(symtab, $2, TYPE_FLOAT);
        $$ = create_node("var_decl_float", $2, NULL, NULL);
        free($2);
    }
  | FLOAT_T ID '=' expr ';'
    {
        insert_var(symtab, $2, TYPE_FLOAT);
        $$ = create_node("var_decl_float_init", $2, $4, NULL);
        free($2);
    }
    ;

block:
      '{' stmt_list '}'
    {
        $$ = create_node("block", NULL, $2, NULL);
    }
  | '{' '}'
    {
        $$ = create_node("block", NULL, NULL, NULL);
    }
    ;

expr:
      expr '+' expr
    {
        $$ = create_node("add", NULL, $1, $3);
    }
  | expr '-' expr
    {
        $$ = create_node("sub", NULL, $1, $3);
    }
  | expr '*' expr
    {
        $$ = create_node("mul", NULL, $1, $3);
    }
  | expr '/' expr
    {
        $$ = create_node("div", NULL, $1, $3);
    }
  | expr '%' expr
    {
        $$ = create_node("mod", NULL, $1, $3);
    }
  | expr EQ expr
    {
        $$ = create_node("eq", NULL, $1, $3);
    }
  | expr NEQ expr
    {
        $$ = create_node("neq", NULL, $1, $3);
    }
  | expr LEQ expr
    {
        $$ = create_node("leq", NULL, $1, $3);
    }
  | expr GEQ expr
    {
        $$ = create_node("geq", NULL, $1, $3);
    }
  | expr LT expr
    {
        $$ = create_node("lt", NULL, $1, $3);
    }
  | expr GT expr
    {
        $$ = create_node("gt", NULL, $1, $3);
    }
  | '-' expr %prec UMINUS
    {
        $$ = create_node("unary_minus", NULL, $2, NULL);
    }
  | '+' expr %prec UPLUS
    {
        $$ = create_node("unary_plus", NULL, $2, NULL);
    }
  | ID INC
    {
        if (!lookup_var(symtab, $1)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $1);
        }
        $$ = create_node("post_inc", $1, NULL, NULL);
        free($1);
    }
  | ID DEC
    {
        if (!lookup_var(symtab, $1)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $1);
        }
        $$ = create_node("post_dec", $1, NULL, NULL);
        free($1);
    }
  | INC ID
    {
        if (!lookup_var(symtab, $2)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $2);
        }
        $$ = create_node("pre_inc", $2, NULL, NULL);
        free($2);
    }
  | DEC ID
    {
        if (!lookup_var(symtab, $2)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $2);
        }
        $$ = create_node("pre_dec", $2, NULL, NULL);
        free($2);
    }
  | '(' expr ')'
    {
        $$ = $2;
    }
  | NUMBER
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", $1);
        $$ = create_node("number", buf, NULL, NULL);
    }
  | FLOAT
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", $1);
        $$ = create_node("float", buf, NULL, NULL);
    }
  | ID
    {
        if (!lookup_var(symtab, $1)) {
            fprintf(stderr, "Error at line %d: Undeclared variable '%s'\n", yylineno, $1);
        }
        $$ = create_node("id", $1, NULL, NULL);
        free($1);
    }
    ;

%%