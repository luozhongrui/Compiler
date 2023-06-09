%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "AST/AstDumper.hpp"

#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

static AstNode *root;
static AstNode *declLink, *funcLink, *argLink, *formalArgLink, *varLink, *varDump;
static TypeSet *constTye;
AstNode *var;
ConstantValueNode *con;
struct CharList{
    uint32_t first_line;
    uint32_t first_column;
    char *id;
    struct CharList *next;
    CharList(uint32_t first_line, uint32_t first_column, char *id, CharList *next): first_line(first_line), first_column(first_column), id(id), next(next) {}

};
static struct CharList *idLink, *declTemp, *declConsTemp;
static std::vector<int> *arrDeclLink;
static AstNode *varConstDump, *varConstLink, *arrRefLink;
AstNode *statementLink;
struct ConstantInfo {
    uint32_t first_column;
    uint32_t first_line;
    const char *value;
    int intValue;
    float floatValue;
    const char *typenum;
    
    ConstantInfo(uint32_t first_line, uint32_t first_column, int intValue): first_column(first_column), first_line(first_line), intValue(intValue) {typenum = "integer";}
    ConstantInfo(uint32_t first_line, uint32_t first_column, float floatValue): first_column(first_column), first_line(first_line), floatValue(floatValue) {typenum = "real";}
    ConstantInfo(uint32_t first_line, uint32_t first_column, char *value): first_column(first_column), first_line(first_line), value(value) {
        if(strcmp(value, "true") == 0 || strcmp(value, "false") == 0){
            typenum = "boolean";
        }else {
            typenum = "string";
        }
    }
};

extern "C" int yylex(void);
static void yyerror(const char *msg);
ConstantValueNode *generateConstant(struct ConstantInfo *info);
struct TypeSet *generateType(struct TypeSet info);
extern int yylex_destroy(void);
%}

%code requires {
    #include <vector>
    class AstNode;
    class ExpressionNode;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    char *op;
    float num;
    int val;

    struct ConstantInfo *constInfo;
    std::vector<int> *arrDeclDim;
    struct TypeSet *typeSet;
    struct CharList *idlist;
    AstNode *node;
    AstNode *exprNode;
};

%type <identifier> ProgramName FunctionName
%type <idlist> IdList
%type <constInfo> LiteralConstant IntegerAndReal
%type <arrDeclDim> ArrDecl
%type <typeSet> ArrType Type

%type <identifier> ScalarType ReturnType NegOrNot
%type <identifier> StringAndBoolean

%type <node> DeclarationList FunctionList CompoundStatement 
%type <node> Declaration Declarations Functions Function
%type <node> FunctionDeclaration FunctionDefinition
%type <node> FormalArgList FormalArg FormalArgs

%type <node> Statement Simple Condition While For Return 
%type <node> StatementList Statements  FunctionCall  
%type <node>   ElseOrNot
%type <exprNode> ArrRefList ArrRefs Expression ExpressionList Expressions VariableReference FunctionInvocation
    /* Follow the order in scanner.l */

    /* Delimiter */
%token<identifier> COMMA SEMICOLON COLON
%token<identifier> L_PARENTHESIS R_PARENTHESIS
%token<identifier> L_BRACKET R_BRACKET

    /* Operator */
%token<op> ASSIGN
%left<op> OR
%left<op> AND
%right<op> NOT
%left<op> LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left<op> PLUS MINUS
%left<op> MULTIPLY DIVIDE MOD
%right<op> UNARY_MINUS

    /* Keyword */
%token<identifier> ARRAY BOOLEAN INTEGER REAL STRING
%token<identifier> END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token<identifier> DO ELSE FOR IF THEN WHILE
%token<identifier> DEF OF TO RETURN VAR
%token<identifier> FALSE TRUE
%token<identifier> PRINT READ

    /* Identifier */
%token<identifier> ID

    /* Literal */
%token<val> INT_LITERAL
%token<num> REAL_LITERAL
%token<identifier> STRING_LITERAL

%%

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {

        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1, $3, $4, $5);

    }
;

ProgramName:
    ID {$$ = $1;}
;

DeclarationList:
    Epsilon {
        $$ = NULL;}
    |
    Declarations {
        $$ = $1;}
;

Declarations:
    Declaration {
        declLink = $$ = $1;
        }
    |
    Declarations Declaration {
        auto tmp = $1;
        while(tmp->getNext())
                tmp = tmp->getNext();
        tmp->setNext($2);
        // declLink->setNext($2->getSelf()); 
        // declLink = declLink->getNext();
        }
;

FunctionList:
    Epsilon {$$ = NULL;}
    |
    Functions {$$ = $1;}
;

Functions:
    Function {$$ = $1; funcLink = $1;}
    |
    Functions Function {
        auto tmp = $1;
        while(tmp->getNext())
                tmp = tmp->getNext();
        tmp->setNext($2);
        // funcLink->setNext($2); 
        // funcLink = funcLink->getNext();
        }
;

Function:
    FunctionDeclaration {$$ = $1;}
    |
    FunctionDefinition {$$ = $1;}
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {   
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $5, $3);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $5, $3, $6);
    }
;

FunctionName:
    ID {$$ = $1;}
;

FormalArgList:
    Epsilon {$$ = NULL;}
    |
    FormalArgs {$$ = $1;}
;

FormalArgs:
    FormalArg {$$ = $1; argLink = $1;}
    |
    FormalArgs SEMICOLON FormalArg {
        auto tmp = $1;
        while(tmp->getNext())
                tmp = tmp->getNext();
        tmp->setNext($3);
        // argLink->setNext($3->getSelf());
        // argLink->getNext();
        }
;

FormalArg:
    IdList COLON Type {
        AstNode *varT = new VariableNode($1->first_line, $1->first_column, $1->id, $3);
        $$ = new DeclNode($1->first_line, $1->first_column, varT);
        formalArgLink = varT;
        $1 = $1->next;
        while($1){
            formalArgLink->setNext(new VariableNode($1->first_line, $1->first_column, $1->id, $3));
            formalArgLink = formalArgLink->getNext();
            $1 = $1->next;
        }

    }
;

IdList:
    ID { 
        idLink = $$ = new CharList(@1.first_line, @1.first_column, $1, NULL);
        } 
    |
    IdList COMMA ID {
        $$ = idLink;
        idLink->next = new CharList(@3.first_line, @3.first_column, $3, NULL);
        idLink = idLink->next;
        }
;

ReturnType:
    COLON ScalarType {$$ = $2;}
    |
    Epsilon{$$ = (char*)"void";}
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        declTemp = $2;
        varDump = varLink = new VariableNode($2->first_line, $2->first_column, $2->id, $4);
        $2 = $2->next;
        while($2){
            varLink->setNext(new VariableNode($2->first_line, $2->first_column, $2->id, $4));
            varLink = varLink->getNext();
            $2 = $2->next;
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, varDump);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        
        declConsTemp = $2;
        ConstantValueNode *conTemp = generateConstant($4);
        
        constTye =  generateType(conTemp->getType());
        
        varConstDump = varConstLink = new VariableNode($2->first_line, $2->first_column, $2->id, constTye, conTemp);
        $2 = $2->next;
        while($2){
            ConstantValueNode *conTemp = generateConstant($4);
            varConstLink->setNext(new VariableNode($2->first_line, $2->first_column, $2->id, constTye, conTemp));
            varConstLink = varConstLink->getNext();
            $2 = $2->next;
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, varConstDump);

    }
;

Type:
    ScalarType { 
        // printf("---type----\n");
        $$ = new TypeSet($1, {});
        // printf("---%s-%d-\n", $$->typeNum, $$->dim.size());
        }
    |
    ArrType {$$ = $1;}
    ;

ScalarType:
    INTEGER {
        // printf("----sclar---%s\n", $1);
        $$ = $1; }
    |
    REAL {$$ = $1;}
    |
    STRING {$$ = $1;}
    |
    BOOLEAN {$$ = $1;}
;

ArrType:
    ArrDecl ScalarType {$$ = new TypeSet($2, *$1);}
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        $$ = arrDeclLink = new std::vector<int>;
        $$->push_back($2);
        }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        $$ = arrDeclLink;
        arrDeclLink->push_back($3);
    } 
    ;

LiteralConstant:
    NegOrNot INT_LITERAL { 
        // printf("-----LiteralContant-----\n");
        if (strcmp($1, "1")==0) {
            // printf("-----LiteralContant-new--%d--\n", $2);
            $$ = new ConstantInfo(@2.first_line, @2.first_column, $2);
            // printf("--%s--%d---\n", $$->typenum, $$->intValue);
            // printf("-----LiteralContant-finish----\n");
        }else {
            $$ = new ConstantInfo(@1.first_line, @1.first_column, -$2);
        }
    }
    |
    NegOrNot REAL_LITERAL {
        if (strcmp($1, "1") == 0) {
            // printf("-----LiteralContant-real----\n");
            $$ = new ConstantInfo(@2.first_line, @2.first_column, $2);
            // printf("-----LiteralContant-real-finish---\n");
        }else {
            //  printf("-----LiteralContant-real--neg--\n");
            //  printf("-------%f----", $2);
            $$ = new ConstantInfo(@1.first_line, @1.first_column, -$2);
            // printf("---%s--negreal-...%f---",$$->typenum ,$$->floatValue);
        }
    }
    |
    StringAndBoolean {
        // printf("------stringAndBoolean----%s-", $1);
        $$ = new ConstantInfo(@1.first_line, @1.first_column, $1);
    }
;

NegOrNot:
    Epsilon {$$= (char*)"1";}
    |
    MINUS %prec UNARY_MINUS {$$ = (char*)"0";}
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = $1;
        }
    |
    TRUE {$$ = (char*)"true";}
    |
    FALSE {$$ = (char*)"false";}
;

IntegerAndReal:
    INT_LITERAL {$$ = new ConstantInfo(@1.first_line, @1.first_column, $1);}
    |
    REAL_LITERAL {$$ = new ConstantInfo(@1.first_line, @1.first_column, $1);}
    ;

    /*
       Statements
                  */

Statement:
    CompoundStatement {
        $$ = $1;}
    |
    Simple {$$ = $1;
    } 
    |
    Condition {$$ = $1;
    }
    |
    While {$$ = $1;
    }
    |
    For {$$ = $1;}
    |
    Return {$$ = $1;}
    |
    FunctionCall {$$ = $1;}
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END {$$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);}
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);}
    |
    PRINT Expression SEMICOLON {
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);}
    |
    READ VariableReference SEMICOLON {
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);}
;

VariableReference:
    ID ArrRefList {
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2, NULL);
    }
;

ArrRefList:
    Epsilon {$$ = NULL;}
    |
    ArrRefs {
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
    $$ = $2;
    arrRefLink = $2;
    
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        // printf("----ArrRefs-link Expression---\n");
        arrRefLink->setNext($3);
        arrRefLink = arrRefLink->getNext();
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {$$ = $2;}
    |
    Epsilon {
        $$ = NULL;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        // variableNode
        
        AstNode *intConst = generateConstant(new ConstantInfo(@4.first_line, @4.first_column, $4));
        TypeSet *consTyp = generateType(intConst->getType());
        AstNode *varab = new VariableNode(@2.first_line, @2.first_column, $2, consTyp, NULL);
        // DeclNode
        AstNode *declFor = new DeclNode(@2.first_line, @2.first_column, varab, NULL);
        // VariableReferenceNode
        AstNode *varRefer = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL, NULL);
        intConst = generateConstant(new ConstantInfo(@4.first_line, @4.first_column, $4));
        // AssignmentNode
        AstNode *assign = new AssignmentNode(@3.first_line, @3.first_column, varRefer, intConst, NULL);
        // ConstantValueNode
        intConst = generateConstant(new ConstantInfo(@6.first_line, @6.first_column, $6));
        $$ = new ForNode(@1.first_line, @1.first_column, declFor, assign, intConst, $8);
    }
;

Return:
    RETURN Expression SEMICOLON {$$ = new ReturnNode(@1.first_line, @1.first_column, $2);}
;

FunctionCall:
    FunctionInvocation SEMICOLON {
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon {$$ = NULL;}
    |
    Expressions {$$ = $1;}
;

Expressions:
    Expression {
        $$ = $1;
        // expressLink = $$;
        }
    |
    Expressions COMMA Expression {
        $1->setNext($3);
        $1 = $1->getNext();
    }
;

StatementList:
    Epsilon {$$ = NULL;}
    |
    Statements {$$ = $1;}
;

Statements:
    Statement {
        statementLink = $$ = $1;
        }
    |
    Statements Statement {
        // printf("----link statements----------\n");
        auto tmp = $1;
        while(tmp->getNext())
                tmp = tmp->getNext();
        tmp->setNext($2);

        // $$->setNext($2->getSelf());
        // statementLink->setNext($2->getSelf());
        // statementLink = statementLink->getNext(); 
    }
    ;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {$$ = $2;}
    |
    MINUS Expression %prec UNARY_MINUS {$$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2, NULL);}
    |
    Expression MULTIPLY Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression DIVIDE Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL); }
    |
    Expression MOD Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression PLUS Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression MINUS Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression LESS Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression LESS_OR_EQUAL Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression GREATER Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression GREATER_OR_EQUAL Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression EQUAL Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression NOT_EQUAL Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    NOT Expression {$$ = new UnaryOperatorNode(@1.first_line, @1.first_column, $1, $2, NULL);}
    |
    Expression AND Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    Expression OR Expression {$$ = new BinaryOperatorNode(@2.first_line, @2.first_column, $2, $1, $3, NULL);}
    |
    IntegerAndReal {
        $$ = generateConstant($1);
        }
    |
    StringAndBoolean {
         
        $$ = generateConstant(new ConstantInfo(@1.first_line, @1.first_column, $1));}
    |
    VariableReference  {$$ = $1;}
    |
    FunctionInvocation {$$ = $1;}
;

    /*
       misc
            */
Epsilon:
;

%%
ConstantValueNode *generateConstant(struct ConstantInfo *info){
    if (strcmp(info->typenum, "integer") == 0){
       return new ConstantValueNode(info->first_line, info->first_column, info->intValue);
    }
    if (strcmp(info->typenum, "real") == 0){
        return new ConstantValueNode(info->first_line, info->first_column, info->floatValue);
    }
    return new ConstantValueNode(info->first_line, info->first_column, info->value);
}

struct TypeSet *generateType(struct TypeSet info){
    // printf("func------%s---\n",info.typeNum);
    if (info.typeNum[0] == 'i'){
        return new TypeSet((char *)"integer", {});
    }
    if (info.typeNum[0] == 'r'){
        return new TypeSet((char *)"real", {});
    }
    if (info.typeNum[0] == 'b'){
        return new TypeSet((char *)"boolean", {});
    }
    return new TypeSet((char *)"string", {});
}

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
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        // root->print();
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
