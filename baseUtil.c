#include "header.h"
#define SIZE_OF_WORD_ODD_BASE 8
/* changing a data word into odd base 4 word
 * parameters -
 * data - pointer to the data word
 * out - the string to store the odd base 4 representation */

void createOddBase(void * data,char * out)
{
    unsigned int * code = (unsigned int *)data; /* input in binary */
    int i; /* index to go over the data word */
    int j=SIZE_OF_WORD_ODD_BASE; /* index to store the output */
    unsigned int digit; /* to store a digit */
    unsigned int mask =3; /* mask */
    char zero = '*';  /* initials the odd base values */
    char one = '#';
    char two = '%';
    char three = '!';
    out[j--]='\0'; /* last output char */
    out[j--]='\n'; /* end of output */
    for (i = 0; i <SIZE_OF_DATA_WORD ; i+=2) { /* going over the data word */
        digit = mask & ((*code)>>i); /* gets the digit  */
        switch (digit) { /* digit can be between 0-3 */
        case 0:
            out[j--] = zero;
            break;
        case 1:
            out[j--] = one;
            break;
            case 2:
                out[j--]=two;
                break;
            case 3:
                out[j--]=three;
                break;
            default:
                break;
        } /*end of switch */
    } /* end of for loop */
} /* end of method createOddBase */
