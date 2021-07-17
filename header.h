#include <stdio.h> /* for input and output */
#include <stdlib.h> /* for malloc and free */
#include <string.h> /* for strtok, strcpy, memcpy, strlen and strcat */
#include <ctype.h>  /* for isdigit and tolower */
#define SIZE_OF_DATA_WORD 14 /* the size in bits of data word */
#define LINE 200 /* max characters for fgets */
#define NUM_OF_SETTERS 4 /* number of setters instructions */
#define NUM_OF_PARAM_TYPES 4 /* number of parameters type */
#define NUM_OF_COMMANDS 16 /* number of command */
#define NUM_OF_REGISTERS 9 /* number of registers */
#define MEMORY_START 100 /* memory start address */
#define MAX_LABEL 31 /* max characters for label */
#define MAX_LINE 82 /* max is 80 (not including \n and \0) */

typedef struct symbol * symbolPtr; /* pointer to symbol list */
typedef struct opCmd * cmd; /* pointer to command */
typedef struct dataWord * dataPtr; /* pointer to data list */

typedef enum { /* flags */
    FALSE, TRUE, ON, OFF
}flag;
typedef enum { /* ARE types */
    ABSOLUTE, EXTERN, RELOCATE
}ARE;
typedef enum { /* conversion types */
    INSTANT, DIRECT, CONSTANT, REGISTER
}addressing;

enum { /* setters instructions types */
    ENTRY, EXTERNAL, STRING, DATA
}setTypes;

void printToFile(char *, unsigned int, void *); /* prints all the data to a file */
void createObFile(char *, unsigned int , unsigned int); /* creates the ob file*/
int paramType(char *, symbolPtr); /* returns what type of parameter we got */
void printData(char *, dataPtr *); /* prints the data to the ob file */
void freeSymbolList(symbolPtr *); /* free symbol list*/
void writeExtern(char *, char *, unsigned int); /* writes ro the extern file */
int isExtern(char *, symbolPtr); /* Checks if label is extern */
int getArrIndex(char *, symbolPtr); /* gets the index number when an array parameter given */
int getArrAddress(char *, symbolPtr); /* gets an address of an array */
void itos(int, char *); /* int to string */
void changeExtension(char *, char *); /* change file name extension */
int isDefine(char *); /* checking if a string is a define */
int checkLegalParams(cmd, int, int); /* check if command parameter is legal */
void writeEntry(char *, char *, unsigned int); /* write to entry file */
int isInt(char const *); /* checking is string is a n int */
int isLabel(char **); /* checking if string is label and cuts the : */
int addSymbol(symbolPtr *, char *, int, int, int, int); /* add to symbol list */
int isSymbol(char *, symbolPtr); /* checking if string is in the symbol list */
int isMacro(char *, symbolPtr); /* checking if string is macro */
void getCmd(char *, cmd *); /* checking if string is command */
int addData(dataPtr *, unsigned int, int); /* adding data word to data.op */
int getMacro(char *, char *, symbolPtr); /* getting the macro value */
int isSetter(char *); /*  checking if string is in the setters list */
int isArray(char const *, symbolPtr); /* checks if parameter is Array */
int getSymVal(char *, symbolPtr); /* gets the value from a member in the symbol list */
int checkFileName (char const *); /* checks if the file is in .as format */
void freeLists(symbolPtr *,dataPtr *); /* frees lists */
void updateDataAdd(dataPtr *, unsigned int,symbolPtr *); /* after the first pass - updated data addresses */
int firstPass(char *, dataPtr *, symbolPtr *,unsigned int* ,unsigned int* ); /* first pass on the input */
int secondPass(char *, dataPtr *, symbolPtr *,unsigned int* ,unsigned int const*,int *); /* second pass on the input */
int isComment (char const * ); /* checks if we currently in a comment line starts with ; */
int checkCommas(char *); /* checks commas placing in a line */
int isLegalMacroDef(char const *); /* checks if a macro definition is legal */
void strLwr(char *); /* changes string to lower case */
int legalLabelName(char *, symbolPtr); /* checking if a string is not in the symbol list and has legal parameters */
int isLegalString(char const *); /* check if the string parameter is legal */
int getCmdParams (cmd); /* get the command params */
int isOpCode(char *); /* check is string is a command */
int getCmdSource (cmd ,int ); /* gets the command source param */
int getCmdDest (cmd ,int); /* gets the command dest param */
unsigned int getCmdOpCode (cmd); /* gets the command op code value */
unsigned int getDataAddress(dataPtr); /* gets data word address */
unsigned int getDataInfo(dataPtr); /* gets data word value */
dataPtr getNextData(dataPtr); /* gets next data word in memory*/
int getRegister(char * ); /* get register number */
int isRegister(char * ); /* check if string is a register */
void deleteTempFiles(char *); /* deleting all *.tmp files */
void changeTempFilesExtension (char *); /* removing *.tmp from all files extension */
void getCmdName (cmd,char *); /* gets command's name */
