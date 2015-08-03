#include "scanner.h"
#include "parser.h"
#include "vm.h"
#include "header.h"

int main(int argc, char *argv[]){
    int l=0,a=0,v=0,i=0,s=0;

    strcpy(inputFileName,"empty");

    if(argc != 0){
        for(i=1; i<argc; i++){
            if(strcmp(argv[i], "-l") == 0)
                l = 1;//print token list
            else if(strcmp(argv[i], "-a") == 0)
                a = 1;//print assembly code
            else if(strcmp(argv[i], "-v") == 0)
                v = 1; //print virtual machine exec. trace
            else if(strcmp(argv[i], "-s") == 0)
                s = 1; //print symbol table. "hidden"
            else
                strcpy(inputFileName,argv[i]); //optional input filename
        }
    }
    scanner(l);
    parser(a,s);
    vm(v);
    return 0;
}
