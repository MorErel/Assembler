# Assembler
An Assembler program to compile a file into a (made up) machine code.
if success creating a *.ob file with the memory lines (also extern and entry files if there are parameters).

The files:
main:
Getting file names parameters in the command line, compiling the files.

compiler:
Initializing parameter, checking if the file name is legal.
Allocating data and symbols memory.
Processing the first and second pass on input.
Freeing data and symbols memory.

firstPass:
Method processing the first pass on the input file,
Checks if the input is correct and creates symbol and data lists as a preperation for the next pass.

secondPass:
Contains all methods related to the second pass on the input.

parsing:
Contains all methods to check if the input is valid.

utilities:
Contains some methods used in parsing.




