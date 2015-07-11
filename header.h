#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//User Defined (Changeable)
#define nameMCode "mcode.txt"
#define nameTrace "stacktrace.txt"
#define nameCode "input.txt"
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

//ERROR_CODES
# define ERROR_USE_EQL 1
# define ERROR_NUM_EXPECTED 2
# define ERROR_EQL_EXPECTED 3
# define ERROR_MISSING_IDENT 4
# define ERROR_MISSING_SEMICOLON_COMMA 5
# define ERROR_SYM_EXPECTED 6
# define ERROR_STATEMENT_EXPECTED 7
# define ERROR_INCORRECT_SYM 8
# define ERROR_PERIOD_EXPECTED 9
# define ERROR_MISSING_SEMICOLON_BETWEEN_STATEMENTS 10
# define ERROR_UNDECLARED_IDENT 11
# define ERROR_ASSIGNMENT_TO_CONST_PROC 12
# define ERROR_BECOMESSYM_EXPECTED 13
# define ERROR_IDENT_EXPECTED_AFTER_CALL 14
# define ERROR_UNUSED_CALL 15
# define ERROR_THEN_EXPECTED 16
# define ERROR_SEMICOLON_RBRACKET_EXPECTED 17
# define ERROR_DO_EXPECTED 18
# define ERROR_INVALID_SYM_AFTER_STATEMENT 19
# define ERROR_RELATIONAL_OP_EXPECTED 20
# define ERROR_PROCEDURE_IN_EXPRESSION 21
# define ERROR_MISSING_RPARENT 22
# define ERROR_INVALID_SYM_FOLLOWING 23
# define ERROR_INVALID_EXPRESSION_START 24
# define ERROR_NUM_OVERFLOW 25
# define ERROR_INVALID_FILE 26
# define ERROR_INVALID_OP 27
# define ERROR_CODE_OVERFLOW 28
# define ERROR_IDENT_OVERFLOW 29
# define ERROR_INVALID_SYM 30
# define OUT_OF_MEMORY 31
# define HALT 32
# define ERROR_INVALID_ELSE 33
# define ERROR_IDENT_EXPECTED 34

#ifndef GLB
#define GLB

void printError(int n){
    switch(n){
        case 1:
            printf("An error has occurred: Use ""="" instead of "":="".\n");
            break;
        case 2:
            printf("An error has occurred: ""="" must be followed by a number.\n");
            break;
        case 3:
            printf("An error has occurred: Identifier must be followed by ""="".\n");
            break;
        case 4:
            printf("An error has occurred: ""const"",""var"",""procedure"" must be followed by ident.\n");
            break;
        case 5:
            printf("An error has occurred: "";"" or "","" missing.\n");
            break;
        case 6:
            printf("An error has occurred: Incorrect sym after ""procedure"" declaration.\n");
            break;
        case 7:
            printf("An error has occurred: Statement expected.\n");
            break;
        case 8:
            printf("An error has occurred: Incorrect sym after ""procedure"" declaration.\n");
            break;
        case 9:
            printf("An error has occurred: ""."" expected.\n");
            break;
        case 10:
            printf("An error has occurred: "";"" between statements expected.\n");
            break;
        case 11:
            printf("An error has occurred: Undeclared identifier.\n");
            break;
        case 12:
            printf("An error has occurred: Assignment to ""const"" or ""procedure"".\n");
            break;
        case 13:
            printf("An error has occurred: Assignment op expected.\n");
            break;
        case 14:
            printf("An error has occurred: identifier after ""call"" expected.\n");
            break;
        case 15:
            printf("An error has occurred: ""const"" or ""var"" call is unused.\n");
            break;
        case 16:
            printf("An error has occurred: ""then"" expected.\n");
            break;
        case 17:
            printf("An error has occurred: "";"" or }"" expected.\n");
            break;
        case 18:
            printf("An error has occurred: ""do"" expected.\n");
            break;
        case 19:
            printf("An error has occurred: invalid symbol after statement.\n");
            break;
        case 20:
            printf("An error has occurred: relational operator expected.\n");
            break;
        case 21:
            printf("An error has occurred: procedure in expression.\n");
            break;
        case 22:
            printf("An error has occurred: unclosed parenthesis.\n");
            break;
        case 23:
            printf("An error has occurred: invalid symbol following statement.\n");
            break;
        case 24:
            printf("An error has occurred: invalid start of expression.\n");
            break;
        case 25:
            printf("An error has occurred: number too large.\n");
            break;
        case 26:
            printf("An error has occurred: invalid filename.\n");
            break;
        case 27:
            printf("An error has occurred: invalid operation.\n");
            break;
        case 28:
            printf("An error has occurred: code too long.\n");
            break;
        case 29:
            printf("An error has occurred: variable indentifier too long.\n");
            break;
        case 30:
            printf("An error has occurred: invalid symbol.\n");
            break;
        case 31:
            printf("\nOut of Memory. Exiting . . .\n");
            break;
        case 32:
            printf("\nHalt has occurred.\n");
            return;
        case 33:
            printf("An error has occurred: Invalid use of ""else""\n");
            return;
        case 34:
            printf("An error has occurred: Variable expected.\n");
            return;
        case 35:
            printf("An error has occurred: Use "":="" instead of ""="".\n");
            break;
        default:
            printf("An error has occurred: ???.\n");
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

