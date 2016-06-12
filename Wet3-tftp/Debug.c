#include <stdio.h>
#include <stdlib.h>
#include "Debug.h"

/*************************/
/* Function definitions  */
/*************************/

void die(char *s)
{
    perror(s);
    exit(1);
}