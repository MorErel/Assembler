#include "header.h"
/*  method to check legal commas in a command line
 * parameters -
 * params - the command line
 * returns 1 if line is legal, negative value otherwise */
int checkCommas(char * params )/*method checks that the commas in the input are correct*/
{
    enum {start,cmd,aftercmd,parameters,betweenparameters,instring,afterstring};/*current location through input*/
    enum {waitingforcomma,commaoff};/*if we are waiting for a comma or not*/
    int FLAG; /* represents if we are waiting for comma or not */
    int i; /* index for input */
    char tempcom[8]; /* in case of instruction it will save the word (its max size if 7) */
    int CURRENT;/* beginning of the input */
    FLAG=commaoff; /* currently no need for comma */
    i=0;
    CURRENT=start; /* start of input */

    while(params[i]!='\0' && ((params[i]==' ') || (params[i]=='\t') ||(params[i]==10) ))/*skips beginning white notes */
    {
        i++;
    }

    while(params[i]!='\0')/*goes through the input char by char*/
    {
        if(params[i]=='.') /* in case of macro/extern/entry definition (should have no commas) */
        {
            memcpy(tempcom,&params[i],7);
            tempcom[7]='\0';

            i++;
            if((strcmp(tempcom,".define")==0)) /* if the input is define instruction */
            {
                while (params[i] != '\0' && params[i] != ',')
                {
                    i++;
                }
                if (params[i] == ',') /* illegal comma in macro definition */
                {
                    return -6;
                }
                else return 1; /* reached end of string with no commas to comma check is good */
            }
            else if(!(strcmp(tempcom,".extern"))) /* if its an extern instruction */
            {
                while (params[i] != '\0' && params[i] != ',')
                {
                    i++;
                }
                if (params[i] == ',') /* illegal comma in extern definition */
                {
                    return -8;
                }
                else return 1; /* reached end of string with no commas to comma check is good */
            }
            tempcom[6]='\0';
            if(!strcmp(tempcom,".entry")) /* if its a entry instruction */
            {
                while (params[i] != '\0' && params[i] != ',')
                {
                    i++;
                }
                if (params[i] == ',') /* illegal comma in entry definition */
                {
                    return -7;
                }
                else return 1; /* reached end of string with no commas to comma check is good */
            }
        }
        if(params[i]=='\"') /*ignores whats inside of a string*/
        {
            i++;
            CURRENT=instring; /* changes current mode */
            while(params[i]!='\0' && params[i] != '\"')
                i++; /* skips all chars in string */
            if(params[i]=='\"') { /* reached end of string */
                CURRENT = afterstring;
                FLAG = commaoff; /* not waiting for comma */
                i++;
            }
        }
        if(params[i] == ' ' || params[i] == '\t' || params[i]==10 ) /* next word in input */
        {
            if(CURRENT==cmd)/*if location is in command*/
                CURRENT = aftercmd;/*new location after command before parameters*/

            if(CURRENT==parameters)
            {
                CURRENT=betweenparameters;/*between parameters we can find spaces or tabs or commas*/
            }

            i++;
            continue;
        }

        if(params[i]==',') /* reached comma */
        {
            if(FLAG==waitingforcomma)
            {
                if(params[i+1]=='\0'||params[i+1]=='\n')
                {
                    return 0;/*if the next char is the end of the input - illegal comma after parameters*/
                }
                CURRENT=betweenparameters;
                FLAG = commaoff;
                i++;
                continue;

            } else {
                if (CURRENT == start) {
                    return -1; /*illegal comma before command*/
                }
                if (CURRENT == cmd) {
                    return -2; /*illegal comma after command*/
                }
                if(CURRENT==betweenparameters)
                {
                    return -3; /*duplicated comma*/
                }

                if (CURRENT == aftercmd) {
                    return -4; /*illegal comma before parameter*/
                }
                if(CURRENT == afterstring)
                {
                    return -9; /*illegal comma after string*/
                }

            }
        }

        else
        {
            if(CURRENT==betweenparameters && FLAG==waitingforcomma)
            {
                return -5; /*missing comma between parameters*/
            }
            if(CURRENT==betweenparameters || CURRENT==aftercmd)
            {
                CURRENT =parameters;
                FLAG= waitingforcomma;
            }
            if(CURRENT==start)
            {
                CURRENT = cmd;
            }

        }
        i++;

    }
    return 1;
}/* end of method checkCommas */

/*  method to change string to lower cases
 * parameters -
 * input - the string */
void strLwr(char *input) {
    int i; /* index */
    for (i = 0; i < strlen(input); i++) { /* going over the string */
        input[i] = (char)tolower(input[i]); /* changing the char to lower case */
    }
} /* end of strLwr method */
/*  checks if a a given label is legal
 * parameters -
 * name - the label
 * head - symbol list head
 * returns 1 if legal, -1 if label already defined or 0 if label is not legal */

int legalLabelName (char * name, symbolPtr head)
{
    int i; /* index for name */
    if (!islower(name[0]) && !isupper(name[0])) /* if label dont start with letter return false */
    {
        return FALSE;
    }
    for(i=1 ; name[i] !='\0' ; i++) {
        if (!islower(name[i]) && !isupper(name[i]) && !isdigit(name[i])) { /* if char is not a number or letter return false */
            return FALSE;
        }
    }
    if (isOpCode(name) || isRegister(name) ||isSymbol(name,head)) /* if label name is already defined */
        return -1;
    return TRUE; /* if no errors return true */
}/* end of method legalLabelName */
/*  checks if the string starts and ends with "
 * parameters -
 * returns true if legal, false otherwise */
int isLegalString(char const * string)
{
    int i=0; /* index for chars in string */
    if(string[i]=='\"') /* starts with " */
    {
        i++;
        while(string[i] != '\0' && string[i] != '\"') /* goes through string until reaches end or " */
        {
            i++;
        }
        if(string[i] == '\"') {
            if (string[i + 1] != '\0')
                return -1; /* the string is legal but there is extra text after */
            return TRUE; /*The string is legal*/
        }
    }
    return FALSE;
} /* end of method isLegalString */
/*  checks if a given parameter is legal for a command regarding whether its source or destination
 * parameters -
 * command - the command
 * destORsource - flag for source or dest param
 * returns true if legal, false otherwise */
int checkLegalParams (cmd command,int type, int destORsource)
{
    if(destORsource==0) /*source param = 0, dest param = 1*/
    {
        if((getCmdSource(command,type)==type)) /* checks if the parameter type is legal */
            return TRUE;
    }
    else
    {
        if((getCmdDest(command,type)==type))
            return TRUE;
    }
    return FALSE;
} /* end of method checkLegalParams */
/*  check if a line is a comment line
 * parameters -
 * commandLine - the command line
 * returns true if its a comment line, false otherwise */
int isComment (char const * commandLine)
{

    if (commandLine[0] == ';') /* a comment line starts with ; */
        return TRUE;
    return FALSE;
} /* end of method isComment */
/*  checks if the given command is a correct define definition
 * parameters -
 * commandLine - the command line
 * returns true if its a comment line, false otherwise */
int isLegalMacroDef(char const * commandLine)
{
    int i; /* index */
    for(i=0 ; commandLine[i] != '=' ; i++) /* goes through string until reaches = */
    {
        if (commandLine[i] == '\0') /* if reached end of line before = */
            break;
    }
    if (commandLine[i] == '=') /* macro definition */
        return TRUE;
    else
        return FALSE;
} /* end of method isLegalMacroDef */
