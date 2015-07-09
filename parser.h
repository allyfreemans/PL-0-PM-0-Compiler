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

void printSymTable();
void printMCode();
void createSym();
void generateMCode();
void numFound(int returnPos, varArray vars[], int returnF);
void varFound(int returnPos, varArray vars[], int returnF);
void operationFound(int sym, int returnPos, varArray vars[]);

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
    int returnPos, temPos, run;
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
    nStack = 3+j;
    //3a) add const values into stack!
    j = 0;
    for(i=0; i<MAX_SYMBOL_TABLE_SIZE; i++){
        if(thisTable[i].kind == 1){
            hashy = hashMe(thisTable[i].name);
            fprintf(fileCode,"1 0 %d\n",thisTable[hashy].val);
            fprintf(fileCode,"4 0 %d\n",3+j);
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
                    numFound(returnPos, vars, 0);
                }
                else if (sym == 2) // If it's a var
                    varFound(returnPos, vars, 0);
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
                fprintf(fileCode,"9 0 1\n",sym);
                fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
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
                fprintf(fileCode,"3 0 %d\n", returnPos); //put into stack
                fprintf(fileCode,"9 0 0\n",sym);
                break;

            case ifsym: //!!CANNOT DEAL WITH "odd" OR <> !!
                fscanf(fileLexTable,"%d", &sym); // "???"
                while(sym != lessym && sym != gtrsym && sym != eqlsym && sym != leqsym && sym != geqsym){// < OR > OR = OR <= OR >=
                    fscanf(fileLexTable,"%d", &sym);
                    if(sym == numbersym){ //If it's a number
                        numFound(returnPos, vars, 0);
                    }
                    else if (sym == 2) // If it's a var
                        varFound(returnPos, vars, 0);
                } //Should now be on the comparison sym

                switch(sym){//switch based on comparison needed
                    case lessym:
                        //
                        break;

                    case gtrsym:
                        //
                        break;

                    case eqlsym:
                        //
                        break;

                    case leqsym:
                        //
                        break;

                    case geqsym:
                        //
                        break;
                }

                fscanf(fileLexTable,"%d", &sym); // "???"
                while(sym != thensym){// "then"
                    fscanf(fileLexTable,"%d", &sym);
                    if(sym == numbersym){ //If it's a number
                        numFound(returnPos, vars, 0);
                    }
                    else if (sym == 2) // If it's a var
                        varFound(returnPos, vars, 0);
                } //Should now be on the next line
                break;
        }
    }
    //5) halt/end.
    fprintf(fileCode,"9 0 2\n");

    fclose(fileCode);
}

void numFound(int returnPos, varArray vars[], int returnF){
    int sym = 0;
    fscanf(fileLexTable,"%d", &sym);//"##"
    printf("numFound. into: %d\n", sym);
    fprintf(fileCode,"1 0 %d\n",sym); //push number to stack
    if(returnF != 1){//If you want to add number to sum
        printf("  numfound return.\n");
        fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
        fscanf(fileLexTable,"%d", &sym);//"##"
        printf("Testing %d.\n", sym);
        if(sym == identsym || sym == numbersym)
            printError(20);
        else if(sym == semicolonsym)
            return;
        else
            operationFound(sym, returnPos, vars);
    }
}

void varFound(int returnPos, varArray vars[], int returnF){
    int temPos = 0, run = 1, sym = 0;
    printf("varFound.\n");
    char varname[identMax];
    fscanf(fileLexTable,"%s", &varname);
    //printf("  looking for %s.\n", varname);
    while(run != 0){
        run = strcmp(varname,vars[temPos].name);
        temPos++;
    }
    temPos+=2;
    printf("  %s at pos %d.\n", varname, temPos);
    fprintf(fileCode,"3 0 %d\n", temPos);//Add to stack
    if(!returnF){
        printf("  varfound return.\n");
        fprintf(fileCode,"4 0 %d\n", returnPos); //Store in stack
        fscanf(fileLexTable,"%d", &sym);//"##"
        if(sym == identsym || sym == numbersym)
            printError(20);
        else if(sym == semicolonsym)
            return;
        else
            operationFound(sym, returnPos, vars);
    }

}

void operationFound(int sym, int returnPos, varArray vars[]){
    char varname[identMax];
    int runalready = 0;
    //printf("opFound. Into: %d w return %d.\n", sym, returnPos);
    while(sym != semicolonsym){
        switch(sym){
            case minussym:
                if(!runalready)
                    fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                fscanf(fileLexTable,"%d", &sym);//var OR num
                //printf("grabbed - [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1);
                }
                fprintf(fileCode,"2 0 3\n");
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;

            case plussym:
                if(!runalready)
                    fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                fscanf(fileLexTable,"%d", &sym);//var OR num
                //printf("grabbed - [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1);
                }
                fprintf(fileCode,"2 0 2\n");
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;

            case multsym:
                if(!runalready)
                    fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                fscanf(fileLexTable,"%d", &sym);//var OR num
                //printf("grabbed - [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1);
                }
                fprintf(fileCode,"2 0 4\n");
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;

            case slashsym:
                if(!runalready)
                    fprintf(fileCode,"3 0 %d\n", returnPos);//Add to stack
                fscanf(fileLexTable,"%d", &sym);//var OR num
                //printf("grabbed - [%d] ???.\n", sym);
                if(sym == 2){
                    varFound(returnPos,vars,1);
                }
                else if(sym == numbersym){
                    numFound(returnPos,vars,1);
                }
                fprintf(fileCode,"2 0 5\n");
                fscanf(fileLexTable,"%d", &sym);
                runalready = 1;
                break;
        }
    }
    fprintf(fileCode,"4 0 %d\n", returnPos);
    readAlready = 0;
}
