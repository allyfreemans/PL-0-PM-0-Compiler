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
		      handles ident := XXX;
		      handles XXX <= (etc.) XXX test
		      handles read var;
		      handles write var;
		      handles unlimited parenthesis (recursive)
		      handles odd XXX (for while do and if then else)
		      fixed reading ANY number 2 as identsym
			  
	needed:   handles if then else
			  handles while do
			  handles begin ... end (for while do and if then else)


 - vm.h       :: done
 
 - header.h   :: n/a 
 
 current build works on EUSTIS?: Y
 current release works on EUSTIS?: Y
 
 compile: gcc compiler.c -o compile
 run: ./compile (-a) (-l) (-v)
