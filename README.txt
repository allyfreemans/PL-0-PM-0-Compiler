# pl/0 Project, Assignment 4

pl/0 compiler, scanner, parser, etc.

In order to compile this program, please enter into the command line:

 	"gcc compiler.c -o compile"

In order to run after compiling, enter:

	"./compile (-l) (-a) (-v) (input.txt)
	
Where () are optional items.

If you do not enter an input filename, the program will assume the input is input.txt

Arguments allowed:

  -l: Print the list of lexemes/tokens to the screen
  -a: Print the generated assembly code to the screen
  -v: Print virtual machine exectution trace to the screen

Examples:

  "./compile -l -a -v" prints all output types to console
  "./compile -v" prints only the virtual machine trace to console
  "./compile" prints nothing except prompts for read and write

Existing issues: The nested calling of procedures tends to confuse the poor program sometimes, so don't nest a million procedures please.
