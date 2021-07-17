/* Methods in this file are used in file related operations */
#include "header.h"

void createOddBase(void * ,char * );

/* checks for legal file name (ends with .as)
 * parameters -
 * fileName - the filename
 * returns true if legal, false otherwise */
int checkFileName (char const * fileName)
{
    int i=0; /* index */
    if (!fileName) /* if filename is NULL */
        return FALSE;
    while (fileName[i] !='.') /* goes through file name string */
    {
        if (fileName[i] == '\0') /* if not legal */
            return FALSE;
        i++;
    }
    if (fileName[i] == '.' && fileName[i+1] == 'a' && fileName[i+2]=='s' && fileName[i+3] == '\0') /* if legal */
        return TRUE; /* checks if after dot we have "as" */
    else
        return FALSE;
}/* end of checkFileName method */
/* this method changes the extension of a given file name
 * parameters -
 * name - the file name
 * ex - the new extension
 */
void changeExtension (char * name,char * ex)
{
    int i; /* index on name string */
    if (!name) /* if no file name */
        return;
    for (i=0;name[i] !='.';i++)
    {
        if (name[i] == '\0') /* if we reached the end of the string */
            break;
        ; /* continue to next char */
    }
    if (name[i] != '.')
        name[i]='.';
    name[i+1]='\0'; /* reached after dot, ends string */
    strcat(name,ex); /* puts given extension at end of file name */
} /* end of changeExtension method */
/* this method prints to the output file a given word and the IC
 * parameters -
 * file_name - the file name
 * IC - the IC counter
 * word - the word data to print
 */
void printToFile(char * file_name,unsigned int IC,void * word)
{
    char codeLine[MAX_LINE]; /* buff for odd base line */
    FILE * fd= fopen(file_name,"a"); /* opening file to print to */
        if (!fd)
    {
        fprintf(stderr,"Error while opening file %s\n",file_name);
        exit(0);
    }
    createOddBase(word,codeLine); /* creates an odd base string out of the given word */
    fprintf(fd,"%04u\t%s",IC,codeLine); /* prints to file */
    fclose(fd);
} /* end of printToFile method */
/* method prints data words to file
 * parameters -
 * file_name - the file name
 * symbolList - the symbol list
 */
void printData (char * file_name,dataPtr *symbolList)
{
    dataPtr temp = *symbolList; /* temp pointer to point the data list head */
    changeExtension(file_name,"ob");
    while (temp) /* goes through each symbol in the list and prints in odd base */
    {
        unsigned int t = getDataInfo(temp); /* saves data */
        printToFile(file_name,getDataAddress(temp),&t); /* prints to file */
        temp=getNextData(temp); /* next word */
    }
} /* end of printData method */
/* method writes to the extern file
 * parameters -
 * file_name - the file name
 * label - the label
 * line - the line number
 */
void writeExtern (char * file_name, char * label, unsigned int line)
{
    FILE *fd;
    char externFile[MAX_LABEL+1]; /* extern file name */
    if (!file_name)
        return;
    strcpy(externFile,file_name); /* creates a file with .extern */
    changeExtension(externFile,"ext.tmp");
    fd = fopen(externFile,"a");
    if (!fd)
    {
        fprintf(stderr,"Error while creating entry file\n");
        exit(0);
    }
    fprintf(fd,"%s\t%04u\n",label,line); /* print to file */
    fclose(fd);

}/* end of writeExtern method */
/* method writes to the entry file
 * parameters -
 * file_name - the file name
 * label - the label
 * address - the data address
 */
void writeEntry (char * file_name, char * label, unsigned int address)
{
    FILE *fd;
    char entryFileName[MAX_LABEL+1]; /* entry file name */
    if (!file_name)
        return;
    strcpy(entryFileName,file_name); /* creates a file with .entry */
    changeExtension(entryFileName,"ent.tmp");
    fd = fopen(entryFileName,"a");
    if (!fd)
    {
        fprintf(stderr,"Error while creating entry file\n");
        exit(0);
    }
    fprintf(fd,"%s\t%04u\n",label,address); /* print to file */
    fclose(fd);

}/* end of writeEntry method */
/* method to remove any file extension
 * parameters -
 * file_name - the file name
 */
void removeExtension (char * file_name)
{
    int i; /* index */
    if (!file_name)
        return;
    for(i=0;file_name[i] !='.';i++) /* going over the file name until the dot */
    {
        if (file_name[i] == '\0') /* if no dot */
            return;
    }
    file_name[i]='\0'; /* removing extension */
}/* end of removeExtension method */

/*  method creates an .ob.tmp file with the IC and DC
 * parameters -
 * file_name - the file name
 * IC - IC counter
 * DC - DC counter */
void createObFile(char *file_name, unsigned int IC, unsigned int DC) {
    FILE *fd; /* file pointers */
    if (!file_name)
        return;
    changeExtension(file_name, "ob.tmp"); /* changing extension and creating the ob file */
    fd = fopen(file_name, "w");
    if (!fd) { /* if cannot create the ob file */
        fprintf(stderr, "Error creating .ob file\n");
        exit(0);
    }
    fprintf(fd, "   %u\t%u\n", IC - MEMORY_START, DC); /* prints IC and DC to top of file */
    fclose(fd);
} /* end of createObFile method */

/* method to check if file is exist
 * parameters -
 * file_name - the file name
 * returns true if exists, false otherwise
 */
int exists(const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "r"))) /* checks if the file exists */
    {
        fclose(file);
        return TRUE; /* return true if exists */
    }
    return FALSE; /* false if not exists */
}/* end of exists method */

/* method to remove .tmp extension
 * parameters -
 * file_name - the file name
 */
void changeTempFilesExtension (char * file_name)
{
    char name[MAX_LABEL]; /* temp string */
    if (!file_name)
        return;
    removeExtension(file_name); /* removing extension */
    strcpy(name,file_name); /* copy the file name without the extension */
    changeExtension(name,"ent"); /* the name to change to */
    changeExtension(file_name,"ent.tmp"); /* the name to change */
    if (exists(name)) /* if file exist remove it */
        remove(name);
    rename(file_name,name); /* rename the file name */
    removeExtension(file_name); /* removing extension */
    strcpy(name,file_name); /* copy the file name without extension */
    changeExtension(name,"ext"); /* the name to change to */
    changeExtension(file_name,"ext.tmp"); /* the name to change */
    if (exists(name)) /* if file exist remove it */
        remove(name);
    rename(file_name,name); /* rename the file name */
    removeExtension(file_name); /* removing extension */
    strcpy(name,file_name); /* copy the file name without extension */
    changeExtension(name,"ob"); /* the name to change to */
    changeExtension(file_name,"ob.tmp"); /* the name to change */
    if (exists(name)) /* if file exist remove it */
        remove(name);
    rename(file_name,name); /* rename the file name */
}/* end of changeTempFileExtensions method */

/*  method deletes *.ext.tmp *.ent.temp and *.ob.tmp files
 * parameters -
 * file_name - the file name */
void deleteTempFiles(char * file_name)
{
    changeExtension(file_name,"ext.tmp"); /* change extension */
    remove(file_name); /* remove file */
    changeExtension(file_name,"ent.tmp"); /* change extension */
    remove(file_name); /* remove file */
    changeExtension(file_name,"ob.tmp"); /* change extension */
    remove(file_name); /* remove file */
    changeExtension(file_name,"as"); /* change the extension back to the original */
} /* end of method deleteTempFiles */
