//TODO: SYMBOL TABLE
#include "header.h"

//enums table
typedef enum{
	nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym,
	oddsym, eqlsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym,
	rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym,
	ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
	readsym, elsesym, errsym
} token;

//Lex table
typedef struct lexemeTable{
    char *lexeme;
    token tokenType;
    struct lexemeTable *next;
    struct lexemeTable *prev;
} lexTable;

//Files
FILE *fileCode;
FILE *fileCleanCode;
FILE *fileLexTable;
FILE *fileLexTableList;
FILE *hashCode;

int beginFlag = 0;
int ifFlag = 0, ifbeginFlag = 0, ifLevel = 0;

//Functions
lexTable *removeComments(int rflag);
void printLexeme(lexTable *lexs, int flag);
void printTable(lexTable *lexs);
void printList(lexTable *lexs, int flag);
void analyzeCode(lexTable *current, int flag);
lexTable *newLexeme(lexTable *lexs, char *lexeme, token t);
lexTable *statement(lexTable *current);
lexTable *expression(lexTable *current);
lexTable *factor(lexTable *current);
lexTable *term(lexTable *current);
lexTable *condition(lexTable *current);

void scanner(int flag){ //if flag is true (-l) print list of lexemes to screen

    fileCode = fopen(nameCode,"r");
    if(fileCode == NULL)
        printError(ERROR_INVALID_FILE);

    lexTable *start = NULL;

    start = removeComments(flag);
    printLexeme(start, flag);
    //printf("closing\n");
    fclose(fileCode);
}

lexTable *removeComments(int rflag){

    lexTable *front = NULL;
    char numHolder[numMax];
    char varHolder[identMax];
    int numLines = 0;
    int count = 0;


    char scanner;
    int flag = 0;
    int ignore_flag = 0;

    fileCleanCode = fopen(nameCleanCode,"w");
    if(fileCleanCode == NULL)
        printError(ERROR_INVALID_FILE);


    fscanf(fileCode, "%c", &scanner);
    while (scanner != EOF){
        if(flag == 1){ //if comment ignoring is active
            scanner = fgetc(fileCode);
            if (scanner == '*'){
                scanner = fgetc(fileCode);
                if(scanner == '/'){
                    flag = 0;
                    scanner = fgetc(fileCode);
                }
            }
        }
        else{
            if((int)scanner == 32 || ((int)scanner == 9)){ //space or tab
                fprintf(fileCleanCode," ");
                scanner = fgetc(fileCode);
            } //is space, do nothing.
            else if(((int)scanner == 10) || ((int)scanner == 59)){ //is newline or ';'
                fprintf(fileCleanCode,"%c", scanner);

                if(scanner == 59)
                    front = newLexeme(front,";", semicolonsym);

                numLines++;
                //analyzeCode(front);
                scanner = fgetc(fileCode);
                while(((int)scanner == 10) || ((int)scanner == 59) || ((int)scanner == 32) || ((int)scanner == 9) || ((int)scanner == 3)){
                    fprintf(fileCleanCode,"%c",scanner);
                    if(scanner == 59)
                        front = newLexeme(front,";", semicolonsym);
                    scanner = fgetc(fileCode);
                }
            }

            else if (((int)scanner >= 58 && (int)scanner <= 62) || ((int)scanner >= 40 && (int)scanner <= 47)) { //CASE: is a symbol
                switch(scanner){
                    case '+' :
                        front = newLexeme(front,"+", plussym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case '-' :
                        front = newLexeme(front,"-", minussym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case '*' :
                        front = newLexeme(front,"*",multsym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case '/' :
                        //might be a comment, stay tuned
                        scanner = fgetc(fileCode);
                        if(scanner == '*')
                            flag = 1;
                        else{
                            front = newLexeme(front,"/", slashsym);
                            fprintf(fileCleanCode,"/");
                        }
                        break;
                    case '=' :
                        front = newLexeme(front,"=", eqlsym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case ',' :
                        front = newLexeme(front,",", commasym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case '.' :
                        front = newLexeme(front,".", periodsym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case '>' :
                        scanner = fgetc(fileCode);
                        if (scanner == '='){ //matches '>='
                            front = newLexeme(front,">=", geqsym);
                            fprintf(fileCleanCode,">=");
                            scanner = fgetc(fileCode);
                        }
                        else{
                            front = newLexeme(front,">", gtrsym);
                            fprintf(fileCleanCode,">");
                        }
                        break;
                    case '(' :
                        front = newLexeme(front,"(", lparentsym);
                        fprintf(fileCleanCode,"(");
                        scanner = fgetc(fileCode);
                        break;
                    case ')' :
                        front = newLexeme(front,")", rparentsym);
                        fprintf(fileCleanCode,"%c", scanner);
                        scanner = fgetc(fileCode);
                        break;
                    case '<' :
                        scanner = fgetc(fileCode);
                        if (scanner == '='){ //matches '<='
                            front = newLexeme(front,"<=", leqsym);
                            fprintf(fileCleanCode,"<=");
                            scanner = fgetc(fileCode);
                        }
                        else if (scanner == '>'){ //matches <>
                            front = newLexeme(front,"<>", neqsym);
                            fprintf(fileCleanCode,"<>");
                            scanner = fgetc(fileCode);
                        }
                        else{
                            front = newLexeme(front,"<", lessym);
                            fprintf(fileCleanCode,"<");
                        }
                        break;
                    case ':' :
                        scanner = fgetc(fileCode);
                        if (scanner == '='){ //matches ':='
                            front = newLexeme(front,":=", becomessym);
                            fprintf(fileCleanCode,":=");
                            scanner = fgetc(fileCode);
                        }
                        else
                            printError(ERROR_INVALID_SYM);
                        break;
                    default:
                        printError(ERROR_INVALID_SYM);
                }
                if(scanner == EOF)
                    break;
            }

            else if ((int)scanner <= 57 && (int)scanner >= 48){ //CASE: is a number
               numHolder[0] = '\0';
               numHolder[0] = scanner;
               count = 1;
               ignore_flag = 0;

                while((int)scanner <= 57 && (int)scanner >= 48){
                    if(count >= numMax)
                        printError(ERROR_NUM_OVERFLOW);
                    scanner = fgetc(fileCode);
                    if(((int)scanner >= 58 && (int)scanner <= 62) || ((int)scanner >= 40 && (int)scanner <= 47) || ((int)scanner == 32) || ((int)scanner == 10) || ((int)scanner == 9) || ((int)scanner == 59)){//not a letter. must break!
                        ignore_flag = 1;
                    }
                    if(!ignore_flag)
                        numHolder[count++] = scanner;
                }
                numHolder[count] = '\0';

                front = newLexeme(front, numHolder, numbersym);
                fprintf(fileCleanCode,"%s", numHolder);

                if(!ignore_flag)
                    scanner = fgetc(fileCode);
            }

            else if ((int)scanner <= 122 && (int)scanner >= 97){ //CASE: is a letter (lowercase only)
                varHolder[0] = '\0';
                varHolder[0] = scanner;
                count = 1;
                ignore_flag = 0;


                while((((int)scanner <= 57 && (int)scanner >= 48) || ((int)scanner <= 122 && (int)scanner >= 97 )) && (int)scanner > 32){
                    if(count >= identMax)
                        printError(ERROR_IDENT_OVERFLOW);
                    if((int)scanner < 32)
                        break;
                    scanner = fgetc(fileCode);
                    if(((int)scanner >= 58 && (int)scanner <= 62) || ((int)scanner >= 40 && (int)scanner <= 47) || ((int)scanner == 32) || ((int)scanner == 10) || ((int)scanner == 9) || ((int)scanner == 59)){//not a letter. must break!
                        ignore_flag = 1;
                    }
                    if(!ignore_flag)
                        varHolder[count++] = scanner;
                }
                varHolder[count] = '\0';
                if((int)varHolder[count-1] < 32)
                    varHolder[count-1] = '\0';
                //figure out if you have a reserved name
                if (strcmp(varHolder, "begin") == 0){
                    front = newLexeme(front, "begin", beginsym);
                }
                else if (strcmp(varHolder, "call") == 0){
                    front = newLexeme(front, "call", callsym);
                }
                else if (strcmp(varHolder, "const") == 0){
                    front = newLexeme(front, "const", constsym);
                }
                else if (strcmp(varHolder, "var") == 0){
                    front = newLexeme(front, "var", varsym);
                }
                else if (strcmp(varHolder, "do") == 0){
                    front = newLexeme(front, "do", dosym);
                }
                else if (strcmp(varHolder, "else") == 0){
                    front = newLexeme(front, "else", elsesym);
                }
                else if (strcmp(varHolder, "end") == 0){
                    front = newLexeme(front, "end", endsym);
                }
                else if (strcmp(varHolder, "if") == 0){
                    front = newLexeme(front, "if", ifsym);
                }
                else if (strcmp(varHolder, "write") == 0){
                    front = newLexeme(front, "write", writesym);
                }
                else if (strcmp(varHolder, "procedure") == 0){
                    front = newLexeme(front, "procedure", procsym);
                }
                else if (strcmp(varHolder, "then") == 0){
                    front = newLexeme(front, "then", thensym);
                }
                else if (strcmp(varHolder, "while") == 0){
                    front = newLexeme(front, "while", whilesym);
                }
                else if (strcmp(varHolder, "read") == 0){
                    front = newLexeme(front, "read", readsym);
                }
                else if (strcmp(varHolder, "odd") == 0){
                    front = newLexeme(front, "odd", oddsym);
                }
                else{
                    front = newLexeme(front, varHolder, identsym);
                    }
                fprintf(fileCleanCode,"%s", varHolder);

                if(!ignore_flag)
                    scanner = fgetc(fileCode);
            }
            else //might be invalid symbol
                scanner = EOF;
        }
    }
    analyzeCode(front, rflag);
    fclose(fileCleanCode);
    return front;
}

void analyzeCode(lexTable *front, int flag){
    lexTable *current = front;
    int rwflag = 0;
    int procFlag = 0;

    //printf("got here.\n");

    front = newLexeme(front, "err", errsym); //Padding! You need that last pos occupied with a dummy.

    while(current->next){ //"block"
        switch(current->tokenType){
            case procsym: //"procedure"
                procFlag++;
                current = current->next;
                if(current->tokenType != identsym) // "ident"
                    printError(4);
                current = current->next;
                if(current->tokenType != semicolonsym) // ";"
                    printError(5);
                current = current->next;
                break; //Now loop for "block"

            case constsym: // "const-declaration"
                current = current->next;
                if(current->tokenType != identsym) // "ident"
                    printError(4);
                current = current->next;
                if(current->tokenType != eqlsym) // "="
                    printError(3);
                current = current->next;
                if(current->tokenType != numbersym) // "number"
                    printError(2);
                current = current->next;
                while(current->tokenType == commasym){ // OPTIONAL ","
                    current = current->next;
                    if(current->tokenType != identsym) // "ident"
                        printError(ERROR_MISSING_IDENT);
                    current = current->next;
                    if(current->tokenType == becomessym) // ERROR ":=" used
                        printError(1);
                    if(current->tokenType != eqlsym) // "="
                        printError(3);
                    current = current->next;
                    if(current->tokenType != numbersym) // "number"
                        printError(2);
                    current = current->next;
                }
                if(current->tokenType != semicolonsym) // ";"
                    printError(5);
                current = current->next;
                break;

            case varsym: // "var-declaration"
                current = current->next;
                if(current->tokenType != identsym) // "ident"
                    printError(4);
                current = current->next;
                if(current->tokenType == commasym){ // OPTIONAL ","
                    while(current->tokenType == commasym){
                        current = current->next;
                        if(current->tokenType != identsym) // "ident"
                            printError(ERROR_MISSING_IDENT);
                        current = current->next;
                    }
                }
                if(current->tokenType != semicolonsym) // ";"
                    printError(5);
                current = current->next;
                break;

            case identsym:
            case beginsym:
            case ifsym:
            case whilesym:
            case readsym:
            case writesym: // "statement"
                if((current->tokenType == readsym) || (current->tokenType == writesym))
                    rwflag = 1;
                if(current->tokenType == beginsym)
                    beginFlag++;
                current = statement(current);
                if(rwflag){
                    if(current->tokenType != semicolonsym) // ";"
                        printError(5);
                    current = current->next;
                    rwflag = 0;
                }
                break;

            case semicolonsym: //Assuming this is the end of a begin
                if(beginFlag == 0)
                    printError(5);
                current = current->next;
                procFlag--;
                break;

            case periodsym:
                current->next = NULL;
                break;

            case elsesym:
                current = statement(current);
                break;

            case endsym:
                current = current->next;
                break;

            default:
                printf("error with %s.", current->lexeme);
                printError(-52);
        }
    }
    //printf("ended branch.\n");
    if(current->tokenType != periodsym) //end "."
        printError(9);

    current = front;
    while(current->next != NULL){
        current = current->next;
    }
    //printf("ended whole.\n");
}
lexTable *statement(lexTable *current){ // "statement"
        if(!current->next || !current)
            return(current);
        switch(current->tokenType){
            case identsym: // "ident"
                current = current->next;
                if(current->tokenType != becomessym) // ":="
                    printError(ERROR_BECOMESSYM_EXPECTED);
                current = current->next;
                current = expression(current); //"expression"
                break;

            case beginsym: // "begin"
                beginFlag++;
                current = current->next;
                current = statement(current); // "statement"
                while(current->tokenType == semicolonsym){ // ";"
                    if(!current->next)
                        break;
                    current = current->next;
                    if(current->tokenType == endsym)
                        break;
                    current = statement(current); // "statement"
                }
                if(current->tokenType == elsesym){
                    current = statement(current);
                }
                if(current->tokenType != endsym) // "end"
                    printError(19);
                if(current->tokenType == endsym){
                    beginFlag--;
                    current = current->next;
                }
                break;

            case ifsym: // "if"
                current = current->next;
                current = condition(current); // "Condition"
                if(current->tokenType != thensym) // "then"
                    printError(ERROR_THEN_EXPECTED);
                ifFlag++;
                ifLevel = beginFlag;
                current = current->next;
                current = statement(current); // "statement"
                break;

            case elsesym: // "else"
                //Test for VALID else, ie. a if then exists
                if(!ifFlag)
                    printError(33);
                ifFlag--;
                if(ifLevel != beginFlag)
                    printError(33);
                current = current->next;
                current = statement(current); // "statement"
                break;

            case whilesym: // "while"
                current = current->next;
                current = condition(current); // "Condition"
                if(current->tokenType != dosym) // "do"
                    printError(ERROR_DO_EXPECTED);
                current = current->next;
                current = statement(current); // "statement"
                break;

            case writesym: // "write" OR
            case readsym: // "read"
                current = current->next;
                if(current->tokenType != identsym) // "ident"
                    printError(-51);
                current = current->next;
                break;

            case callsym: // "call" AFTER proc!
                current = current->next;
                if(current->tokenType != identsym) // "ident"
                    printError(-51);
                current = current->next;
                break;

            default:
                printError(7);
        }
    return current;
}


lexTable *expression(lexTable *current){ // "expression"
    if(!current->next)
        return(current);
    if(current->tokenType == plussym || current->tokenType == minussym)
        current = current->next;   //OPTIONAL "+" or "-"
    current = term(current); // "term"
    while((current->tokenType == plussym) || (current->tokenType == minussym)){
        current = current->next;   //OPTIONAL LOOP "+" or "-"
        current = term(current); // "term"
    }
    return current;
}

lexTable *term(lexTable *current){ // "term"
    if(!current->next)
        return(current);
    current = factor(current); // "factor"
    while((current->tokenType == multsym)||(current->tokenType == slashsym)){ // OPTIONAL "*" OR "/"
        current = current->next;
        current = factor(current); // "factor"
    }
    return current;
}

lexTable *factor(lexTable *current){ // "factor"
    if(!current->next)
            return(current);
    if((current->tokenType == identsym) || (current->tokenType == numbersym)){
        current = current->next;   /* "ident" OR "number" */ }
    else if(current->tokenType == lparentsym){// OR "("
        current = current->next;
        current = expression(current); // "expression"
        if(current->tokenType != rparentsym) // ")"
            printError(ERROR_MISSING_RPARENT);
        current = current->next;
    }
    else
        printError(-54);
    return current;
}

lexTable *condition(lexTable *current){ // "condition"
    if(!current->next)
        return(current);
    if(current->tokenType == oddsym){ // "odd"
        current = current->next;
        current = expression(current); // "expression"
    }
    else{ // ORR
       current = expression(current); // "expression"
       if(current->tokenType == becomessym)
            printError(1);
       if((current->tokenType != eqlsym) && (current->tokenType != neqsym) && (current->tokenType != gtrsym) && (current->tokenType != lessym) && (current->tokenType != leqsym) && (current->tokenType != geqsym))
            printError(-50); // "rel-op"
       current = current->next;
       current = expression(current); // "expression"
    }
    return current;
}

void printLexeme(lexTable *lexs, int flag){
    fileLexTable = fopen(nameLexTable,"w");
    if(fileLexTable == NULL)
        printError(OUT_OF_MEMORY);

    printTable(lexs);

    fclose(fileLexTable);

    fileLexTableList = fopen(nameLexTableList,"w");
    if(fileLexTableList == NULL)
        printError(OUT_OF_MEMORY);

    printList(lexs, flag);

    fclose(fileLexTableList);
}

void printTable(lexTable *lexs){
    lexTable *current = lexs;

    fprintf(fileLexTable,"lexeme      token type\n");
    while(current){
        fprintf(fileLexTable,"%-11s %d\n", current->lexeme, current->tokenType);
        current = current->next;
    }
}

void printList(lexTable *lexs, int flag){
    lexTable *current = lexs;
    int i;
    if(flag)
        printf("\nLexeme List:\n");
    //printf("dummy for loop\n");
    for(i=0; i<9999; i++){
        if(current == NULL)
            break;
        if(flag == 1){
            printf("%d ", current->tokenType);
            if(current->tokenType == identsym || current->tokenType == numbersym){
                printf("%s ", current->lexeme);
            }
        }
        fprintf(fileLexTableList,"%d ", current->tokenType);
        if(current->tokenType == identsym || current->tokenType == numbersym){
            fprintf(fileLexTableList,"%s ", current->lexeme);
        }
        current = current->next;
    }
    if(flag)
        printf("\n");
}


lexTable *newLexeme(lexTable *lexs, char *lexeme, token t){
    lexTable *current = lexs;
    lexTable *prev = NULL;

    if(!lexs){
        lexs = (lexTable *)malloc(sizeof(lexTable));
        if(!lexs)
            printError(OUT_OF_MEMORY);
        current = lexs;
    }
    else{
        while(current->next){
            current = current->next;
        }
        current->next = (lexTable *)malloc(sizeof(lexTable));
        if (!current->next){
            current->next = NULL;
            return lexs;
        }
        prev = current;
        current = current->next;
    }
    current->lexeme = (char *)malloc(strlen(lexeme) +1);
    strcpy(current->lexeme, lexeme);
    current->tokenType = t;
    current->next = NULL;
    current->prev = prev;
    return lexs;
}
