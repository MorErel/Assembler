#include "header.h"

/* represents a symbol */
typedef struct symbol {
    char name[MAX_LABEL+1]; /* name of symbol */
    int data; /* data in symbol */
    int external; /* if symbol is external */
    int op_related; /* if symbol is op related */
    int macro; /* if symbol is macro */
    struct symbol *next; /* next symbol in the list */
} symbol;

/* represents a data word */
typedef struct dataWord {
    unsigned int datainfo:SIZE_OF_DATA_WORD;
    unsigned int address;
    struct dataWord *next;
}dataWord;

/* represents a register */
typedef struct rg {
    char *name; /* name of register */
    int number; /* number of register */
} rg;

typedef struct set {
    char *name;
} set;

/* registers array */
rg registers[] = {
        {"r0",  0},
        {"r1",  1},
        {"r2",  2},
        {"r3",  3},
        {"r4",  4},
        {"r5",  5},
        {"r6",  6},
        {"r7",  7},
        {"PSW", 8},

}; /* represents the registers */

set setters[]= {
        {"entry"},
        {"extern"},
        {"string"},
        {"data"}
}; /* represents the setters */

/* method checks if a given string is a register */
int isRegister (char * name)
{
    int i;
    for (i=0 ; i<NUM_OF_REGISTERS ; i++) /* compare the given string with the names of the registers */
    {
        if (strcmp(name,registers[i].name)==0)
        {
            return TRUE; /* found the register */
        }
    }
    return FALSE; /* didn't find register */
} /* end of isRegister method */

/* method check if a given string is a setter
 * parameters -
 * token - the string
 * returns the instruction number , -1 otherwise */
int isSetter(char * token) {
    int i;
	if (token[0] != '.')
	    return -1;
	strcpy(token,token+1);
    for (i=0 ; i < NUM_OF_SETTERS ; i++) {
        if (strcmp(token, setters[i].name) == 0) {
            return i; /* returns the intruction number */
        }
    }
    return -1; /* not a setter */
}/* end of isSetter method */

/* gets the register number
 * parameters -
 * name - string represent the register
 * returns the register's number */
int getRegister(char * name)
{
    int i;
    if (!name)
        return -1;
    for (i=0;i<NUM_OF_REGISTERS;i++)
    {
        if (!(strcmp(name,registers[i].name))) /* comparing string with registers list */
            return registers[i].number; /* found register and returns number */
    }
    return -1;
} /* end of getRegister method */

/* method adds a symbol to the list
 * parameters: symbol list head pointer, name of symbol, if it's external/op related/ macro, and its data*/
int addSymbol(symbolPtr *head,char *name,int external,int op_related,int macro,int data) {
    symbolPtr temp = *head;
    symbolPtr node = (symbolPtr)malloc(sizeof(symbol)); /* allocating memory */
    if (!node) /* if cannot allocate memory */
    {
        fprintf(stderr,"Cannot allocate memory\n");
        return FALSE;
    }
    node->data = data; /* setting nodes data */
    node->external=external;
    node->op_related = op_related;
    node->macro=macro;
    node->next=NULL;
    strcpy(node->name,name);
    if (!temp) { /* if list is empty */
        *head = node; /* setting head field to point this node */
        return TRUE; /* stop */
    }
    temp=*head;
    while (temp->next) /* getting the last node */
        temp = temp->next;
    temp->next = node; /* makes it point to this node */
    return TRUE;
} /* end of addSymbol method */

/* method adds data to the data list
 * parameters: data list pointer, address of data, data */
int addData(dataPtr *head, unsigned int address, int data) /* add to symbol list */
{
    dataPtr temp = *head;
    dataPtr node = (dataPtr) malloc(sizeof(dataWord)); /* allocating memory */
    if (!node) /* if cannot allocate memory */
        return FALSE;
    node->address = address; /* setting all the data in current word */
    node->datainfo=data;
    node->next=NULL;

    if (!temp) /* if list is empty */
    {
        *head=node; /* setting head field to point this node */
        return TRUE; /* stop */
    }
    while (temp->next)
    {
        temp=temp->next; /* getting the last node */
    }
    temp->next=node; /* makes it point to this node */
    return TRUE;
}/* end of addData method */

/* this method updates the data list addresses after finishing the first pass according to IC
 * parameters: data list, instruction counter, symbol list*/
void updateDataAdd( dataPtr * list, unsigned int IC,symbolPtr *symbolList)
{
    dataPtr temp = *list;
    symbolPtr t = *symbolList;
    while (temp) /* goes through the data list and updates the address */
    {
        temp->address+=IC;
        temp = temp->next;
    }
    while (t) /* goes through symbol list and updates address of data related symbols */
    {
        if (!(t->macro) && !(t->op_related) && !(t->external))
        {
            t->data+=IC;
        }
        t=t->next;

    }
} /* end of updateDataAdd method */

/* method frees the memory we allocated for the symbol list */
void freeSymbolList(symbolPtr *hptr)
{
    symbolPtr lastSymbol; /* to store the previous node */
    while (*hptr) /* while its not the last node */
    {
        lastSymbol=*hptr; /* setting last node to point  this node */
        *hptr=(*hptr)->next; /* moving to the next node */
        free(lastSymbol); /* free the nodes memory */
        *hptr=NULL;
    }
} /* end of  freeSymbolList method */

/* frees the data list
 * parameters
 * hptr - pointer to the list */
void freeDataList(dataPtr *hptr)
{
    dataPtr lastNode; /* to store the previous node */
    while (*hptr) /* while its not the last node */
    {
        lastNode=*hptr; /* setting last node to point  this node */
        *hptr=(*hptr)->next; /* moving to the next node */
        free(lastNode); /* free the nodes memory */
    }
    *hptr=NULL;
} /* end of  freeDataList method */

/* frees both data list and symbol list
 * parameters -
 * symbol - pointer to the symbol list
 * data - pointer to the data list */
void freeLists(symbolPtr *symbol,dataPtr *data)
{
    freeSymbolList(symbol);
    freeDataList(data);
} /* end of freeLists method */

/* checks if the string is a symbol on our list */
int isSymbol(char *name ,symbolPtr head)
{
    symbolPtr temp;
    char tmpString[MAX_LABEL] = ".";
    temp=head;
    while (temp != NULL) /* goes through the list */
    {
        if (strcmp(name,temp->name)==0) /* compare symbols to current string */
            return TRUE; /* found symbol */
        temp=temp->next;
    }
    strcat(tmpString,name);
    if(isSetter(tmpString) != -1)
        return TRUE;
    return FALSE;
}/* end of isSymbol method */

/* checks if a given string is a macro from our symbol list */
int isMacro(char * token,symbolPtr head)
{
    symbolPtr temp;
    temp=head;
    while (temp != NULL) /* goes through symbol list */
    {
        if (strcmp(token,temp->name)==0 && (temp->macro==TRUE)) /* string is a name of a macro */
            return TRUE;
        temp=temp->next;
    }
    return FALSE; /* string is not a macro */
}/* end of isMacro method */

/* checks if a given string is an external parameter */
int isExtern(char * token,symbolPtr head)
{
    symbolPtr temp;
    temp=head;
    while (temp != NULL) /* goes through symbol list */
    {
        if (strcmp(token,temp->name)==0 && (temp->external==TRUE)) /* string is a name of external param */
            return TRUE;
        temp=temp->next;
    }
    return FALSE;
} /* end of isExtern method */

/* changes parameter "number" to a string representation of macro's value, and returns it's int value */
int getMacro(char * name, char * number, symbolPtr head)
{
    symbolPtr temp=head;
    while (temp != NULL) /* searches macro through list */
    {
        if (strcmp(name,temp->name)==0) /* finds the macro in the list */
        {
            itos(temp->data,number); /* make a string from macro's value */
            return temp->data; /* return data as int */
        }
        temp=temp->next; /* check next symbol */
    }
    return FALSE;
}/* end of getMacro method */

/* method return a given symbol value */
int getSymVal (char * sym , symbolPtr symbolList)
{
    while (symbolList) /* searches through symbol list */
    {
        if (strcmp(symbolList->name,sym)==0)
        {
            return symbolList->data; /* found the name of the symbol and return its data */
        }
        symbolList=symbolList->next; /* check next symbol */
    }
    return FALSE;
} /* end of getSymVal method */

/* gets the data of a data word
 * parameters -
 * data - the data word
 * returns the value of the data word */
unsigned int getDataInfo(dataPtr data)
{
    return data->datainfo;
} /* end of getDataInfo method */

/* gets the address of a data word
 * parameters -
 * data - the data word
 * returns the address of the data word */
unsigned int getDataAddress(dataPtr data)
{
    return data->address;
} /* end of getDataAddress */
/* gets the next data word from the data list
 * parameters -
 * data - the data word
 * returns the next data word */
dataPtr getNextData(dataPtr data)
{
    return data->next;
} /* end of getNextData method */

/* struct represents a general operation and its details */
typedef struct opCmd {
    char name[MAX_LABEL]; /* name of the operation */
    int params; /* number of parameters it gets */
    unsigned int opcode; /* operation's code */
    int sourceParam[NUM_OF_PARAM_TYPES]; /* legal type of parameters for source parameter */
    int destParam[NUM_OF_PARAM_TYPES]; /* legal type of parameters for destination parameter */
} opCmd;

/* represents each of the operations, their op code, number of parameters */
/* also represents which kind of parameters are legal for source and destination parameter for each operation */
opCmd operations[] = {
        {"mov",  2, 0, {INSTANT,DIRECT,CONSTANT,REGISTER},{-1,DIRECT,CONSTANT,REGISTER}},
        {"cmp",  2, 1,{INSTANT,DIRECT,CONSTANT,REGISTER},{INSTANT,DIRECT,CONSTANT,REGISTER}},
        {"add",  2, 2,{INSTANT,DIRECT,CONSTANT,REGISTER},{-1,DIRECT,CONSTANT,REGISTER}},
        {"sub",  2, 3, {INSTANT,DIRECT,CONSTANT,REGISTER},{-1,DIRECT,CONSTANT,REGISTER}},
        {"not",  1, 4,{-1,-1,-1,-1},{-1,DIRECT,CONSTANT,REGISTER}},
        {"clr",  1, 5,{-1,-1,-1,-1},{-1,DIRECT,CONSTANT,REGISTER}},
        {"lea",  2, 6,{-1,DIRECT,CONSTANT,-1},{-1,DIRECT,CONSTANT,REGISTER}},
        {"inc",  1, 7,{-1,-1,-1,-1},{-1,DIRECT,CONSTANT,REGISTER}},
        {"dec",  1, 8,{-1,-1,-1,-1},{-1,DIRECT,CONSTANT,REGISTER}},
        {"jmp",  1, 9,{-1,-1,-1,-1},{-1,DIRECT,-1,REGISTER}},
        {"bne",  1, 10,{-1,-1,-1,-1},{-1,DIRECT,-1,REGISTER}},
        {"red",  1, 11,{-1,-1,-1,-1},{-1,DIRECT,CONSTANT,REGISTER}},
        {"prn",  1, 12,{-1,-1,-1,-1},{INSTANT,DIRECT,CONSTANT,REGISTER}},
        {"jsr",  1, 13,{-1,-1,-1,-1},{-1,DIRECT,-1,REGISTER}},
        {"rts",  0, 14,{-1,-1,-1,-1},{-1,-1,-1,-1}},
        {"stop", 0, 15,{-1,-1,-1,-1},{-1,-1,-1,-1}}
};
/* gets the command opCode
 * parameters -
 * command - the command
 * returns the command opcode */
unsigned int getCmdOpCode (cmd command)
{
    return command->opcode;
} /* end of getCmdOpCode */

/* gets the commands number of parameters
 * parameters -
 * command - the command
 * returns the number of parameters */
int getCmdParams (cmd command)
{
    return command->params;
} /* end of getCmdParams */

/* gets the commands source parameters
 * parameters -
 * command - the command
 * type - the param type
 * returns the type */
int getCmdSource (cmd command,int type)
{
    return command->sourceParam[type];
} /* end of getCmdSource */

/* gets the commands dest parameters
 * parameters -
 * command - the command
 * type - the param type
 * returns the type */
int getCmdDest (cmd command,int type)
{
    return command->destParam[type];
} /* end of getCmdDest method */

/* method to check if a given string is equals to one of the valid commands
 * parameters:
 * token - the string to check
 * returns 1 if command is valid, 0 otherwise */
int isOpCode(char *token) {
    int i; /* index */
    if (token == NULL) /* if string is empty */
        return FALSE;
    for (i = 0; i<NUM_OF_COMMANDS; i++) { /* going over the commands array */
        if (strcmp(token, operations[i].name) == 0) /* if string is equals to one of the commands name */
            return TRUE;
    }
    return FALSE;

} /* end of isCmdValid method */

/* Method to get a command
 * parameters -
 * token = the string
 * command - to store the command */
void getCmd(char *token, cmd *command)
{
    int i; /* index */
    if (token == NULL) /* if string is empty */

    {
        *command=NULL;  /* not a command */
        return;
    }
    for (i = 0; i<NUM_OF_COMMANDS; i++) { /* going over the commands array */
        if (strcmp(token, operations[i].name) == 0) /* if string is equals to one of the commands name */
        {
            *command = operations+i; /* storing the command */
            return;
        }
    }
    *command=NULL;
} /* end of getCmd method */
/* method to get a command name
 * parameters -
 * command the command
 * name - the string to store the name of command */
void getCmdName (cmd command, char * name)
{
	if (command && name)
	    strcpy(name,command->name);
} /* end of getCmdName method */
