# pl/0 Project, Assignment 4
pl/0 compiler, scanner, parser, etc.

Group Members: Travis Le and Allysia Freeman

In order to compile this program, please enter into the command line:
 	"gcc compiler.c -o compile"

In order to run after compiling, enter:
	"./compile (-l) (-a) (-v)"
Where () are optional items.

Arguements allowed:
-l: Print the list of lexemes/tokens to the screen
-a: Print the generated assembly code to the screen
-v: Print virtual machine exectution trace to the screen

Examples:
"./compile -l -a -v" prints all output types to console
"./compile -v" prints only the virtual machine trace to console
"./compile" prints nothing except prompts for read and write