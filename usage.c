#include "mysync.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Prints out the usage statement for the mysync CLI tool.
 *
 */
void usage()
{
       printf("Example Usage: ./mysync [options] directory1 directory 2 ... directory x\n");

       printf("Program Options: -a -n -p -v -r -i -o\n");
       exit(EXIT_FAILURE);
}