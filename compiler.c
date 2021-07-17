#include "header.h"

/* compiling a file name
 * parameters -
 * file_name - the filename to process */
void compile(char *file_name) {
    symbolPtr symbolList = NULL; /* symbol list */
    int error = OFF; /* error flag */
    dataPtr dataList = NULL; /* data list */
    unsigned int IC = MEMORY_START, DC = 0; /* ic and dc counters */
    printf("\nOpening file %s\n", file_name);
    if (!checkFileName(file_name)) { /* checking if file has a legal extension */
        fprintf(stderr, "Error: Wrong file extension %s (*.as only)\n", file_name);
        return; /* return when the file is not in .as format */
    }
    printf("Compiling file\n");
    if (!(firstPass(file_name, &dataList, &symbolList, &IC, &DC))) /* doing the first pass */
        error = ON; /* if firstPass returns error */
    updateDataAdd(&dataList, IC, &symbolList); /* update the symbol list and data list addresses */
    secondPass(file_name, &dataList, &symbolList, &IC, &DC, &error); /* doing second pass */
    freeLists(&symbolList, &dataList); /* frees data and symbol lists */
    if (error == OFF) /* if no error print done */
        printf("Done with no errors.\n");
} /* end of compile method */
