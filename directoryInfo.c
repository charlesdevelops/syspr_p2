#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <time.h>

extern char *strdup(const char *str);

/**
 * @brief Checks whether a filepath is a directory or not
 *
 * @param path String specifying the directory location
 * @return `true` The path is a valid directory
 * @return `false` The path is not a valid directory
 */
bool isDirectory(const char *path)
{
    DIR *dir = opendir(path);
    if (dir != NULL)
    {
        closedir(dir);
        return true;
    }
    return false;
}

/**
 * @brief Adds all directory infomation into a 'Directory' struct containing name, fileinfo, file_count
 *
 * @param index Index of where in argument line directories start
 * @return 'dir_list' contains all directories information
 */
struct Directory *directoryInfo(int argc, char *argv[], int index)
{
    int num_dirs = argc - index; // Variable holding number of directories in command line
    struct Directory *dir_list = malloc(num_dirs * sizeof(struct Directory));

    if (dir_list == NULL)
    {
        perror("Memory allocation error in assigning directories");
        exit(EXIT_FAILURE);
    }

    argv += index;

    for (int i = 0; i < num_dirs; ++i)
    {
        if (!isDirectory(argv[i]))
        {
            fprintf(stderr, "Error: %s is not a valid directory.\n", argv[i]);
            usage();
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Directory %d: %s\n", i + 1, argv[i]);
            strcpy(dir_list[i].dirname, argv[i]);
        }
    }

    for (int x = 0; x < num_dirs; ++x)
    {
        dir_list[x].files = checkdir(dir_list[x].dirname, &dir_list[x].file_count);
    }

    puts("-----------------------------------\n");

    for (int x = 0; x < num_dirs; ++x)
    {
        printf("%s\n", dir_list[x].dirname);
        printf("%d\n", dir_list[x].file_count);
        for (int i = 0; i < dir_list[x].file_count; i++)
        {
            printf("File Path: %s\n", dir_list[x].files[i].path);
            printf("Last Modified: %s\n", ctime(&dir_list[x].files[i].lastModified));
        }
    }
    return dir_list;
}