#include "header.h"
void compile(char *);

/* Maman 14 - Assembler program to compile a file into a machine code
 * if success creating a *.ob file with the memory lines (also extern and entry files if there are parameters)
 * author: Eldar Erel, ID: 301001046 & Mor Dayan, ID: 308376037
 * version v1.0
 */
/* main method
 * getting file names parameters in the command line */
int main(int argc, char **argv) {
if (argc==1) /* if no file were sent report error*/
{
    fprintf(stderr,"Error: Missing files to process\n");
    exit(0);
}
while (argc>1) { /* compiling the files */
    compile(argv[--argc]);
}
    return 0;
} /* end of main method */