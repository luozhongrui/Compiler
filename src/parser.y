%{
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token ID CHAR NUM VAR ARRAY OF BOOLEAN INTEGER REAL STRING TRUE FALSE DEF RETURN begin end WHILE DO IF THEN ELSE FOR TO PRINT READ ASSIGN ',' ';' '(' '[' ')' ']' ':'

%left AND OR 
%left '<' '>'  '=' GreaterEqual LessEqual NotEqual
%left '+' '-'
%left '*' '/' MOD
%right NOT
%% 

ProgramName: ProgramName itemProgram
           | itemProgram
           ;

itemProgram: ID ';' CompStat end
           | ID ';' VarConFunDecDef CompStat end
           ;
           

VarConFunDecDef: VarConFunDecDef itemVarConFunDecDef
               | itemVarConFunDecDef
               ;

itemVarConFunDecDef: VarConstant
                    | FuncDecDef
                    ;


/* var and constant*/
VarConstant:  VarConstant itemVarConstant
            | itemVarConstant
            | 
            ;
            
itemVarConstant:  VarDecl
                | Constant
                ;

/* var declare*/
VarDecl: VAR IdList ':' ScalType ';'
        | VAR IdList ':' ArrayType ';'
        ;

/* Constant declare*/
Constant: VAR IdList ':' LiterConstant ';'

ScalType: INTEGER
        | BOOLEAN
        | REAL
        | STRING
        ;
/* array type */
ArrayType: ArrayDim ScalType;
ArrayDim: ArrayDim itemArrayDim
         | itemArrayDim
         ;
itemArrayDim: ARRAY NUM OF ;

/* identifier list */
IdList:   IdList ',' ID
        | ID
        ;

LiterConstant: NUM
             | '-' NUM
             | CHAR
             | TRUE
             | FALSE
             ;

/* Function declaration and definition */
FuncDecDef: FuncDecDef itemFuncDecDef
           | itemFuncDecDef
           ;
itemFuncDecDef: FuncDec
              | FuncDef
              ;

/* func declaration */
FuncDec:  ID '(' ')' ':' ScalType ';'
        | ID '(' argument ')' ':' ScalType ';'
        | ID '(' ')' ';'
        | ID '(' argument ')' ';'
        ;

argument: argument ';' IdList ':' ScalType
        | IdList ':' ScalType
        ;

/* function definition */
FuncDef:  ID '(' ')' ':' ScalType CompStat end
        | ID '(' argument ')' ':' ScalType CompStat end
        | ID '(' ')' CompStat end
        | ID '(' argument ')' CompStat end
        ;

Statements: Statements itemStatement
          | itemStatement
          | 
          ;

itemStatement: CompStat
             | SimpleStat
             | ConditionStat
             | WhileStat
             | ForStat
             | ReturnStat
             | FunCall
             ;

CompStat: begin end
        | begin VarConstant Statements end
        ;

SimpleStat: VarRefer ASSIGN Expr ';'
         | PRINT Expr ';'
         | READ VarRefer ';'
         ;
ConditionStat: IF Expr THEN CompStat ELSE CompStat end IF
             | IF Expr THEN CompStat end IF
             ;

WhileStat: WHILE Expr DO CompStat end DO;

ForStat: FOR ID ASSIGN Expr TO Expr DO CompStat end DO;
ReturnStat: RETURN Expr ';'

FunCall: FunCall itemFunCall
        | itemFunCall
        ;
itemFunCall:  ID '(' ')' ';'
            | ID '(' ExprList ')' ';'
/* variable reference */
VarRefer: ID
        | ID Wrapped
        ;

Wrapped: Wrapped '[' Expr ']'
        | '[' Expr ']'
        ;


Expr: Expr itemExpr
    | itemExpr
    
    ;
itemExpr: CHAR
        | NUM
        | VarRefer
        | FunCallNotSem
        | AritExpr
        | LogicExpr
        ;

ExprList: ExprList ',' Expr
        | Expr
        ;


/*FunCallNotSem: FunCallNotSem itemFunCallNotSem
        | itemFunCallNotSem
        ;*/

FunCallNotSem:  ID '(' ')'
              | ID '(' ExprList ')'
              ;

AritExpr:     '-' AritExpr %prec '*'
            | '(' AritExpr ')'
            | AritExpr '+' AritExpr
            | AritExpr '-' AritExpr
            | AritExpr '*' AritExpr
            | AritExpr '/' AritExpr
            | AritExpr MOD AritExpr
            | NUM
            | CHAR
            | VarRefer
            | FunCallNotSem
            ;

LogicExpr:    LogicExpr '>' LogicExpr
            | LogicExpr '<' LogicExpr
            | LogicExpr '=' LogicExpr
            | LogicExpr GreaterEqual LogicExpr
            | LogicExpr LessEqual LogicExpr
            | LogicExpr NotEqual LogicExpr
            | LogicExpr AND LogicExpr
            | LogicExpr OR LogicExpr
            | NOT LogicExpr
            | '(' LogicExpr ')'
            | AritExpr
            ;


%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
