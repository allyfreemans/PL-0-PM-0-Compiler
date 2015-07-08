#include "header.h"

//Files
FILE *fileCode;
FILE *fileLexTable;
FILE *readFile;

typedef struct variable{
    char name[identMax];
    int type;
} varArray;

symTable thisTable[MAX_SYMBOL_TABLE_SIZE];
int nStack = 3;
int readAlready = 0;
int cLines = 0;
int backlogpos = 0;
int backlog[100][3];


void printSymTable();
void printMCode();
void createSym();
void generateMCode();
void numFound(int returnPos, varArray vars[], int returnF, int backlogFlag);
void varFound(int returnPos, varArray vars[], int returnF, int backlogFlag);
void operationFound(int sym, int returnPos, varArray vars[], int backlogFlag);

void parser(int flag){

    createSym();
    printf("\nNo errors, program is syntactically correct!\n\n");

    //printSymTable();

    generateMCode();

    if(flag)
        printMCode();
    printf("\n\nlines:%d\n\n", cLines-1);

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
    int i, sym=0, mult, runs=0;//don't remove int i
    int position = 0, L = 0, runFlag, scanFlag = 0, commaFlag = 0, commaFlag2 = 0;
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
        else if(sym == 2){
            fscanf(fileLexTable,"%s",name);
            c = fgetc(fileLexTable);
            position = hashMe(name); //Test to see if declared earlier
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
    int returnPos, temPos, run, operation;
    int i, j = 0,numVar = 0, numConst = 0, hashy=0;
    varArray vars[100];
    int sym = 0, procFlag = 0;
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
            printf("\nlines %d\n",cLines);
        if(sym == 2){
            fscanf(fileLexTable,"%s", varname);
            //printf("%d lines: Found [%s]\n",lines, varname);
            fscanf(fileLexTable,"%d", &sym);
        }
        else
            fscanf(fileLexTable,"%d", &sym);
        if((sym == beginsym) && (procFlag > 0)){
            printf("\nlines %d\n",cLines); cLines++;
            //printf("%d lines: Found [%d]\n",lines, sym);
            fscanf(fileLexTable,"%d", &sym);
        }
    }
    //printf("%d lines (on line %d): Found [%d]\n",lines, lines+1,sym);
    //2) count number of variables, log their pos into array
        //count the number of vars inside main proc
        //count num of consts
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
    fprintf(fileCode,"6 0 %d\n",3+j);
    printf("\nlines %d\n",cLines); cLines++;
    nStack = 3+j;
    //3a) add const values into stack!
    j = 0;
    for(i=0; i<MAX_SYMBOL_TABLE_SIZE; i++){
        if(thisTable[i].kind == 1){
            hashy = hashMe(thisTable[i].name);
            fprintf(fileCode,"1 0 %d\n",thisTable[hashy].val);
            printf("\nlines %d\n",cLines); cLines++;
            fprintf(fileCode,"4 0 %d\n",3+j);
            printf("\nlines %d\n",cLines); cLines++;
            j++;
        }
    }
    //4) go through main lines as such
    while(sym != periodsym){
        if(!readAlready)
            fscanf(fileLexTable,"%d", &sym);
        if(readAlready)
            readAlready = 0;
        switch(sym){

            case endsym:
                break;

            case identsym: // var := ????
                //printf("into: '%d'.\n", sym);
                fscanf(fileLexTable,"%s", &varname); //variable
                //printf("a looking for '%s'.\n", varname);
                //Get position of var
                returnPos = 0;
                run = 1;
                while(run != 0){
                    run = strcmp(varname,vars[returnPos].name);
                    returnPos++;
                }
                returnPos+=2;
                printf("  found at pos %d.\n", returnPos);
                fscanf(fileLexTable,"%d", &sym); // :=

                fscanf(fileLexTable,"%d", &sym); // "???"

                if(sym == numbersym){ //If it's a number
                    numFound(returnPos, vars, 0, 0);
                }
                else if (sym == 2) // If it's a var
                    varFound(returnPos, vars, 0, 0);
                break;

            case readsym:
                printf("reading.\n");
                fscanf(fileLexTable,"%d", &sym); // "2"
                fscanf(fileLexTable,"%s", &varname); //variable
                //Get position of var
                returnPos = 0;
                run = 1;
                while(run != 0){
                    run = strcmp(varname,vars[returnPos].name);
                    returnPos++;
                }
                returnPos+=2;
                fprintf(fileCode,"9 0 1\n",sym);
                printf("\nlines %d\n",cLines); cLines++;
                fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
                printf("\nlines %d\n",cLines); cLines++;
                break;

            case writesym:
                printf("write.\n");
                fscanf(fileLexTable,"%d", &sym); // "2"
                fscanf(fileLexTable,"%s", &varname); //variable
                //Get position of var
                returnPos = 0;
                run = 1;
                while(run != 0){
                    run = strcmp(varname,vars[returnPos].name);
                    returnPos++;
                }
                returnPos+=2;
                fprintf(fileCode,"3 0 %d\n", returnPos); //put into stack
                printf("\nlines %d\n",cLines); cLines++;
                fprintf(fileCode,"9 0 0\n",sym);
                printf("\nlines %d\n",cLines); cLines++;
                break;

            case ifsym: //!!CANNOT DEAL WITH "odd" OR <> !! (cause i don't know what they do)
                fscanf(fileLexTable,"%d", &sym); // "???"
                while(sym != lessym && sym != gtrsym && sym != eqlsym && sym != leqsym && sym != geqsym){// < OR > OR = OR <= OR >=
                    printf("sym: %d\n", sym);
                    if(sym == numbersym){ //If it's a number
                        numFound(returnPos, vars, 1, 0);
                    }
                    else if (sym == 2){ // If it's a var
                        varFound(returnPos, vars, 1, 0);
                    }
                    fscanf(fileLexTable,"%d", &sym); // "???"
                } //Should now be on the comparison sym
                operation = sym;

                fscanf(fileLexTable,"%d", &sym); // "???"
                while(sym != thensym){// < OR > OR = OR <= OR >=
                    printf("sym: %d\n", sym);
                    if(sym == numbersym){ //If it's a number
                        numFound(returnPos, vars, 1, 0);
                    }
                    else if (sym == 2){ // If it's a var
                        varFound(returnPos, vars, 1, 0);
                    }
                    fscanf(fileLexTable,"%d", &sym); // "???"
                } //Should now be on then

                switch(operation){//switch based on comparison needed
                    case eqlsym:
                        fprintf(fileCode,"2 0 8\n");//test
                        printf("\nlines %d\n",cLines); cLines++;
                        break;

                    case neqsym:
                        fprintf(fileCode,"2 0 9\n");//test
                        printf("\nlines %d\n",cLines); cLines++;
                        break;

                    case lessym:
                        fprintf(fileCode,"2 0 10\n");//test
                        printf("\nlines %d\n",cLines); cLines++;
                        break;

                    case leqsym:
                        fprintf(fileCode,"2 0 11\n");//test
                        printf("\nlines %d\n",cLines); cLines++;
                        break;

                    case gtrsym:
                        fprintf(fileCode,"2 0 12\n");//test
                        printf("\nlines %d\n",cLines); cLines++;
                        break;

                    case geqsym:
                        fprintf(fileCode,"2 0 13\n");//test
                        printf("\nlines %d\n",cLines); cLines++;
                        break;
                }
                printf("\nlines %d\n",cLines); cLines++;
                //READ NORMALLY
                fscanf(fileLexTable,"%d", &sym); // "??"
                printf(" at sym %d.\n", sym);
                backlogpos = 0;
                switch(sym){
                    case identsym: // var := ????
                        //printf("into: '%d'.\n", sym);
                        fscanf(fileLexTable,"%s", &varname); //variable
                        //printf("a looking for '%s'.\n", varname);
                        //Get position of var
                        returnPos = 0;
                        run = 1;
                        while(run != 0){
                            run = strcmp(varname,vars[returnPos].name);
                            returnPos++;
                        }
                        returnPos+=2;
                        printf("  found at pos %d.\n", returnPos);
                        fscanf(fileLexTable,"%d", &sym); // :=

                        fscanf(fileLexTable,"%d", &sym); // "???"

                        if(sym == numbersym){ //If it's a number
                            numFound(returnPos, vars, 0, 1);
                        }
                        else if (sym == 2) // If it's a var
                            varFound(returnPos, vars, 0, 1);
                        break;

                    case readsym:
                        fscanf(fileLexTable,"%d", &sym); // "2"
                        fscanf(fileLexTable,"%s", &varname); //variable
                        //Get position of var
                        returnPos = 0;
                        run = 1;
                        while(run != 0){
                            run = strcmp(varname,vars[returnPos].name);
                            returnPos++;
                        }
                        returnPos+=2;
                        backlog[backlogpos][0] = 9;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = 1;
                        backlogpos++;
                        printf("\nlines %d\n",cLines); cLines++;
                        backlog[backlogpos][0] = 4;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = returnPos;
                        backlogpos++;
                        printf("\nlines %d\n",cLines); cLines++;
                        break;

                    case writesym:
                        fscanf(fileLexTable,"%d", &sym); // "2"
                        fscanf(fileLexTable,"%s", &varname); //variable
                        //Get position of var
                        returnPos = 0;
                        run = 1;
                        while(run != 0){
                            run = strcmp(varname,vars[returnPos].name);
                            returnPos++;
                        }
                        returnPos+=2;
                        backlog[backlogpos][0] = 3;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = returnPos;
                        backlogpos++;
                        printf("\nlines %d\n",cLines); cLines++;
                        backlog[backlogpos][0] = 9;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = 0;
                        backlogpos++;
                        printf("\nlines %d\n",cLines); cLines++;
                        break;
                }
                fscanf(fileLexTable,"%d", &sym); // "?"
                printf("at sym %d.\n", sym);
                if(sym != elsesym)
                fprintf(fileCode,"8 0 %d\n", cLines-1);// jump to end of else
                else{
                    fprintf(fileCode,"8 0 %d\n", cLines);// jump to end of else
                }
                //print backlog
                for(i=0; i<backlogpos-1; i++){
                    fprintf(fileCode,"%d %d %d\n", backlog[i][0],backlog[i][1],backlog[i][2]);
                }
                backlogpos = 0;
                if(sym == elsesym){
                    if(!readAlready)
                        fscanf(fileLexTable,"%d", &sym); // "?"
                    else
                        readAlready = 0;
                    backlogpos = 0;
                    switch(sym){
                        case identsym: // var := ????
                            //printf("into: '%d'.\n", sym);
                            fscanf(fileLexTable,"%s", &varname); //variable
                            //printf("a looking for '%s'.\n", varname);
                            //Get position of var
                            returnPos = 0;
                            run = 1;
                            while(run != 0){
                                run = strcmp(varname,vars[returnPos].name);
                                returnPos++;
                            }
                            returnPos+=2;
                            printf("  found at pos %d.\n", returnPos);
                            fscanf(fileLexTable,"%d", &sym); // :=

                            fscanf(fileLexTable,"%d", &sym); // "???"

                            if(sym == numbersym){ //If it's a number
                                numFound(returnPos, vars, 0, 1);
                            }
                            else if (sym == 2) // If it's a var
                                varFound(returnPos, vars, 0, 1);
                            break;

                        case readsym:
                            fscanf(fileLexTable,"%d", &sym); // "2"
                            fscanf(fileLexTable,"%s", &varname); //variable
                            //Get position of var
                            returnPos = 0;
                            run = 1;
                            while(run != 0){
                                run = strcmp(varname,vars[returnPos].name);
                                returnPos++;
                            }
                            returnPos+=2;
                            backlog[backlogpos][0] = 9;
                            backlog[backlogpos][1] = 0;
                            backlog[backlogpos][2] = 1;
                            backlogpos++;
                            printf("\nlines %d\n",cLines); cLines++;
                            backlog[backlogpos][0] = 4;
                            backlog[backlogpos][1] = 0;
                            backlog[backlogpos][2] = returnPos;
                            backlogpos++;
                            printf("\nlines %d\n",cLines); cLines++;
                            break;

                        case writesym:
                            fscanf(fileLexTable,"%d", &sym); // "2"
                            fscanf(fileLexTable,"%s", &varname); //variable
                            //Get position of var
                            returnPos = 0;
                            run = 1;
                            while(run != 0){
                                run = strcmp(varname,vars[returnPos].name);
                                returnPos++;
                            }
                            returnPos+=2;
                            backlog[backlogpos][0] = 3;
                            backlog[backlogpos][1] = 0;
                            backlog[backlogpos][2] = returnPos;
                            backlogpos++;
                            printf("\nlines %d\n",cLines); cLines++;
                            backlog[backlogpos][0] = 9;
                            backlog[backlogpos][1] = 0;
                            backlog[backlogpos][2] = 0;
                            backlogpos++;
                            printf("\nlines %d\n",cLines); cLines++;
                            break;
                    }
                    fprintf(fileCode,"7 0 %d\n", cLines-1);// jump to else
                    //print backlog
                    printf("backlog:\n");
                    for(i=0; i<backlogpos; i++){
                        printf("%d %d %d\n",backlog[i][0],backlog[i][1],backlog[i][2]);
                        fprintf(fileCode,"%d %d %d\n", backlog[i][0],backlog[i][1],backlog[i][2]);
                    }
                    backlogpos = 0;
                }
                //fscanf(fileLexTable,"%d", &sym); // "???"
                break;
        }
    }
    //5) halt/end.
    fprintf(fileCode,"9 0 2\n");
    printf("\nlines %d\n",cLines); cLines++;
    fclose(fileCode);
}

void numFound(int returnPos, varArray vars[], int returnF, int backlogFlag){
    int sym = 0;
    fscanf(fileLexTable,"%d", &sym);//"##"
    printf("numFound. into: %d\n", sym);
    if(!backlogFlag)
        fprintf(fileCode,"1 0 %d\n",sym); //push number to stack
    else{
        backlog[backlogpos][0] = 1;
        backlog[backlogpos][1] = 0;
        backlog[backlogpos][2] = sym;
        backlogpos++;
    }
    printf("\nlines %d\n",cLines); cLines++;
    if(returnF != 1){//If you want to add number to sum
        printf("  numfound return.\n");
        if(!backlogFlag)
            fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
        else{
            backlog[backlogpos][0] = 4;
            backlog[backlogpos][1] = 0;
            backlog[backlogpos][2] = returnPos;
            backlogpos++;
        }
        printf("\nlines %d\n",cLines); cLines++;
        fscanf(fileLexTable,"%d", &sym);//"##"
        printf("Testing %d.\n", sym);
        if(sym == identsym || sym == numbersym)
            printError(20);
        else if(sym == semicolonsym)
            return;
        else
            operationFound(sym, returnPos, vars, backlogFlag);
    }
}

void varFound(int returnPos, varArray vars[], int returnF, int backlogFlag){
    int temPos = 0, run = 1, sym = 0;
    printf("varFound.\n");
    char varname[identMax];
    fscanf(fileLexTable,"%s", &varname);
    printf("  looking for %s.\n", varname);
    while(run != 0){
        printf("   pos %d.\n", temPos);
        run = strcmp(varname,vars[temPos].name);
        temPos++;
    }
    temPos+=2;
    printf("  %s at pos %d.\n", varname, temPos);
    if(!backlogFlag)
        fprintf(fileCode,"3 0 %d\n", temPos);//Add to stack
    else{
        backlog[backlogpos][0] = 3;
        backlog[backlogpos][1] = 0;
        backlog[backlogpos][2] = temPos;
        backlogpos++;
    }
    printf("\nlines %d\n",cLines); cLines++;
    if(!returnF){
        printf("  varfound return.\n");
        if(!backlogFlag)
            fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
        else{
            backlog[backlogpos][0] = 4;
            backlog[backlogpos][1] = 0;
            backlog[backlogpos][2] = returnPos;
            backlogpos++;
        }
        printf("\nlines %d\n",cLines); cLines++;
        fscanf(fileLexTable,"%d", &sym);//"##"
        printf("  sym: %d\n", sym);
        if(sym == identsym || sym == numbersym)
            printError(20);
        else if(sym == semicolonsym)
            return;
        else if(sym == minussym || sym == plussym || sym == multsym || sym ==  slashsym)
            operationFound(sym, returnPos, vars, backlogFlag);
        else
            return;
    }

}

void operationFound(int sym, int returnPos, varArray vars[], int backlogFlag){
    char varname[identMax];
    int runalready = 0;
    printf("opFound. Into: %d w return %d.\n", sym, returnPos);
    while(sym != semicolonsym){
        switch(sym){
            case plussym:
                if(!runalready){
                    if(!backlogFlag)
                        fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                    else{
                        backlog[backlogpos][0] = 3;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = returnPos;
                        backlogpos++;
                    }
                    printf("\nlines %d\n",cLines); cLines++;
                }
                fscanf(fileLexTable,"%d", &sym);//var OR num
                printf("grabbed + [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1, backlogFlag);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1, backlogFlag);
                }
                if(!backlogFlag)
                    fprintf(fileCode,"2 0 3\n");
                else{
                    backlog[backlogpos][0] = 2;
                    backlog[backlogpos][1] = 0;
                    backlog[backlogpos][2] = 2;
                    backlogpos++;
                }
                printf("\nlines %d\n",cLines); cLines++;
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;


            case minussym:
                if(!runalready){
                    if(!backlogFlag)
                        fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                    else{
                        backlog[backlogpos][0] = 3;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = returnPos;
                        backlogpos++;
                    }
                    printf("\nlines %d\n",cLines); cLines++;
                }
                fscanf(fileLexTable,"%d", &sym);//var OR num
                printf("grabbed - [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1, backlogFlag);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1, backlogFlag);
                }
                if(!backlogFlag)
                    fprintf(fileCode,"2 0 3\n");
                else{
                    backlog[backlogpos][0] = 2;
                    backlog[backlogpos][1] = 0;
                    backlog[backlogpos][2] = 3;
                    backlogpos++;
                }
                printf("\nlines %d\n",cLines); cLines++;
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;

            case multsym:
                if(!runalready){
                    if(!backlogFlag)
                        fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                    else{
                        backlog[backlogpos][0] = 3;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = returnPos;
                        backlogpos++;
                    }
                    printf("\nlines %d\n",cLines); cLines++;
                }
                fscanf(fileLexTable,"%d", &sym);//var OR num
                printf("grabbed * [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1, backlogFlag);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1, backlogFlag);
                }
                if(!backlogFlag)
                    fprintf(fileCode,"2 0 3\n");
                else{
                    backlog[backlogpos][0] = 2;
                    backlog[backlogpos][1] = 0;
                    backlog[backlogpos][2] = 4;
                    backlogpos++;
                }
                printf("\nlines %d\n",cLines); cLines++;
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;

            case slashsym:
                if(!runalready){
                    if(!backlogFlag)
                        fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                    else{
                        backlog[backlogpos][0] = 3;
                        backlog[backlogpos][1] = 0;
                        backlog[backlogpos][2] = returnPos;
                        backlogpos++;
                    }
                    printf("\nlines %d\n",cLines); cLines++;
                }
                fscanf(fileLexTable,"%d", &sym);//var OR num
                printf("grabbed / [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1, backlogFlag);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1, backlogFlag);
                }
                if(!backlogFlag)
                    fprintf(fileCode,"2 0 3\n");
                else{
                    backlog[backlogpos][0] = 2;
                    backlog[backlogpos][1] = 0;
                    backlog[backlogpos][2] = 5;
                    backlogpos++;
                }
                printf("\nlines %d\n",cLines); cLines++;
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;
        }
    }
    if(!backlogFlag)
        fprintf(fileCode,"4 0 %d\n", returnPos);
    else{
        backlog[backlogpos][0] = 4;
        backlog[backlogpos][1] = 0;
        backlog[backlogpos][2] = returnPos;
        backlogpos++;
    }
    printf("\nlines %d\n",cLines); cLines++;
    readAlready = 0;
}
