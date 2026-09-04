#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_MAX 100

/* Here I have implemented a basic compiler in C which supports a C like language(With no support for
 advanced stuff like libararies,etc..), to perform simple calculations like Addition(+), Subtraction(-), 
 Multiplication(x) and Division(/) of INTs, FLOATs and their NEGATIVES. 
 THANK YOU :) */

int lineNo = 1;

// LEXAR

typedef enum {
    TKN_INT, TKN_FLOAT, TKN_PRINT,
    TKN_ID, TKN_NUM,
    TKN_ASSIGN, TKN_PLUS, TKN_MINUS,
    TKN_MUL, TKN_DIV,
    TKN_SEMI, TKN_LPAREN, TKN_RPAREN,
    TKN_EOF
} LexType;

typedef struct {
    LexType type;
    char text[50];
} LexToken;

// SYMBOL TABLE

typedef struct {
    char name[50];
    double value;
} Sym;

Sym SymTabl[TABLE_MAX];
int SymCount = 0;

// GLOBALS

FILE *fp;
LexToken current;

// ERROR

void syntaxError(char *msg) {
    printf("Error at line %d: %s\n", lineNo, msg);
    exit(1);
}

// TOKENIZER

void skipSpaces() {
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if(c == '\n')
            lineNo++;

        if (!isspace(c)){
            ungetc(c, fp);
            break;
        }
    }
}

LexToken getNextToken(){
    LexToken t;

    skipSpaces();

    int c = fgetc(fp);

    if (c == EOF) {
        t.type = TKN_EOF;
        strcpy(t.text,"EOF");
        return t;
    }

    // KEYWORD
    if (isalpha(c)) {
        int i=0;
        t.text[i++] = c;

        while (isalnum(c = fgetc(fp))) {
            t.text[i++] = c;
        }

        t.text[i] = '\0';
        ungetc(c, fp);

        if(strcmp(t.text, "int")==0)
            t.type = TKN_INT;
        else if(strcmp(t.text, "float")==0)
            t.type = TKN_FLOAT;
        else if(strcmp(t.text, "print")==0)
            t.type = TKN_PRINT;
        else
            t.type = TKN_ID;

        return t;
    }

    // NUMBER (With INT, FLOAT and their NEGATIVES support)
    if(isdigit(c) || c=='.') {
        int i=0;
        t.text[i++] = c;

        while(isdigit(c = fgetc(fp)) || c=='.'){
            t.text[i++] = c;
        }
        t.text[i] = '\0';
        ungetc(c, fp);

        if(strcmp(t.text, ".")==0){
            syntaxError("Invalid number");
        }

        t.type = TKN_NUM;
        return t;
    }

    switch(c) {
    case '=' : t.type = TKN_ASSIGN; strcpy(t.text,"="); break;
    case '+' : t.type = TKN_PLUS; strcpy(t.text,"+"); break;
    case '-' : t.type = TKN_MINUS; strcpy(t.text,"-"); break;
    case '*' : t.type = TKN_MUL; strcpy(t.text,"*"); break;
    case '/' : t.type = TKN_DIV; strcpy(t.text,"/"); break;
    case ';' : t.type = TKN_SEMI; strcpy(t.text,";"); break;
    case '(' : t.type = TKN_LPAREN; strcpy(t.text,"("); break;
    case ')' : t.type = TKN_RPAREN; strcpy(t.text,")"); break;
    default: syntaxError("Invalid character");
}

    return t;
}

// MATCH

void match(LexType expected){
    if(current.type == expected){
        current = getNextToken();
    } else {
        char msg[100];
        sprintf(msg,"Unexpected token '%s'", current.text);
        syntaxError(msg);
    }
}

// SYMBOL TABLE

int findSym (char *name){
    for(int i=0; i<SymCount; i++) {
        if(strcmp(SymTabl[i].name,name)==0)
            return i;
    }
    return -1;
}

void setSym(char *name, double value) {
    int pos = findSym(name);

    if(pos != -1){
        syntaxError("Variable redeclared");
    }

    if(SymCount >= TABLE_MAX){
        syntaxError("Symbol table overflow");
    }

    strcpy(SymTabl[SymCount].name, name);
    SymTabl[SymCount].value = value;
    SymCount++;
}

double getSym (char *name) {
    int pos = findSym(name);
    if(pos == -1){
        char msg[100];
        sprintf(msg,"Variable '%s' not declared",name);
        syntaxError(msg);
    }
    return SymTabl[pos].value;
}

// PARSER

double parseExpr();

double parseFactor() {

    if(current.type == TKN_MINUS){
        match(TKN_MINUS);
        return -parseFactor();
    }

    if(current.type == TKN_NUM) {
        double val = atof(current.text);
        match(TKN_NUM);
        return val;
    }

    if(current.type == TKN_ID) {
        double val = getSym(current.text);
        match(TKN_ID);
        return val;
    }

    if(current.type == TKN_LPAREN){
        match(TKN_LPAREN);
        double val = parseExpr();
        match(TKN_RPAREN);
        return val;
    }

    syntaxError("Invalid factor");
    return 0;
}

double parseTerm() {
    double left = parseFactor();

    while(current.type == TKN_MUL || current.type == TKN_DIV){

        if(current.type == TKN_MUL){
            match(TKN_MUL);
            left *= parseFactor();
        }
        else{
            match(TKN_DIV);
            double d = parseFactor();
            if(d==0) syntaxError("Division by zero");
            left /= d;
        }
    }
    return left;
}

double parseExpr() {
    double left = parseTerm();

    while(current.type == TKN_PLUS || current.type == TKN_MINUS){

        if(current.type == TKN_PLUS){
            match(TKN_PLUS);
            left += parseTerm();
        }
        else{
            match(TKN_MINUS);
            left -= parseTerm();
        }
    }

    return left;
}

void parseDeclaration() {

    if(current.type == TKN_INT || current.type == TKN_FLOAT)
        match(current.type);
    else
        syntaxError("Expected type int or float");

    if(current.type != TKN_ID)
        syntaxError("Expected variable name");

    char name[50];
    strcpy(name, current.text);
    match(TKN_ID);

    if(current.type != TKN_ASSIGN)
        syntaxError("Missing '=' in declaration");

    match(TKN_ASSIGN);

    double value = parseExpr();

    if(current.type != TKN_SEMI)
        syntaxError("Missing ';'");

    match(TKN_SEMI);

    setSym(name, value);
}

void parsePrint(){
    match(TKN_PRINT);
    match(TKN_LPAREN);

    double value = parseExpr();

    match(TKN_RPAREN);

    if(current.type != TKN_SEMI)
        syntaxError("Missing ';'");

    match(TKN_SEMI);

    printf("%g\n", value);
}

void parseStatement() {
    if(current.type == TKN_INT || current.type == TKN_FLOAT)
        parseDeclaration();
    else if(current.type == TKN_PRINT)
        parsePrint();
    else
        syntaxError("Invalid Statement or Variable Type in the Program");
}

void parseProgram() {
    current = getNextToken();

    while(current.type != TKN_EOF) {
        parseStatement();
    }
}

// Main

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Usage: ./parser <file name>\n");
        return 1;
    }

    fp = fopen(argv[1], "r");
    if(!fp){
        printf("File open error. Please Try again\n");
        return 1;
    }

    parseProgram();

    fclose(fp);
    return 0;
}