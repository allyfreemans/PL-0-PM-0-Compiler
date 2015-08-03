//DONE!
#include "header.h"

//OpCode String Holders
const char* OPCODE_STRINGS[] = {"FCH", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO"};
const char* STACK_OPERATION_STRINGS[] = {"RET", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD", "MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ"};
const char* SIO_OPERATION_STRINGS[] = {"", "SOT", "SIN"};

enum OPCODE {FCH, LIT, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO};
enum STACK_OPERATION {RET, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ};
enum SIO_OPERATION {SOT = 1, SIN};

//Register
int BP = 1;
int SP = 0;
int PC = 0;
instruction IR;

//Global Arrays
int stack[MAX_STACK_HEIGHT];
instruction code[MAX_CODE_LENGTH];

//Other
int add_one = 0, numCalls = 0, baseLex = 0;
int codeSize = 0;
int additons[999], addPos = 0;

//Functions
void loadFile();
void writeCode();
void runCode(int flag, int flag2);
void fetch_cycle();
void execute_cycle(int flag);
void operate();
int base(int level, int b);
void printStack();
void printToScreen(int flag);

void vm(int flag){
    stack[1] = 0;
    stack[2] = 0;
    stack[3] = 0;

    fileCode = fopen(nameMCode,"r");
    if(fileCode == NULL)
        printError(1);

    loadFile();

    fileTrace = fopen(nameTrace,"w");
    if(fileTrace == NULL)
        printError(1);

    writeCode(0);

    runCode(0, flag);

    fclose(fileTrace);
    fclose(fileCode);
}

void loadFile(){
    int OP, L, M, i = 0;
    while(fscanf(fileCode,"%d",&OP) != EOF){
        if(i>MAX_CODE_LENGTH)
            printError(22);

        fscanf(fileCode, "%d", &L);
        fscanf(fileCode, "%d", &M);

        code[i].OP = OP;
        code[i].L = L;
        code[i].M = M;

        i++;
    }
    codeSize = i;
}

void writeCode(int flag) {
	int i;
	fprintf(fileTrace,"Line OP   L   M\n");
	for(i = 0; i < codeSize; i++){
		fprintf(fileTrace," %3d %s %2d %3d\n", i, OPCODE_STRINGS[code[i].OP], code[i].L, code[i].M);
	}
	fprintf(fileTrace,"\n\n");
	fprintf(fileTrace,"                pc  bp  sp  stack\nInitial values  %2d   %d  %2d\n", PC, BP, SP);
}

void runCode(int flag, int flag2){
    int a,c,d;
    char b[3];
    while (BP > 0){
        if (PC < codeSize){
            if(flag)
                printf("%3d %s %2d %3d ", PC, OPCODE_STRINGS[code[PC].OP], code[PC].L, code[PC].M);
            fprintf(fileTrace,"%3d %s %2d %3d ", PC, OPCODE_STRINGS[code[PC].OP], code[PC].L, code[PC].M);

            fetch_cycle();
            execute_cycle(flag2);

            if(flag)
                printf("%3d %3d %3d  ", PC, BP, SP);
            fprintf(fileTrace,"%3d %3d %3d  ", PC, BP, SP);
            printStack(flag);
            if(flag)
                printf("\n");
            fprintf(fileTrace,"\n");
        }
        else{
            printToScreen(flag2);
            exit(99);

        }
    }
}

void fetch_cycle(){
	IR = code[PC];
	PC++;
}

void execute_cycle(int flag){
    int value = 0, i = 0;
	switch (IR.OP) {
		case FCH:	//Fetch, not really used here
			break;
		case LIT:	//push literal onto stack
			SP++;
			stack[SP] = IR.M;
			break;
		case OPR:	//Operate on stack
			operate();
			break;
		case LOD:	//push onto stack
			SP++;
			if(IR.L != 0)
                stack[SP] = stack[base(IR.L, BP) + IR.M];
            else
                stack[SP] = stack[base(IR.L, BP) + IR.M];
			break;
		case STO:	//pop the value off of the stack and store at offset IR.M
		    if(IR.L != 0)
                stack[base(IR.L, BP) + IR.M] = stack[SP];
            else
                stack[base(IR.L+0, BP) + IR.M] = stack[SP];
			SP--;
			break;
		case CAL:	//call proced. at IR.M
		    numCalls++;
		    stack[SP + 1] = 0; // return value (FV)
            stack[SP + 2] = base(IR.L, BP); // static link (SL)
            stack[SP + 3] = BP; // dynamic link (DL)
            stack[SP + 4] = PC; // return address (RA)
            BP = SP + 1;
            PC = IR.M;
			break;
		case INC:	//Increment stack pointer (sp)
			SP += IR.M;
			break;
		case JMP:	//Jump to the instruction at IR.M
			PC = IR.M;
			break;
		case JPC:	//Jump to the instruction at IR.M if == 0
			if(stack[SP] == 0){
				PC = IR.M;
                SP--;
			}
			else
                SP--;
			break;
		case SIO:	//perform standard IO op, depending on the instruction register
			if(IR.M == 0){
                printf("=====================\nWrite: %d\n=====================\n", stack[SP]);
                SP--;
			}
            else if(IR.M == 1){
                printf("=====================\nRead: ");
                scanf("%d", &value);
                printf("=====================\n\n");
                SP++;
                stack[SP] = value;
            }
            else if(IR.M == 2){
                fclose(fileTrace);
                printToScreen(flag);
                exit(0);
                exit(32);
            }
			break;
		case 10:
		    exit(0);
		    break;
		default:
			exit(-98);
    }
}

void operate(){
	switch (IR.M){
		case RET:
		    numCalls--;
			SP = BP - 1;
			PC = stack[SP + 4];
			BP = stack[SP + 3];
			break;
		case NEG:
			stack[SP] *= -1;
			break;
		case ADD:
			SP--;
			stack[SP] = stack[SP] + stack[SP + 1];
			break;
		case SUB:
			SP--;
			stack[SP] = stack[SP] - stack[SP + 1];
			break;
		case MUL:
			SP--;
			stack[SP] = stack[SP] * stack[SP + 1];
			break;
		case DIV:
			SP--;
			stack[SP] = stack[SP] / stack[SP + 1];
			break;
		case ODD:
			stack[SP] %= 2;
			break;
		case MOD:
			SP--;
			stack[SP] %= stack[SP + 1];
			break;
		case EQL:
			SP--;
			stack[SP] = stack[SP] == stack[SP+1];
			break;
		case NEQ:
			SP--;
			stack[SP] = stack[SP] != stack[SP + 1];
			break;
		case LSS:
			SP--;
			stack[SP] = stack[SP] < stack[SP + 1];
			break;
		case LEQ:
			SP--;
			stack[SP] = stack[SP] <= stack[SP + 1];
			break;
		case GTR:
			SP--;
			stack[SP] = stack[SP] > stack[SP + 1];
			break;
		case GEQ:
			SP--;
			stack[SP] = stack[SP] >= stack[SP + 1];
			break;
		default:
			printError(17);
	}
}

int base (int level, int base){
	while(level > 0){
		base = stack[base + 1];
		level--;
	}
	return base;
}

void printStack(int flag){
	int this_BP = BP;
	int num_BPs = 1;
	int BPs[MAX_LEXI_LEVELS];
	int i = 1;
	BPs[0] = 1;

	while (this_BP > 1){
        BPs[i++] = this_BP;//Set to pos i in BP array
        this_BP = stack[this_BP + 2];//Advance to previous BP number
	}
    num_BPs = i-1;
    if(num_BPs > MAX_LEXI_LEVELS){
        printf("\nError: :: ");
        printError(23);
    }

	for(i = 1; i <= SP; i++) {
		if (i == BPs[num_BPs] && i != 1) {
            if(flag)
                printf("| ");
			fprintf(fileTrace,"| ");
			num_BPs--;
		}
		if(flag)
            printf("%d ", stack[i]);
		fprintf(fileTrace,"%d ", stack[i]);
	}
}

void printToScreen(int flag){
    char scanned[99], c, hold;
    int run = 1;
    fileTrace = fopen(nameTrace, "r");
    if(flag)
    printf("\n=============================================\nStack Trace:\n=============================================\n");
    if(flag){
        run = strcmp("pc",scanned);
        while(run != 0){
            fscanf(fileTrace,"%s",scanned);
            run = strcmp("pc",scanned);
        }
        fscanf(fileTrace,"%s",scanned); //"bp"
        fscanf(fileTrace,"%s",scanned); // "sp"
        fscanf(fileTrace,"%s",scanned); // "stack"
        printf("                pc  bp  sp  stack");
        c = fgetc(fileTrace);
        while(c != EOF){
            if((int)c < 156)
            printf("%c", c);
            c = fgetc(fileTrace);
        }
        printf("\n");
    }
    fclose(fileTrace);
}
