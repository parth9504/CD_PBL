%{
#include "ast.h"
#include "syntax.tab.h"
#include <stdlib.h>
#include <string.h>

void yyerror(const char* s);
%}

%option yylineno

digit   [0-9]
id      [a-zA-Z_][a-zA-Z0-9_]*
float   {digit}+\.{digit}+

%%

"int"           { return INT; }
"float"         { return FLOAT_T; }
"return"        { return RETURN; }
"main"          { return MAIN; }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"           { return FOR; }

"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LEQ; }
">="            { return GEQ; }
"<"             { return LT; }
">"             { return GT; }

"++"            { return INC; }
"--"            { return DEC; }

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"%"             { return '%'; }
"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
";"             { return ';'; }
"="             { return '='; }

{float}         {
                    yylval.fnum = atof(yytext);
                    return FLOAT;
                }

{digit}+        {
                    yylval.num = atoi(yytext);
                    return NUMBER;
                }

{id}            {
                    yylval.id = strdup(yytext);
                    if (!yylval.id) {
                        yyerror("Memory allocation failed for identifier");
                        exit(1);
                    }
                    return ID;
                }

[ \t]+          ; /* Skip spaces and tabs */
\n              ; /* Skip newlines, yylineno tracks line numbers */

"//"[^\n]*      ; /* Single-line comments */
"/*"([^*]|\*[^/])*"\*/" ; /* Multi-line comments */

.               {
                    
                }

%%

int yywrap() {
    return 1;
}