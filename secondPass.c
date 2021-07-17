/* methods related to the second pass of the compilation */
#include "header.h"
/* struct representation of the bits of an opcode word */
typedef struct opWord {
    unsigned int are:2;
    unsigned int op_dest:2;
    unsigned int op_source:2;
    unsigned int opcode:4;
    unsigned int null:4;
} opWord;
/* struct representation of the bits of a data word related to opcode word */
typedef struct dataOpWord {
    unsigned int are:2;
    unsigned int data:12;
} dataOpWord;
/* struct representation of the bits of an array data word */
typedef struct arrWord {
    dataOpWord address;
    dataOpWord index;
} arrWord;
/* struct representation of the bits of a register data word */
typedef struct regData {
    unsigned int are:2;
    unsigned int dest:3;
    unsigned int source:3;
    unsigned int null:6;
} regDataWord;
/* Method to print the command's data words into a file
 * Parameters:
 * file_name - input file
 * command - the command
 * parameters  - the parameters
 * IC - instruction counter
 * DC - data counter
 * symbolList the symbol list
 * error - flag
 * line - line number
 * Returns TRUE if no errors were found, else returns FALSE */
int printCode(char *file_name, cmd command, char *parameters, unsigned int *IC, symbolPtr symbolList, int *error, int line) {
    enum types {NONE, RG, DATA, ARR}; /* types of data */
    char commandName[MAX_LABEL];
    flag dest = OFF; /* dest or source flag */
    int sourceParam = NONE, destParam = NONE; /* to store type of parameter */
    opWord opCode; /* a command data word */
    char *token; /* to break the command line into words */
    char temp[MAX_LABEL + 1] = ""; /* to store temp string */
    int numOfParams = getCmdParams(command); /* gets the command's number of parameters */
    dataOpWord opCodeData, opCodeData2; /* data words */
    arrWord arr1, arr2; /* array data words */
    char arrName[MAX_LABEL + 1]; /* to store the array name */
    regDataWord regWord; /* register data word */
    regWord.source = 0; /* initializing all values to 0 */
    regWord.null = 0;
    regWord.dest = 0;
    opCode.null = 0;
    opCode.op_source = 0;
    opCode.op_dest = 0;
    opCodeData.are = 0;
    opCodeData.data = 0;
    opCodeData2.are = 0;
    opCodeData2.data = 0;
    opCode.opcode = getCmdOpCode(command); /* gets the command opcode */
    opCode.are = ABSOLUTE; /* sets the conversion type */
    token = strtok(parameters, " \t\n,");
    getCmdName(command,commandName); /* gets the command's name and stores it */
    if (numOfParams == 1) { /* if only one parameter we sets it in the dest area */
        dest = ON;
    }
    while (numOfParams > 0) { /* while there are parameters */
        switch (paramType(token, symbolList)) { /* switch the parameter type */
            case INSTANT: /* instant variable type */
                if (dest == ON) {
                    if (!(checkLegalParams(command, INSTANT, 1))) { /* if parameter is not legal for  this command */
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    destParam = DATA;
                    if (isMacro(token + 1, symbolList)) { /* if its macro get the value */
                        opCodeData2.data = (unsigned int) getMacro(token + 1, temp, symbolList);
                    } else {
                        opCodeData2.data = (unsigned int) atoi(token + 1); /* if an int gets the value */
                    }
                    opCodeData2.are = ABSOLUTE; /* setting conversion type */
                    opCode.op_dest = 0;
                    dest = OFF;
                } else { /* same procces but for source */
                    if (!(checkLegalParams(command, INSTANT, 0))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    sourceParam = DATA;
                    opCode.op_source = 0;
                    if (isMacro(token + 1, symbolList)) {
                        opCodeData.data = (unsigned int) getMacro(token + 1, temp, symbolList);
                    } else {
                        opCodeData.data = (unsigned int) atoi(token + 1);
                    }
                    opCodeData.are = ABSOLUTE;
                    dest = ON;
                }
                token = strtok(NULL, "\n\t, ");
                break;
            case DIRECT:  /* direct conversion type */
                if (dest == ON) {
                    if (!(checkLegalParams(command, DIRECT, 1))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    destParam = DATA;
                    opCode.op_dest = DIRECT;
                    if (isSymbol(token, symbolList)) {
                        opCodeData2.data = (unsigned int) getSymVal(token, symbolList);
                        if (isExtern(token, symbolList)) {
                            opCodeData2.are = EXTERN;
                            switch (sourceParam) {
                                case INSTANT:
                                case DIRECT:
                                case REGISTER:
                                    writeExtern(file_name, token, *IC + 1);
                                    break;
                                case CONSTANT:
                                    writeExtern(file_name, token, *IC + 2);
                                    break;
                            }
                        } else
                            opCodeData2.are = RELOCATE;
                    } else {
                        fprintf(stderr, "Error in line %d: no such variable %s\n", line, token);
                        *error = ON;
                    }
                } else {
                    if (!(checkLegalParams(command, DIRECT, 0))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    sourceParam = DATA;
                    opCode.op_source = DIRECT;
                    if (isSymbol(token, symbolList)) {
                        opCodeData.data = (unsigned int) getSymVal(token, symbolList);
                        if (isExtern(token, symbolList)) {
                            opCodeData.are = EXTERN;
                            writeExtern(file_name, token, *IC + 1);
                        } else
                            opCodeData.are = RELOCATE;
                    } else {
                        fprintf(stderr, "error in line %d: no such variable %s\n", line, token);
                        *error = ON;
                    }
                    dest = ON;
                }

                token = strtok(NULL, "\n\t, ");
                break;
            case CONSTANT: /* constant conversion type */
                strcpy(arrName, token);
                token = strtok(NULL, " \t,\n");
                strcpy(parameters, arrName);
                if (dest == ON) {
                    if (!(checkLegalParams(command, CONSTANT, 1))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    destParam = ARR;
                    opCode.op_dest = CONSTANT;
                    arr2.address.data = (unsigned int) getArrAddress(arrName, symbolList);
                    arr2.address.are = RELOCATE; /* not handled external !!! */
                    strcpy(arrName, parameters);
                    arr2.index.data = (unsigned int) getArrIndex(arrName, symbolList);
                    arr2.index.are = ABSOLUTE;
                } else {
                    if (!(checkLegalParams(command, CONSTANT, 0))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    sourceParam = ARR;
                    arr1.address.data = (unsigned int) getArrAddress(arrName, symbolList);
                    arr1.address.are = RELOCATE; /* not handled external !!! */
                    strcpy(arrName, parameters);
                    arr1.index.data = (unsigned int) getArrIndex(arrName, symbolList);
                    arr1.index.are = ABSOLUTE;
                    opCode.op_source = CONSTANT;
                    dest = ON;
                }
                break;
            case REGISTER: /* register conversion type */
                regWord.are = ABSOLUTE;
                if (dest == ON) {
                    if (!(checkLegalParams(command, REGISTER, 1))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    destParam = RG;
                    opCode.op_dest = REGISTER;
                    regWord.dest = (unsigned int) getRegister(token);
                } else {
                    if (!(checkLegalParams(command, REGISTER, 0))) {
                        fprintf(stderr, "Error in line %d: Illegal parameter type for command \"%s\"\n", line,commandName);
                        *error = ON;
                        break;
                    }
                    sourceParam = RG;
                    opCode.op_source = REGISTER;
                    regWord.source = (unsigned int) getRegister(token);
                    dest = ON;
                }
                token = strtok(NULL, "\n\t, ");
                break;
            default:
                fprintf(stderr, "Error in line %d: no such label %s\n", line, token);
                *error = ON;
        }
        numOfParams--;
    }
    if (*error == ON)
        return FALSE;
    changeExtension(file_name, "ob.tmp"); /* changing file Extension */
    printToFile(file_name, (*IC)++, &opCode); /* printing opcode data to file */
    switch (sourceParam) { /* prints the source data word */
        case RG: /* if register */
            printToFile(file_name, (*IC)++, &regWord);
            break;
        case DATA: /* if symbol or number */
            printToFile(file_name, (*IC)++, &opCodeData);
            break;
        case ARR: /* if array cell or string */
            printToFile(file_name, (*IC)++, &(arr1.address));
            printToFile(file_name, (*IC)++, &(arr1.index));
            break;
        default: /* if none */
            break;
    }
    switch (destParam) { /* prints the dest data word */
        case RG: /* if register */
            if (sourceParam != RG) /* will add an extra word for register only if the first parameter is not register */
                printToFile(file_name, (*IC)++, &regWord);
            break;
        case DATA: /* if symbol or number */
            printToFile(file_name, (*IC)++, &opCodeData2);
            break;
        case ARR: /* if array cell or string */
            printToFile(file_name, (*IC)++, &(arr2.address)); /* first word added for the address */
            printToFile(file_name, (*IC)++, &(arr2.index)); /* second word added for index number in array */
            break;
        default: /* if none */
            changeExtension(file_name, "as"); /* changing file Extension to original */
            return FALSE;
    }
    changeExtension(file_name, "as"); /* changing file Extension to original */
    return TRUE; /* if print succeed return true */
} /* end of printCode method */

/* Method processing the second pass on the input file
 * Parameters:
 * file_name - input file name
 * dataList - list of data we get from the input
 * symbolList - list of symbols we get from input
 * IC - instruction counter
 * DC - data counter
 * error - errors flag
 * Returns TRUE if no errors were found, else returns FALSE */

int secondPass(char *file_name, dataPtr *dataList, symbolPtr *symbolList, unsigned int *IC, unsigned int const *DC, int *error) {
    int lineNumber = 0; /* counter for lines */
    char commandLine[MAX_LINE + 1]; /* to store the command line */
    cmd command = NULL; /* to store the command */
    char *token; /* to store tokens from command line */
    char buff[MAX_LINE + 1]; /* to store a command line */
    FILE *fd;
    int setter; /* to store setter */
    if (!(fd = fopen(file_name, "r"))) {
        fprintf(stderr, "error while opening file %s\n", file_name);
        exit(0);
    }
    createObFile(file_name,*IC,*DC); /* creates a temporary .ob output file */
    *IC = MEMORY_START; /* resets IC */
    while (!(feof(fd))) { /* while not end of file */
        fgets(buff, MAX_LINE + 1, fd); /* get line */
        lineNumber++; /* pointer to next line */
        strcpy(commandLine, buff);
        token = strtok(buff, " \t\n");
        if (!token) { /* if nothing in line */
            continue;/* moving to next line */
        }/* end of if */
        if (isLabel(&token)) { /* if token is label */
            token = strtok(NULL, ", \t\n");
        } /* end of label check */
        if (!token) {
            continue;

        }
        if (isDefine(token)) { /* if token is macro */
            continue;
        } /* end of if define */
        command = NULL;
        getCmd(token, &command); /* getting the command */
        if (command != NULL) { /* if command is valid */
            token = strtok(NULL, "");
            printCode(file_name, command, token, IC, *symbolList, error, lineNumber); /* print code line data */
            continue;
        } else if ((setter = isSetter(token)) != -1) {
            if (setter == DATA) { /* if data skip this line */
                continue;

            } else if (setter == STRING) { /* if string skip this line */
                continue;
            } else if (setter == ENTRY) { /* if entry */
                token = strtok(NULL, ", \t\n");
                if (isSymbol(token, *symbolList) && !isMacro(token, *symbolList)) { /* if its a symbol */
                    writeEntry(file_name, token, (unsigned int) getSymVal(token, *symbolList)); /* print to entry file */
                } else { /* if no such label */
                    *error = ON;
                    fprintf(stderr, "error in line %d: no such label \"%s\"\n", lineNumber, token);
                }
                token = strtok(NULL, "\n");
                if (token) {
                    *error = ON;
                    fprintf(stderr, "error in line %d: extra text after parameters \"%s\"\n", lineNumber, token);
                }
                continue;
            }
        }
    } /* end of while loop */
    if (*error == OFF) { /* if no errors changing the extensions of the files and adding the data */
        printf("Creating files\n");
        changeTempFilesExtension(file_name); /* removing tmp extension from files */
        printData(file_name,dataList); /* prints the data words list to file */
    }
    else
        deleteTempFiles(file_name); /* deleting temporary files we created on the way */
    return (*error == ON) ? TRUE : FALSE;
} /* end of secondPass method */

