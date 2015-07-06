#include "header.h"

//Files
FILE *fileCode;
FILE *fileLexTable;

symTable thisTable[MAX_SYMBOL_TABLE_SIZE];

void printSymTable();
void createSym();
void generateMCode();

void parser(int flag){

    createSym();
    printf("\nNo errors, program is syntactically correct!\n");

    printSymTable();

    if(flag)
        printf("\nThis is where I would print my vm trace.\nIF I HAD ONE (/ 'A')/ /// _|__|_\n");

    generateMCode();

}

void printSymTable(){
    int i;
    printf("\nSymbol Table:\n");
    for(i=1; i<MAX_SYMBOL_TABLE_SIZE; i++){
        printf("%d %s %d %d %d \n", thisTable[i].kind, thisTable[i].name, thisTable[i].val, thisTable[i].level, thisTable[i].addr);
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
    }
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
    fileCode = fopen(nameMCode,"w");
    if(fileCode == NULL)
        printError(ERROR_INVALID_FILE);
    fileLexTable = fopen(nameLexTableList,"r");
    //Starting from line 0
    //jump to var/const declarations, top to bottom
        //if const
            //6 0 6 increment M
    //9 0 2 on bottom






    fclose(fileCode);
}
