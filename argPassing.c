#include "mysync.h"
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>

#define OPTLIST "anprvi:o:" // Accepted Arguments by getopt

// Set global flag variables, switched to true if used in command line
bool flag_a = false;
bool flag_n = false;
bool flag_p = false;
bool flag_v = false;
bool flag_i = false;
bool flag_o = false;

//  CONVERT A FILENAME PATTERN (a glob) TO A REGULAR EXPRESSION.
//  FILENAME PATTERNS MAY NOT INCLUDE DIRECTORY SEPARATORS.
//	ON SUCCESS - A REGULAR EXPRESSION WILL BE RETURNED IN DYNAMICALLY
//		     ALLOCATED MEMORY.
//	ON FAILURE - A NULL POINTER WILL BE RETURNED.
char *glob2regex(char *glob)
{
    char *re = NULL;

    if (glob != NULL)
    {
        re = calloc(strlen(glob) * 2 + 4, sizeof(char));
        if (re == NULL)
        {
            return NULL;
        }

        char *r = re;

        *r++ = '^';
        while (*glob != '\0')
            switch (*glob)
            {
            case '.':
            case '\\':
            case '$':
                *r++ = '\\';
                *r++ = *glob++;
                break;
            case '*':
                *r++ = '.';
                *r++ = *glob++;
                break;
            case '?':
                *r++ = '.';
                glob++;
                break;
            case '/':
                free(re);
                re = NULL;
                break;
            default:
                *r++ = *glob++;
                break;
            }
        if (re)
        {
            *r++ = '$';
            *r = '\0';
        }
    }
    return re;
}

extern char *strdup(const char *str);

/**
 * @brief Checks command line arugments for validity and flips flag switches based on presence
 *
 * @return 'optind' represents where options in command line end
 */
int argPassing(int argc, char *argv[], struct regexList *regex_list)
{

    // Check for valid amount of command line arguments
    if (argc < 2)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    int opt;
    // char *ifilenm = NULL; // Variable to store filename for -i
    // char *ofilenm = NULL; // Variable to store filename for -o

    while ((opt = getopt(argc, argv, OPTLIST)) != -1)
    {
        switch (opt)
        {
        case 'a':
            flag_a = true;
            break;

        case 'n':
            flag_n = true;
            break;

        case 'p':
            flag_p = true;
            break;

        case 'r':
            puts("-r unsupported at this time");
            exit(EXIT_FAILURE);
            break;

        case 'v':
            flag_v = true;
            break;

        case 'i':
            regex_list->include_patterns = realloc(
                regex_list->include_patterns, (regex_list->include_count + 1) * sizeof(struct regex)
            );
            regex_list->include_patterns[regex_list->include_count].regex = glob2regex(optarg);
            regex_list->include_patterns[regex_list->include_count].type = false;
            regex_list->include_count++;
            flag_i = true;
            break;

        case 'o':
            regex_list->exclude_patterns = realloc(
                regex_list->exclude_patterns, (regex_list->exclude_count + 1) * sizeof(struct regex)
            );
            regex_list->exclude_patterns[regex_list->exclude_count].regex = glob2regex(optarg);
            regex_list->exclude_patterns[regex_list->exclude_count].type = true;
            regex_list->exclude_count++;
            flag_o = true;
            break;

        case '?':
            usage();
        }
    }

    int num_dirs = argc - optind; // Variable to hold the number of directories in command line

    // Check if number of directories is more then 2
    if ((num_dirs) < 2)
    {
        fprintf(stderr, "Not Enough Directories\n");
        usage();
        exit(EXIT_FAILURE);
    }

    printf("Number of arguments:   %d\n", optind);
    printf("Number of directories: %d\n", argc - optind);

    return optind;
}
