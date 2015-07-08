# pl/0 Project, Assignment 3
pl/0 compiler, scanner, parser, etc.
Work in  progress! 

Work Log:
 - compiler.c :: done
 
 - scanner.h  :: done
 
 - parser.h   :: wip
 
	progress: handles const
		  generates symbol table
		  tests for undeclared vars
		  handles := XXX;
		  handles read var;
		  handles write var;
			  
	needed:   handles if then else
			  
 - vm.h       :: done
 
 - header.h   :: n/a 
 
 current build works on EUSTIS?: ?
 current release works on EUSTIS?: Y
 
 compile: gcc compiler.c -o compile
 run: ./compile (-a) (-l) (-v)
