#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//User Defined (Changeable)
#define nameMCode "mcode.txt"
#define nameTrace "stacktrace.txt"
#define nameCode "input.pl0"
#define nameCleanCode "cleaninput.txt"
#define nameLexTable "lexemetable.txt"
#define nameLexTableList "lexemelist.txt"

//Constants
#define MAX_STACK_HEIGHT 2000
#define MAX_SYMBOL_TABLE_SIZE 2000
#define MAX_CODE_LENGTH 2000
#define MAX_LEXI_LEVELS 3
#define identMax 12
#define numMax 6

#ifndef GLB
#define GLB

void printError(int n){
    switch(n){
        case 1:
            printf("Invalid file input\n");
            break;
        case 2:
            printf("Use ""="", not "":=""\n");
            break;
        case 3:
            printf("Use "":="", not ""=""\n");
            break;
        case 4:
            printf("""="" expected after const declaration\n");
            break;
        case 5:
            printf("Number expected after ""="" with const\n");
            break;
        case 6:
            printf("""then"" expected after ""if""\n");
            break;
        case 7:
            printf("""do"" expected after ""while""\n");
            break;
        case 8:
            printf("const, var, and procedure must be followed by an identifier\n");
            break;
        case 9:
            printf(""":="" expected after identifier\n");
            break;
        case 10:
            printf("Ident expected after ""call""\n");
            break;
        case 11:
            printf("Relational operator expected\n");
            break;
        case 12:
            printf("Assignment to constants and procedures not allowed\n");
            break;
        case 13:
            printf("Semicolon needed between statements.\n");
            break;
        case 14:
            printf("Cannot begin statement with this symbol\n");
            break;
        case 15:
            printf("Undeclared variable detected\n");
            break;
        case 16:
            printf("Unclosed parenthesis detected\n");
            break;
        case 17:
            printf("Invalid operator\n");
            break;
        case 18:
            printf("Invalid symbol\n");
            break;
        case 19:
            printf(""";"" expected\n");
            break;
        case 20:
            printf("Number too long\n");
            break;
        case 21:
            printf("Identifier too long\n");
            break;
        case 22:
            printf("Generated code too long\n");
            break;
        case 23:
            printf("Compiler has run out of memory\n");
            break;
        case 24:
            printf("Period expected\n");
            break;
        case 25:
            printf("Var or const detected more than once\n");
            break;
        default:
            printf("An error has occurred.\n");
        }
    exit(n);
}

int hashMe(char *name);

typedef struct symbol {
    int kind; // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
} symTable;

//Struct
typedef struct instr{
	int OP;
	int L;
	int M;
} instruction;
/*
For constants, you must store kind, name and value.
For variables, you must store kind, name, L and M.
For procedures, you must store kind, name, L and M.
*/

//Lex table
typedef struct tokens{
    int type;
    char name[identMax];
} Token;

Token tokenList[MAX_CODE_LENGTH];

#endif // GLB

