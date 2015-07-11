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

void printError(int n);
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

