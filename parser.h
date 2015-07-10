#include "header.h"

//Files
FILE *fileCode;
FILE *fileLexTable;
FILE *readFile;

//Struct
typedef struct variable{
    char name[identMax];
    int type;
} varArray;

//Global variables
symTable thisTable[MAX_SYMBOL_TABLE_SIZE];
int nStack = 4;
int lines = 0;
int toggleBacklog = 0;
int backlogPos = 0;
int backlog[999][3];
int read = 1;

//Functions
void printSymTable();
void printMCode();
void createSym();
void generateMCode();
void handleBecomeSym(varArray vars[]);
void handleOperation(int sym);
void handleIdent(varArray vars[]);
void handleNumber();
void handleRead(varArray vars[]);
void handleWrite(varArray vars[]);
int findReturnPos(char varname[], varArray vars[]);
void handleParenthesis(varArray vars[]);
void handleIf(varArray vars[]);
void handleWhile(varArray vars[]);
void handleOdd(varArray vars[]);
void printBacklog(int beginValue);

//Main function
void parser(int flag){

    createSym();
    printf("\nNo errors, program is syntactically correct!\n\n");

    //printSymTable();

    generateMCode();

    if(flag)
        printMCode();

}

void printSymTable(){
    int i;
    printf("\nSymbol Table:\n");
    for(i=0; i<MAX_SYMBOL_TABLE_SIZE; i++){
        printf("%d %s %d %d %d \n", thisTable[i].kind, thisTable[i].name, thisTable[i].val, thisTable[i].level, thisTable[i].addr);
    }
    printf("\n\n");
}

void printMCode(){
    readFile = fopen(nameMCode,"r");
    char c;
    c = getc(readFile);
    printf("Machine Code Generated:\n");
    while(c != EOF){
        printf("%c", c);
        c = getc(readFile);
    }
}

void createSym(){
    int sym;//don't remove int i
    int position = 0, L = 0, scanFlag = 0, commaFlag = 0, commaFlag2 = 0;
    char name[identMax];
    char c;

    fileLexTable = fopen(nameLexTableList,"r");
    if(fileLexTable == NULL)
        printError(ERROR_INVALID_FILE);

    //printf("\n\nwelcome to sym!\n");

    c = 'a'; // set dummy to make sure default value isn't EOF
    while (c != EOF){ //Create the symbol table
        if(scanFlag != 1){
            fscanf(fileLexTable,"%d", &sym);
        }
        else if(scanFlag == 1){
            scanFlag = 0;
        }
        if(commaFlag == 1)
            sym = 28;
        if(commaFlag2 == 1)
            sym = 29;
        //printf("o[%d]\n", sym);
        if(sym == 21) //begin
            L++;
        else if(sym == 22) //end
            L--;
        else if(sym == 29){//var
                sym = 0;
                if(commaFlag2 == 1)
                    commaFlag2 = 0;
                else{
                    fscanf(fileLexTable,"%d", &sym); // get rid of dummy "2"
                }
                //printf("2='%d'\n", sym);
                fscanf(fileLexTable,"%s",name);
                //printf("'%s'\n", name);
                position = hashMe(name);
                    thisTable[position].kind = 2;
                    thisTable[position].level = L;
                    strcpy(thisTable[position].name,name);
                fscanf(fileLexTable,"%d", &sym);
                scanFlag = 1;
                if(sym == commasym){
                    fscanf(fileLexTable,"%d", &sym);
                    commaFlag2 = 1;
                }
        }
        else if(sym == 28){ //CONST If commas found, do it all again.
            commaFlag = 0;
            //printf("28[%d] = ", sym);
            fscanf(fileLexTable,"%d", &sym); //ON PURPOSE, get rid of '2'
            fscanf(fileLexTable,"%s",name); //varname
            //printf("'%s' = ", name);
            fscanf(fileLexTable,"%d", &sym); // "9"
            fscanf(fileLexTable,"%d", &sym); // "3"
            fscanf(fileLexTable,"%d", &sym); // "##"
            //printf("%d\n", sym);
            position = hashMe(name);
            thisTable[position].kind = 1;
            thisTable[position].level = L;
            strcpy(thisTable[position].name,name);
            thisTable[position].val = sym;
            scanFlag = 1;
            fscanf(fileLexTable,"%d", &sym); // "sym"
            if(sym == 17)
                commaFlag = 1;
        }
        else if (sym == numbersym){
            fscanf(fileLexTable,"%d", &sym); //value of num
            printf("grabbed number %d.\n", sym);
        }
        else if(sym == 2){
            fscanf(fileLexTable,"%s",name);
            c = fgetc(fileLexTable);
            position = hashMe(name); //Test to see if declared earlier
            printf("%d declared earlier? %s:\n", sym, name);
            if((thisTable[position].level > L) || (thisTable[position].name[0] == NULL)){
                printError(11);
            }
        }
        else if(sym == procsym){
            scanFlag = 0;
            fscanf(fileLexTable,"%d", &sym); // "2"
            fscanf(fileLexTable,"%s",name); //varname
            position = hashMe(name);
            thisTable[position].kind = 3;
            thisTable[position].level = L;
            strcpy(thisTable[position].name,name);
            scanFlag = 1;
            fscanf(fileLexTable,"%d", &sym);
        }
        else if(sym == periodsym){
            c = EOF;
        }
        //printf("loop ");
    }
    //printf("completed.\n");
    fclose(fileLexTable);
}

int hashMe(char name[]){ //The hash function will be (length *E(char * char's position in string)) % MAX_SIZE
    int i, length=0, hashValue = 0;
    length = strlen(name);
    for(i=0; i<(length); i++){
        hashValue += (int)name[i]*(i+1);
        if(i%1 == 0)
            hashValue *= 3;
    }
    hashValue *= length;
    hashValue %= MAX_SYMBOL_TABLE_SIZE;
    return hashValue;
}

void generateMCode(){
    int i, j = 0,numVar = 0, numConst = 0, hashy=0;
    varArray vars[100];
    int sym = 0, procFlag = 0, lines = 0;
    char varname[identMax];
    fileCode = fopen(nameMCode,"w");
    if(fileCode == NULL)
        printError(ERROR_INVALID_FILE);
    fileLexTable = fopen(nameLexTableList,"r");

    //1) find main procedure
    fscanf(fileLexTable,"%d", &sym);
    //printf("%d != %d, && %d > 0\n", sym, beginsym, procFlag);
    while(sym != beginsym){
        //printf("%d lines: Found [%d]\n",lines, sym);
        if(sym == procsym)
            procFlag ++;
        else if(procsym > 0 && sym == endsym)
            procFlag--;
        else if((sym == beginsym) || (sym == semicolonsym))
            lines++;
        if(sym == 2){
            fscanf(fileLexTable,"%s", varname);
            //printf("%d lines: Found [%s]\n",lines, varname);
            fscanf(fileLexTable,"%d", &sym);
        }
        else
            fscanf(fileLexTable,"%d", &sym);
        if((sym == beginsym) && (procFlag > 0)){
            lines++;
            //printf("%d lines: Found [%d]\n",lines, sym);
            fscanf(fileLexTable,"%d", &sym);
        }
    }
    //2) count number of const variables, log their pos into array
        for(i=0; i<MAX_SYMBOL_TABLE_SIZE; i++){
            if(thisTable[i].kind == 1){
                strcpy(vars[j].name,thisTable[i].name);
                vars[j++].type = 2;
                numConst++;
            }
        }
        //count regular vars
        for(i=0; i<MAX_SYMBOL_TABLE_SIZE; i++){
            if(thisTable[i].kind == 2){
                strcpy(vars[j].name,thisTable[i].name);
                vars[j++].type = 1;
                numVar++;
            }
        }
    //3) initialize program with 6 0 # found above
    fprintf(fileCode,"6 0 %d\n",4+j);
    lines++;
    nStack = 4+j;
    //3a) add const values into stack!
    j = 0;
    for(i=0; i<MAX_SYMBOL_TABLE_SIZE; i++){
        if(thisTable[i].kind == 1){
            hashy = hashMe(thisTable[i].name);
            fprintf(fileCode,"1 0 %d\n",thisTable[hashy].val);
            lines++;
            fprintf(fileCode,"4 0 %d\n",4+j);
            lines++;
            j++;
        }
    }
    //4) go through main lines as such
    while(sym != periodsym){
        if(read)
            fscanf(fileLexTable,"%d", &sym);
        else
            read = 1;
        printf("main loop. sym: %d. if: %d.\n", sym, ifsym);
        switch(sym){

            case identsym: // var := ????
                handleBecomeSym(vars);
                break;

            case readsym:
                handleRead(vars);
                break;

            case writesym:
                handleWrite(vars);
                break;

            case ifsym:
                handleIf(vars);
                break;

            case whilesym:
                handleWhile(vars);
                break;
        }
    }
    //5) halt/end.
    fprintf(fileCode,"9 0 2\n");
    lines++;

    fclose(fileCode);
}

void handleIf(varArray vars[]){
    int sym, temp = -1, currentLines = 0, currentBacklog = 0, currentBacklog2 = 0;
    int op = 0, singleLine = 0, tempy = 0;
    while(sym != eqlsym && sym != leqsym && sym != lessym && sym != geqsym && sym != gtrsym && sym != neqsym){ //while not comparison sym
        fscanf(fileLexTable,"%d", &sym);//find next sym
        printf("??? %d.\n", sym);
        switch(sym){

            case numbersym:
                handleNumber();
                if(temp != -1){
                    handleOperation(temp);
                    temp = -1;
                }
                break;

            case identsym:
                handleIdent(vars);
                if(temp != -1){
                    handleOperation(temp);
                    temp = -1;
                }
                break;

            case plussym:
            case minussym:
            case slashsym:
            case multsym:
                temp = sym;
                break;

            case lparentsym:
                handleParenthesis(vars);
                if(temp != -1){
                    handleOperation(temp);
                    temp = -1;
                }
                break;
        }
    }
    op = sym;
    while(sym != thensym){ //while not "then"
        fscanf(fileLexTable,"%d", &sym);//find next sym
        printf("??? %d.\n", sym);
        switch(sym){

            case numbersym:
                handleNumber();
                if(temp != -1){
                    handleOperation(temp);
                    temp = -1;
                }
                break;

            case identsym:
                handleIdent(vars);
                if(temp != -1){
                    handleOperation(temp);
                    temp = -1;
                }
                break;

            case plussym:
            case minussym:
            case slashsym:
            case multsym:
                temp = sym;
                break;

            case lparentsym:
                handleParenthesis(vars);
                if(temp != -1){
                    handleOperation(temp);
                    temp = -1;
                }
                break;
        }
    }
    handleOperation(op); //handle the comparison
    fscanf(fileLexTable,"%d", &sym);//???
    printf("??? %d.\n", sym);
    currentLines = lines;
    currentBacklog = backlogPos;
    toggleBacklog++;//store in backlog.
    if(sym == beginsym){ // "begin"
        //do all REGULAR OPERATIONS.
        while(sym != endsym){
            if(read)
                fscanf(fileLexTable,"%d", &sym);
            else
                read = 1;
            switch(sym){

                case identsym: // var := ????
                    handleBecomeSym(vars);
                    break;

                case readsym:
                    handleRead(vars);
                    break;

                case writesym:
                    handleWrite(vars);
                    break;

                case ifsym:
                    handleIf(vars);
                    break;

                case whilesym:
                    handleWhile(vars);
                    break;
            }
        }
        fscanf(fileLexTable,"%d", &sym);
        printf("%d else or ??? %d.\n", elsesym, sym);
    } // VVV should now be on ??? ORR "else" VVV
    else{ //One line ONLY
        switch(sym){

                case identsym: // var := ????
                    handleBecomeSym(vars);
                    break;

                case readsym:
                    handleRead(vars);
                    break;

                case writesym:
                    handleWrite(vars);
                    break;

                case ifsym:
                    handleIf(vars);
                    break;

                case whilesym:
                    handleWhile(vars);
                    break;
        }
        fscanf(fileLexTable,"%d", &sym);
        printf("%d else or ??? %d.\n", elsesym, sym);
        // ^^^ should now be on ??? ORR "else" ^^^
    }
    //Now test for "else"
    if(sym == semicolonsym){
        fscanf(fileLexTable,"%d", &sym);
    }
    singleLine = 1;
    printf("%d else or ??? %d.\n", elsesym, sym);
    if(sym == elsesym){
        currentBacklog2 = backlogPos;
        //!SCAN BACKLOG
        fscanf(fileLexTable,"%d", &sym);
        if(sym == beginsym){ // "begin"
            //do all REGULAR OPERATIONS.
            while(sym != endsym){
                if(read)
                    fscanf(fileLexTable,"%d", &sym);
                else
                    read = 1;
                switch(sym){

                    case identsym: // var := ????
                        handleBecomeSym(vars);
                        break;

                    case readsym:
                        handleRead(vars);
                        break;

                    case writesym:
                        handleWrite(vars);
                        break;

                    case ifsym:
                        handleIf(vars);
                        break;

                    case whilesym:
                        handleWhile(vars);
                        break;
                }
            }
            fscanf(fileLexTable,"%d", &sym);
            printf("%d else or ??? %d.\n", elsesym, sym);
        } // VVV should now be on ??? ORR "else" VVV
        else{ //One line ONLY9
            switch(sym){

                    case identsym: // var := ????
                        handleBecomeSym(vars);
                        break;

                    case readsym:
                        handleRead(vars);
                        break;

                    case writesym:
                        handleWrite(vars);
                        break;

                    case ifsym:
                        handleIf(vars);
                        break;

                    case whilesym:
                        handleWhile(vars);
                        break;
            }
            fscanf(fileLexTable,"%d", &sym);
            printf("%d else or ??? %d.\n", elsesym, sym);
            // ^^^ should now be on ??? ORR "else" ^^^
        }
        printf("lines: %d.\n", lines);
        //1) print jump to current line (beginning of else)
        if(!(toggleBacklog-1))
                fprintf(fileCode,"8 0 %d\n",lines+1);
            else{
                backlog[backlogPos][0] = 8;
                backlog[backlogPos][1] = 0;
                backlog[backlogPos++][2] = lines+1;
            }
        lines++;
        printf("%d lines, printed cj to %d.\n",lines-1+2);
        //2) scan the backlog of "else" & store first element of "else"
        singleLine = 1;
        //!
        //3) print the backlog of "if", fake it out!
        tempy = backlogPos;
        backlogPos = currentBacklog2; //fake it out, fake max
        printf("print from %d - %d.\n",currentBacklog, currentBacklog2);
        printBacklog(currentBacklog);
        backlogPos = tempy; //return the max.
        //4) print jump to current line (end of else)
        if(!(toggleBacklog-1))
                fprintf(fileCode,"7 0 %d\n",lines+4);
                //print conditional jump to (currentLines)+(backlogPos - currentBacklog)
            else{
                backlog[backlogPos][0] = 7;
                backlog[backlogPos][1] = 0;
                backlog[backlogPos++][2] = lines+4;
            }
        lines++;
        printf("printed j to %d.\n",lines-1);
        //5) print backlog of "else"
        printf("print from %d - %d.\n",currentBacklog2, backlogPos);
        printBacklog(currentBacklog2);
        backlogPos = currentBacklog; //return backlog to normal
        printf("%d. %d. current sym: %d.\n",writesym, semicolonsym, sym);
        singleLine = 1;
        toggleBacklog--;
    }
    else{
        printf("current lines %d. Jump to %d. (%d + (%d - %d))\n",lines,currentLines+(backlogPos-currentBacklog),currentLines,backlogPos,currentBacklog);
        if(!(toggleBacklog-1))
            fprintf(fileCode,"8 0 %d\n",4+currentLines+(backlogPos-currentBacklog));
            //print conditional jump to (currentLines)+(backlogPos - currentBacklog)
        else{
            backlog[backlogPos][0] = 8;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 4+currentLines+(backlogPos-currentBacklog);
        }
        lines++;
        singleLine = 1;
        printBacklog(currentBacklog);//print if's backlog
        toggleBacklog--;
    }
    if(singleLine){ //Test according to main ops
        read = 0;
        if(read)
            fscanf(fileLexTable,"%d", &sym);
        else
            read = 1;
        printf("main loop. sym: %d. if: %d.\n", sym, ifsym);
        switch(sym){

            case identsym: // var := ????
                handleBecomeSym(vars);
                break;

            case readsym:
                handleRead(vars);
                break;

            case writesym:
                handleWrite(vars);
                break;

            case ifsym:
                handleIf(vars);
                break;

            case whilesym:
                handleWhile(vars);
                break;
        }
    }
}

void printBacklog(int beginValue){
    int i, j;
    j = backlogPos;
    for(i = beginValue; i < j; i++){
        fprintf(fileCode,"%d %d %d\n", backlog[i][0],backlog[i][1],backlog[i][2]);
        backlogPos--;
    }
}

void handleWhile(varArray vars[]){}

void handleOdd(varArray vars[]){


}

void handleBecomeSym(varArray vars[]){
    int sym, returnPos, temp = -1;
    char varname[identMax];
    fscanf(fileLexTable,"%s", &varname);//find varname
    returnPos = findReturnPos(varname,vars); // get return address
    //Now handle whatever lies inside, until semicolon
    while(sym != semicolonsym){
        fscanf(fileLexTable,"%d", &sym);//find next sym
        switch(sym){

            case numbersym:
                handleNumber();
                if(temp != -1)
                    handleOperation(temp);
                break;

            case identsym:
                handleIdent(vars);
                if(temp != -1)
                    handleOperation(temp);
                break;

            case plussym:
            case minussym:
            case slashsym:
            case multsym:
                temp = sym;
                break;

            case lparentsym:
                handleParenthesis(vars);
                if(temp != -1)
                    handleOperation(temp);
                break;
        }
    }
    if(!toggleBacklog)
        fprintf(fileCode,"4 0 %d\n", returnPos);
    else{
        backlog[backlogPos][0] = 4;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = returnPos;
    }
    lines++;
}

void handleParenthesis(varArray vars[]){
    int sym = 0, temp = 0;
    while(sym != rparentsym){
        fscanf(fileLexTable,"%d", &sym);//find next sym
        switch(sym){
            case numbersym:
                handleNumber();
                if(temp != -1)
                    handleOperation(temp);
                break;

            case identsym:
                handleIdent(vars);
                if(temp != -1)
                    handleOperation(temp);
                break;

            case plussym:
            case minussym:
            case slashsym:
            case multsym:
                temp = sym;
                break;

            case lparentsym:
                handleParenthesis(vars);
                if(temp != -1)
                    handleOperation(temp);
                break;
        }
    }
}

void handleOperation(int sym){
    switch(sym){
        case plussym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 2\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 2;
        }
        lines++;
        break;

        case minussym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 3\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 3;
        }
        lines++;
        break;

        case multsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 4\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 4;
        }
        lines++;
        break;

        case slashsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 5\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 5;
        }
        lines++;
        break;

        case eqlsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 8\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 8;
        }
        lines++;
        break;

        //! XX % XX not handled ???

        case neqsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 9\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 9;
        }
        lines++;
        break;

        case lessym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 10\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 10;
        }
        lines++;
        break;

        case leqsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 11\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 11;
        }
        lines++;
        break;

        case gtrsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 12\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 12;
        }
        lines++;
        break;

        case geqsym:
        if(!toggleBacklog)
            fprintf(fileCode,"2 0 13\n");
        else{
            backlog[backlogPos][0] = 2;
            backlog[backlogPos][1] = 0;
            backlog[backlogPos++][2] = 13;
        }
        lines++;
        break;
    }
}

void handleIdent(varArray vars[]){
    int position, sym;
    char varname[identMax];
    fscanf(fileLexTable,"%s", varname); // varname
    position = findReturnPos(varname,vars); // get return address
    if(!toggleBacklog)
        fprintf(fileCode,"3 0 %d\n", position); //add variable to stack
    else{
        backlog[backlogPos][0] = 3;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = position;
    }
    lines++;
}

void handleNumber(){
    int sym;
    fscanf(fileLexTable,"%d", &sym); // "##"
    if(!toggleBacklog)
        fprintf(fileCode,"1 0 %d\n", sym);//add number to stack
    else{
        backlog[backlogPos][0] = 1;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = sym;
    }
    lines++;
}

void handleRead(varArray vars[]){
    int sym = 0, returnPos = 0;
    char varname[identMax];
    fscanf(fileLexTable,"%d", &sym); // "2"
    fscanf(fileLexTable,"%s", varname); //variable
    returnPos = findReturnPos(varname,vars); // get return address
    if(!toggleBacklog)
        fprintf(fileCode,"9 0 1\n"); //Read from screen
    else{
        backlog[backlogPos][0] = 9;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = 1;
    }
    lines++;
    if(!toggleBacklog)
        fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
    else{
        backlog[backlogPos][0] = 4;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = returnPos;
    }
    lines++;
}

void handleWrite(varArray vars[]){
    int sym = 0, returnPos = 0;
    char varname[identMax];
    fscanf(fileLexTable,"%d", &sym); // "2"
    fscanf(fileLexTable,"%s", varname); //variable
    returnPos = findReturnPos(varname,vars); // get return address
    if(!toggleBacklog)
        fprintf(fileCode,"3 0 %d\n", returnPos); //put into stack
    else{
        backlog[backlogPos][0] = 3;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = returnPos;
    }
    lines++;
    if(!toggleBacklog)
        fprintf(fileCode,"9 0 0\n"); //Write to screen
    else{
        backlog[backlogPos][0] = 9;
        backlog[backlogPos][1] = 0;
        backlog[backlogPos++][2] = 0;
    }
    lines++;
}

int findReturnPos(char varname[], varArray vars[]){
    int run = 1, returnPos = 0;
    while(strcmp(vars[returnPos].name, varname) != 0){ // Run until you find pos
        returnPos++;
    }
    returnPos+=4; //Plus 4(+1 from final pos++)
    return returnPos;
}
