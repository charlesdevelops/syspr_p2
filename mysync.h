//  CITS2002 Project 2 2023
//  Student1:   23196696   CHARLES-AIDAN-WATSON
//  Student2:   22705919   JAKEM-PINCHIN

#ifndef MYSYNC_H
#define MYSYNC_H
#include <sys/param.h>
#include <stdbool.h>

// Declare global flag variables
extern bool flag_a;
extern bool flag_n;
extern bool flag_p;
extern bool flag_v;
extern bool flag_o;
extern bool flag_i;

// Structure which is used to hold a single regex
struct regex
{
    char *regex;
    bool type; // true = o, false = i
};

// Structure that stores a list of regexes
struct regexList {
    struct regex *include_patterns;
    int include_count;
    struct regex *exclude_patterns;
    int exclude_count;
};

// Structure to hold files information, the file path and when it was last modified
struct FileDetails
{
    char path[MAXPATHLEN];
    time_t lastModified;
};

// Structure to hold directory information, directory name, file information, file count
struct Directory
{
    char dirname[MAXPATHLEN];
    struct FileDetails *files;
    int file_count;
};

// int argPassing(int argc, char *argv[]);
int argPassing(int argc, char *argv[], struct regexList *regex_list);

struct Directory *directoryInfo(int argc, char *argv[], int index);

struct FileDetails *checkdir(const char *dirName, int *fileCount);

void usage();

void synchronizeDirectories(struct Directory dir_list[], int num_dirs, struct regexList *regex_list);

#endif // MYSYNC_H