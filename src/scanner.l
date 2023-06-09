%{
#include <stdint.h>
#include <string.h>

#define LIST                concatenateString(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s%s>\n", #t, yytext); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
static char buffer[MAX_LINE_LENG];
static char buffer_string[MAX_LINE_LENG];
static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *buffer_ptr = buffer;
uint32_t string_flag = 1;
static void concatenateString(const char *yytext_ptr);
static void replacequote(char *bufferString);

%}
blank       " "
delimiter   [,;\:\(\)\[\]]
digit       [0-9]
letter      [a-zA-Z]
op          [\+\-\*\/\=\<\>]|"mod"|":="|"<="|"<>"|">="|"and"|"or"|"not"
keyword     "var"|"def"|"array"|"of"|"boolean"|"integer"|"real"|"string"|"true"|"false"|"while"|"do"|"if"|"then"|"else"|"for"|"to"|"begin"|"end"|"print"|"read"|"return"
identifier  [a-zA-Z]([a-zA-Z0-9])*
octal       0([0-7]+)
integer     ([1-9]{digit}*)|(0)
float       [1-9]{digit}*\.({digit}*)([1-9]+)|({digit}+\.0)
scientific  ([1-9]+)((\.?{digit}*)|(\.{digit}+))(([Ee][0])|([Ee][+-]?[1-9]+{digit}*))
string      ((\"\")|(\32)|(\t)|[^\"])*
%Start COMMENT COMMENT1 STRING
%%
<INITIAL>"/*"   {LIST; BEGIN COMMENT;}
<COMMENT>"*/"   {LIST; BEGIN INITIAL;}
<COMMENT>.      {LIST;}
<INITIAL>"//"   {LIST; BEGIN COMMENT1;}
<COMMENT1>\n    {BEGIN INITIAL; REJECT;}
<COMMENT1>.     {LIST;}
<INITIAL>"//&S-" {opt_src = 0; REJECT;}
<INITIAL>"//&S+" {opt_src = 1; REJECT;}
<INITIAL>"//&T-" {opt_tok = 0; REJECT;}
<INITIAL>"//&T+" {opt_tok = 1; REJECT;}
\t                       {LIST;}
{blank}                  {LIST;}
<INITIAL>{keyword}       {TOKEN(KW);}
<INITIAL>{op}            {TOKEN();}
<INITIAL>{delimiter}     {TOKEN_CHAR(yytext[0]);}
<INITIAL>{octal}         {TOKEN_STRING(oct_integer, yytext);}
<INITIAL>{scientific}    {TOKEN_STRING(scientific, yytext);}
<INITIAL>{integer}       {TOKEN_STRING(integer, yytext);}
<INITIAL>{float}         {TOKEN_STRING(float, yytext);}
<INITIAL>{identifier}    {TOKEN_STRING(id, yytext)}
<INITIAL>\"              {LIST; BEGIN STRING;}
<STRING>\"               {LIST; BEGIN INITIAL; if(string_flag){
                                                printf("<string: >\n");}string_flag = 1;}
<STRING>{string}  {LIST; memcpy(buffer_string, yytext, strlen(yytext)+1); replacequote(buffer_string); string_flag = 0;}


\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void concatenateString(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *buffer_ptr = *yytext_ptr;
        ++buffer_ptr;
        ++yytext_ptr;
    }
    *buffer_ptr = '\0';
}

static void replacequote(char *bufferString){
    int len = strlen(bufferString);
    for(int i = 0; i <= len; i++){
        if(bufferString[i]=='"' && bufferString[i+1] == '"'){
            int j = i;
            while(bufferString[j+1]!='\0'){
                bufferString[j]=bufferString[j+1];
                j++;
            }
            bufferString[j] = '\0';
        }
    }
    if (opt_tok){
        printf("<string: %s>\n", bufferString);
        }
    memset(bufferString, '\0', len+1);
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}
