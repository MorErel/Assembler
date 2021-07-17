#include "header.h"

/* Method processing the first pass on the input file
 * Checks if the input is correct and creates symbol and data lists for the next pass.
 * Parameters:
 * file_name - input file
 * dataList - list of data we get from the input
 * head - list of symbols we get from input
 * IC - instruction counter
 * DC - data counter
 * Returns TRUE if no errors were found, else returns FALSE */
int firstPass(char *file_name, dataPtr *dataList, symbolPtr *head, unsigned int *IC, unsigned int *DC) {
    int numOfWord = 0; /* counter for memory words */
    char temp[MAX_LABEL + 1]; /* temp var */
    char label[MAX_LABEL + 1]; /* to store the label */
    int lineNumber = 0; /* number of line we check */
    char commandLine[MAX_LINE + 1]; /* to store the command line */
    cmd command = NULL; /* to store the command */
    int label_flag = OFF; /* label flag on/off */
    char *token; /* to stores tokens from command line */
    char buff[LINE]; /* to store a command line */
    FILE *fd;
    int setter; /* to store setter */
    int i = 0; /* index var */
    int checkArray; /* stores result from a method that checks arrays */
    flag error = OFF; /* error flag */
    if (!(fd = fopen(file_name, "r"))) {
        fprintf(stderr, "error while opening file %s\n", file_name);
        exit(0);
    }
    while (!(feof(fd))) { /* while not end of file */
        numOfWord = 0; /* reset*/
        if (!fgets(buff, LINE, fd)) /* get line from file */
        {
            continue;
        }
        if (strlen(buff) > MAX_LINE - 1) {
            error = ON;
            fprintf(stderr, "Error in line %d: line is to long (up to 80 characters allowed)\n", lineNumber);
            continue;
        }
        label_flag = OFF; /*reset */
        lineNumber++; /* pointer to next line */
        strcpy(commandLine, buff); /* copy to line */
        token = strtok(buff, " \t\n"); /* gets first word */
        if (!token) /* if its an empty line skip to next line */
            continue;
        if (isComment(token)) { /* if its a comment line skip this line */
            continue;
        } else if (isLabel(&token)) { /* if token is label */

            switch (legalLabelName(token, *head)) { /* checking if token is legal */
                case 0: { /* illegal label name */
                    fprintf(stderr, "Error in line %d: illegal label name \"%s\"\n", lineNumber, token);
                    error = ON;
                    break;
                }/* end of case illegal label name */
                case -1: { /* name already defined */
                    fprintf(stderr, "Error in line %d: label name already defined \"%s\"\n", lineNumber, token);
                    error = ON;
                    break;
                }/* end of case label already defineed */
                case 1: /* legal label name */
                    if (strlen(token) > MAX_LABEL) {
                        fprintf(stderr, "Error in line %d: label name exceeded max length \"%s\"\n", lineNumber, token);
                        error = ON;
                        default:
                            break;
                    }
                    strcpy(label, token);
                    break;   /* end of legal name */
            } /* end of switch legal label name check */
            label_flag = ON;
            token = strtok(NULL, ""); /* getting parameters */
            if (!token) { /* if no parameters */
                fprintf(stderr, "Error in line %d: a label can not stand alone", lineNumber);
                error = ON;
                continue;
            }
            strcpy(commandLine, token);

        } /* end of label check */
        switch (checkCommas(commandLine)) {
            case -1: /*ilegal comma before command or label*/
                fprintf(stderr, "Error in line %d: illegal comma before command or label\n", lineNumber);
                error = ON;
                break;
            case -2: /*ilegal comma after command */
                fprintf(stderr, "Error in line %d: illegal comma after command or label\n", lineNumber);
                error = ON;
                break;
            case -3: /*duplicated comma */
                fprintf(stderr, "Error in line %d: duplicated comma between parameters\n", lineNumber);
                error = ON;
                break;
            case -4: /*ilegal comma before parameter */
                fprintf(stderr, "Error in line %d: illegal comma before parameter\n", lineNumber);
                error = ON;
                break;
            case -5: /*missing comma */
                fprintf(stderr, "Error in line %d: missing comma between parameters\n", lineNumber);
                error = ON;
                break;
            case -6: /* illegal comme in macro */
                fprintf(stderr, "Error in line %d: illegal comma in macro\n", lineNumber);
                error = ON;
                break;
            case -7: /* illegal comme in entry */
                fprintf(stderr, "Error in line %d: illegal comma in entry\n", lineNumber);
                error = ON;
                break;
            case -8: /* illegal comme in extern */
                fprintf(stderr, "Error in line %d: illegal comma in extern\n", lineNumber);
                error = ON;
                break;
            case -9: /* illegal comme after string */
                fprintf(stderr, "Error in line %d: illegal comma after string\n", lineNumber);
                error = ON;
                break;
            default:
                break;
        }
        token = strtok(commandLine, ", \t\n");
        if (isDefine(token)) { /* if token is macro */
            if (label_flag == ON) { /* printing error if line has a label */
                fprintf(stderr, "Error in line %d: Define cannot be under Label \"%s\"\n", lineNumber, token);
                error = ON;
                continue;
            }
            token = strtok(NULL, "\n"); /* getting the macro name */
            strcpy(buff, token);
            if (isLegalMacroDef(token)) {
                strcpy(buff, token);
                token = strtok(buff, " \t\n=");
            } else {
                fprintf(stderr, "Error in line %d: Wrong define declaration (Must be \"name = integer number\")\n",
                        lineNumber);
                error = ON;
                continue;
            }
            if (strlen(token) > MAX_LABEL) { /* checking for legal size for macro label */
                fprintf(stderr, "Error in line %d: Label name is too long \"%s\"\n", lineNumber, token);
                error = ON;
                continue;
            }
            switch (legalLabelName(token, *head)) { /* checking if define name is valid */
                case 0: { /* illegal label name */
                    fprintf(stderr, "Error in line %d: illegal label name \"%s\"\n", lineNumber, token);
                    error = ON;
                    break;
                }
                case -1: { /* if label already exist */
                    fprintf(stderr, "Error in line %d: label name already defined \"%s\"\n", lineNumber, token);
                    error = ON;
                    break;
                }
                case 1: /* if label is legal */
                    i = 0;
                    strcpy(label, token);
                    label_flag = ON;
                    token = strtok(NULL, "=\n");
                    if (!token) {
                        fprintf(stderr, "Error in line %d: Missing parameters", lineNumber);
                        error = ON;
                        break;
                    }
                    while (token[i] == '\t' || token[i] == ' ') { /* skips white chars */
                        i++;
                    }
                    if (isInt(token + i)) { /* if its an integer adding it to symbol list */
                        if (!addSymbol(head, label, FALSE, FALSE, TRUE, atoi(token + i))) {
                            fprintf(stderr, "Error in line %d: Not enough memory\n", lineNumber);
                            error = ON;
                            break;
                        }
                    } else { /* if not ineger report error */
                        fprintf(stderr, "Error in line %d: Parameter \"%s\" is not an integer\n", lineNumber, token);
                        error = ON;
                        break;
                    }
                default:
                    break;
            } /* end of switch define */
            continue;
        } /* end of if define */
        command = NULL;
        getCmd(token, &command); /* getting the command */
        if (!command) { /* if its not a legal command */
            strcpy(temp, token);
            strLwr(temp); /* check if the command is written with illegal upper cases */
            getCmd(temp, &command);
            if (command) { /* report the error */
                fprintf(stderr, "Error in line %d: command must be in Lower cases \"%s\"\n", lineNumber, token);
                command = NULL;
                error = ON;
		continue; /* move to next line */
            }
        }
        if (command) { /* if command is valid */
            flag rg_flag = OFF; /* register flag off */
            numOfWord++;
            if (label_flag == ON) { /* if there is label adding it to symbol list */
                addSymbol(head, label, FALSE, TRUE, FALSE, *IC);
            }
            for (i = 0; i < getCmdParams(command); i++) { /* getting parameters */

                token = strtok(NULL, "\t \n,");
                if (!token) {
                    fprintf(stderr, "Error in line %d: Missing parameters", lineNumber);
                    error = ON;
                    break;
                }
                if (isRegister(token)) { /* if parameter is register */
                    if (rg_flag == OFF) {
                        numOfWord++; /* updating the number of words */
                        rg_flag = ON;
                    } else {
                        if (isRegister(token)) {
                            rg_flag = OFF;
                            continue;
                        }
                    }

                } else if (token[0] == '#') { /* if parameter is a number */
                    if (!isInt(token + 1) &&
                        !isMacro(token + 1, *head)) { /* if not int and not macro def, report error */
                        fprintf(stderr, "Error in line %d: Parameter is not a real number \"%s\"\n", lineNumber,token);
                        error = ON;
                        continue;
                    }
                    numOfWord++;
                } else if ((checkArray = isArray(token, *head))) { /* if param is array */
                    if (checkArray == -1) { /* if index is negative */
                        fprintf(stderr, "Error in line %d: Array index is negative\n", lineNumber);
                        error = ON;
                        continue;
                    } else {
                        numOfWord += 2;
                    }
                } else if (legalLabelName(token, *head)) {

                    numOfWord += 1;
                } else {
                    fprintf(stderr, "Error in line %d: Illegal parameter  \"%s\"\n", lineNumber,token);
                    error = ON;
                    continue;
                }
            }
            (*IC) += numOfWord; /* updating the counter */
            token = strtok(NULL, "\n"); /* getting the rest of the line */
            if (token && token[0] != '\0') { /* if there are more parameters/test report error*/
                fprintf(stderr, "Error in line %d: Extra text after parameter\n", lineNumber);
                error = ON;
                continue;
            }
        } else if ((setter = isSetter(token)) != -1) { /* if token is a set instruction */
            if (setter == DATA) {
                token = strtok(NULL, "\t ,\n");
                if (!token) {
                    fprintf(stderr, "Error in line %d: Missing parameters", lineNumber);
                    error = ON;
                    break;
                }
                while (token != NULL) {
                    if (isMacro(token, *head)) {
                        getMacro(token, temp, *head);
                    } else if (isInt(token)) {
                        strcpy(temp, token);
                    } else {
                        fprintf(stderr, "Error in line %d: Parameter is not real number \"%s\"\n", lineNumber, token);
                        error = ON;
                        break;
                    }
                    if (label_flag == ON) {
                        addSymbol(head, label, FALSE, FALSE, FALSE, *DC); /* adding to symbol list */
                        label_flag = OFF;
                    }
                    if (!addData(dataList, (*DC)++, atoi(temp))) { /* adding to data list */
                        fprintf(stderr, "Error: not enough memory\n");
                        error = ON;
                        exit(0);
                    }
                    token = strtok(NULL, "\t ,\n");
                }
            } else if (setter == STRING) { /* if its a string instruction */
                {
                    if (label_flag == ON) {
                        addSymbol(head, label, FALSE, FALSE, FALSE, *DC); /* adding to symbol list */
                        label_flag = OFF;
                    }
                    token = strtok(NULL, "\n");
                    switch (isLegalString(token)) { /* checking if the string param is a legal string def */
                        case 0:
                            fprintf(stderr, "Error in line %d: illegal string \"%s\"\n", lineNumber, token);
                            error = ON;
                            break;
                        case -1:
                            fprintf(stderr, "Error in line %d: Extra text after string \"%s\"\n", lineNumber, token);
                            error = ON;
                            break;
                        case 1:
                            for (i = 1; token[i] != '\"'; i++) {
                                addData(dataList, (*DC)++, (int) token[i]);
                            }
                            addData(dataList, (*DC)++, 0);
                            break;
                        default:
                            break;
                    }
                } /* end of is string */
            } else if (setter == EXTERNAL) { /* if its an external instruction */
                token = strtok(NULL, " \t\n");
                if (!legalLabelName(token, *head)) {
                    fprintf(stderr, "Error in line %d: illegal label name \"%s\"\n", lineNumber, token);
                    error = ON;
                } else {
                    if (label_flag == ON) /* if label defined report a warning */
                        fprintf(stderr, "Warning in line %d: illegal label before instruction \"extern\"\n",lineNumber);
                    if (!isSymbol(token,*head)) /* if symbol is not in the symbol list */
                        addSymbol(head, token, TRUE, FALSE, FALSE, 0); /* add symbol */
                    else { /* if label name already in the symbol list */
                        fprintf(stderr, "Error in line %d: Label already defined \"%s\"\n", lineNumber, token);
                        error = ON;
                    }
                }
            } else if (setter == ENTRY) { /* if its a entry instruction */
                if (label_flag == ON) {
                    fprintf(stderr, "Warning in line %d: unused label before instruction \"entry\"\n", lineNumber);
                }
            } else if (setter == ENTRY) {
                continue;
            } else {
                strcpy(temp, token);
                strLwr(temp);
                if (isSetter(temp) == -1) {
                    fprintf(stderr, "Error in line %d: instructions must be with lower cases\"%s\"\n", lineNumber,
                            token);
                    setter = 0;
                    error = ON;
                }
            }
        } else {
            fprintf(stderr, "Error in line %d: Undefined command\\set name \"%s\"\n", lineNumber, token);
            error = ON;
        }
    } /* end of while loop */
    fclose(fd);
    if (error == OFF) /* returns according to the error status */
        return TRUE;
    else
        return FALSE;
}/* end of firstPass method */
