#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <time.h>
#include <regex.h>

struct regexList *regexList;

extern char *strdup(const char *str);

/**
 * @brief Determines whether the given path is a file or not
 *
 * @param path The path to be checked
 * @return `int` 1 if the path is a valid file, 0 otherwise
 */
int isFile(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) == 0)
    {
        return S_ISREG(path_stat.st_mode);
    }
    return 0;
}

/**
 * @brief Returns a pointer to an array of filedetails for directory
 *
 * @param dirName Path to the directory
 * @param fileCount The number of files in the directory
 * @return struct FileDetails*
 */
struct FileDetails *checkdir(const char *dirName, int *fileCount)
{
    char *directoryPath = strdup(dirName);
    DIR *dir = opendir(directoryPath);
    if (dir == NULL)
    {
        perror("Error opening directory");
        free(directoryPath);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct FileDetails *fileDetails = NULL;
    *fileCount = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        char filePath[MAXPATHLEN];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        if (isFile(filePath))
        {
            struct stat fileStat;
            if (stat(filePath, &fileStat) == 0)
            {
                // memory allocated here for the new fileDetails
                struct FileDetails *newFileDet = realloc(fileDetails, (*fileCount + 1) * sizeof(struct FileDetails));
                if (newFileDet == NULL)
                {
                    perror("Memory allocation error");
                    free(directoryPath);
                    closedir(dir);
                    exit(EXIT_FAILURE);
                }

                fileDetails = newFileDet;
                strncpy(fileDetails[*fileCount].path, filePath, sizeof(fileDetails[*fileCount].path));
                fileDetails[*fileCount].lastModified = fileStat.st_mtime;
                (*fileCount)++;
            }
        }
    }

    closedir(dir);
    printf("File Details:\n");
    for (int i = 0; i < *fileCount; i++)
    {
        printf("File Path: %s\n", fileDetails[i].path);
        printf("Last Modified: %s", ctime(&fileDetails[i].lastModified));
    }

    free(directoryPath);
    return fileDetails;
}