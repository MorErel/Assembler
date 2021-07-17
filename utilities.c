#include "header.h"
/* checks if a string is an integer number
 * parameters -
 * token = the string to check
 * return 1 if its an integer 0 otherwise */
int isInt(char const *token) {
    int i; /* index */
    if (token == NULL) /* if input is empty return false */
        return FALSE;
    if (!isdigit(token[0]) && token[0] != '-' && token[0] !='+') { /* if first char is not a start of a number */
        return FALSE;
    }
    for(i=1;token[i] != '\0';i++) /* check if all digits are numbers */
    {
        if (!isdigit(token[i])) /* if not number return false */
            return FALSE;
    }
    return TRUE;

} /* end of isInt method */

/* checks if a string is a macro definition
 * parameters -
 * token - the string to check
 * returns 1 if its a macro 0 otherwise */
int isDefine (char * token)
{
    if (!strcmp(token,".define")) /* if true */
    {
        return TRUE;
    }
    else
        return FALSE;
} /* end of isDefine method */
/* checks if a string is a label definition
 * parameters -
 * token - the string to check
 * returns 1 if its a label 0 otherwise */
int isLabel(char ** name)
{
    int i; /* index */
    for (i=0;((*name)[i]) != '\0';i++) /* going over the string */
        ;
    i--;
    if (((*name)[i])==':') /* checking if last char is : */
    {
        (*name)[i]='\0'; /* removing the : and returns true */
        return TRUE;
    }
    return FALSE;
}
/* converting a integer into a string
 * parameters -
 * number - the number to covert
 * string - the string to store the int representation */
void itos(int number,char * string)
{
    sprintf(string,"%d",number);
}
/* checks if a string is an array and its in the symbol list
 * parameters -
 * name - the string to check
 * head - the symbol list
 * returns 1 if its a macro 0 otherwise */
int isArray(char const * name, symbolPtr head)
{
    char params [MAX_LABEL+1]; /* to store the index of the array */
    char arName [MAX_LABEL+1]; /* to store the array name */
    int i=0; /* index */
    int j=0;
    int index;
    while(name[i] != '\0' && name[i] != '[') /* storing the array name */
    {
        arName[j++] = name[i++];
    }
    arName[j]='\0';
    if(name[i] == '[') /* end of arr name  */
    {
        i++;
        if(legalLabelName(arName, head)) /* checks if the arr name is valid */
        {
            j=0;
            while(name[i] != '\0' && name[i] != ']') /* getting the index */
            {
                params[j++] = name[i++];
            }
            params[j]='\0';
            if(name[i] == ']') /* if the index has legal [ ] */
            {

                if(isInt(params)) /* check is the index is an integer */
                {
                    index = atoi(params);
                    if(index<0)
                    {
                        return -1; /*if array index is negative*/
                    }
                    else {
                        if(name[i+1]=='\0' || name[i+1] == '\n') /* if arr name legal and index is legal return true */
                            return TRUE;
                    }
                }

                else if(isMacro(params,head)) /* if the index is not an integer we check if the index is a macro def */
                {
                    index = getMacro(params,arName,head);
                    if(index<0)
                    {
                        return -1; /*if macro value is negative*/
                    }
                    else {
                        if(name[i+1]=='\0' || name[i+1] == '\n') /* if legal returns true */
                            return TRUE;
                    }
                }
                else if(legalLabelName(params,head)==1) /* if its not macro and not integer we check if it can be a legal def of var */
                {
                        if(name[i+1]=='\0' || name[i+1] == '\n') /* if legal returns true */
                            return TRUE;

                }
            }
        }

    }
    return FALSE; /* if not array returns false */
} /* end if isArray method */

/* check the type of a parameter
 * parameters -
 * param - the string to check
 * symbolList - the symbol list
 * returns 0 if its a number def, 1 if its a symbol, 2 if its an array, 3 if its a register, if not legal returns -1 */
int paramType (char * param, symbolPtr symbolList)
{
    if (param[0]=='#' || isMacro(param,symbolList))
        return 0;
    if (isSymbol(param,symbolList))
        return 1;
    if (isArray(param,symbolList))
        return 2;
    if (isRegister(param))
        return 3;
    return -1;

} /* end of paramType method */

/* getting the address of a an array symbol
 * parameters -
 * param - the string to check
 * symbolList - the symbol list
 * returns the address if the arr */
int getArrAddress(char * arr,symbolPtr symbolList)
{
    char * name = strtok(arr,"["); /* getting the arr name */
    return getSymVal(name,symbolList); /* return the value of the symbol (in this case the address) */
} /* end of getArrAddress method */
/* getting the index of a an array symbol
 * parameters -
 * arr - the array
 * symbolList - the symbol list
 * returns the value of the index */
int getArrIndex (char * arr,symbolPtr symbolList)
{
    char * token; /* to store break the string */
    char val[MAX_LINE]; /* to store the value */
    int i=0; /* index */
    while (arr[i] != '[') /* skipping the arr name */
        i++;
    i++;
    strcpy(arr,arr+i); /* make arr to point to the start of the index param */
    token = strtok(arr,"]"); /* getting the string between the [] */
    if (isMacro(token,symbolList)) /* checks if its a macro def */
         getMacro(token,val,symbolList); /* getting the macro val */
    else
        strcpy(val,token); /* if its a not a macro than its a number , copy it */
return atoi(val); /* returns the value of the index as integer */
} /* end of getArrIndex method */