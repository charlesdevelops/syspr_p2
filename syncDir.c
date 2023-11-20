#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <regex.h>

struct regexList *regexList;

/**
 * @brief Checks if a file is hidden or not
 *
 * @param filename The path to the filename to be checked
 * @return true
 * @return false
 */
bool isHidden(const char *filename)
{
    bool isHiddenFile = (strstr(filename, "/.") != NULL) ? true : false;
    return isHiddenFile;
}

/**
 * @brief Returns the index of a regex that matches a given path
 *
 * @param regex_list The list of regexes to be checked
 * @param path The path to be checked against the regexes
 * @return `int` The index of the matching regex
 */
int regexMatch(struct regexList *regex_list, char *path) {
    for (int i = 0; i < regex_list->include_count; ++i) {
        regex_t regex;
        if (regcomp(&regex, regex_list->include_patterns[i].regex, REG_EXTENDED) == 0) {
            if (regexec(&regex, path, 0, NULL, 0) == 0) {
                regfree(&regex);
                return i;
            }
            regfree(&regex);
        }
    }
    return -1;
}

/**
 * @brief returns false if we need to take regex into account.
 *
 * @param regex_list the list of regexes to process
 * @param path the path to match against regexes
 * @return `true` - ignore the file (dont copy)
 * @return `false` - copy the file as usual, ignore regexes
 */
bool ignoreFileDueToRegex(struct regexList *regex_list, const char *path) {
    for (int i = 0; i < regex_list->exclude_count; ++i) {
        regex_t regex;
        if (regcomp(&regex, regex_list->exclude_patterns[i].regex, REG_EXTENDED) == 0) {
            if (regexec(&regex, path, 0, NULL, 0) == 0) {
                regfree(&regex);
                return true;
            }
            regfree(&regex);
        }
    }

    for (int i = 0; i < regex_list->include_count; ++i) {
        regex_t regex;
        if (regcomp(&regex, regex_list->include_patterns[i].regex, REG_EXTENDED) == 0) {
            if (regexec(&regex, path, 0, NULL, 0) == 0) {
                regfree(&regex);
                return false;
            }
            regfree(&regex);
        }
    }
    return true;
}

/**
 * @brief Compares file names, ignoring the path before them. Returns bool `true` if identical.
 *
 * @param path1 Path to first file to compare
 * @param path2 Path to second file to compare
 * @return `true` Matching filename
 * @return `false` Non-matching filename
 */
bool compareFilenames(const char *path1, const char *path2)
{
    const char *filename1 = strrchr(path1, '/');
    const char *filename2 = strrchr(path2, '/');

    if (filename1 == NULL || filename2 == NULL)
    {
        return false;
    }

    filename1++;
    filename2++;

    return strcmp(filename1, filename2) == 0;
}

/**
 * @brief Copies a file from source to destination
 *
 * @param source Source file path
 * @param destination Destination file path
 */
void copyFile(const char *source, const char *destination)
{
    if (isHidden(source) && !flag_a)
    {
        printf("skipping hidden file: %s\n", source);
        return;
    }

    if (strcmp(source, destination) == 0)
    {
        if (flag_v)
        {
            printf("skipping same file: %s\n", source);
        }
        return;
    }
    else if (flag_v || flag_n)
    {
        printf("copying file %s to %s\n", source, destination);
    }
    if (flag_n)
    {
        return;
    }
    if (!ignoreFileDueToRegex(regexList, source))
    {
        if (flag_v)
        {
            printf("skipping file due to regex: %s\n", source);
        }
        return;
    }

    FILE *sourceFile = fopen(source, "rb");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    FILE *destinationFile = fopen(destination, "wb");
    if (destinationFile == NULL)
    {
        perror("Error opening destination file");
        fclose(sourceFile);
        exit(EXIT_FAILURE);
    }

    int filecontent;
    while ((filecontent = fgetc(sourceFile)) != EOF)
    {
        fputc(filecontent, destinationFile);
    }
    fclose(sourceFile);
    fclose(destinationFile);

    // permissions setting using chmod after file writing is complete
    if (flag_p)
    {
        struct stat sourceStat;
        if (stat(source, &sourceStat) == -1)
        {
            perror("Failed to get source file permissions");
            exit(EXIT_FAILURE);
        }

        if (chmod(destination, sourceStat.st_mode) == -1)
        {
            perror("Failed to set destination file permissions");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * @brief Determines whether a file is present in the masterlist of files
 *
 * @param all_files The masterlist of allfiles being built
 * @param num_files The number of files present in the masterlist
 * @param filename The filename to be checked against the masterlist
 * @return `true` if the file is present in the masterlist
 * @return `false` if the file is not present in the masterlist
 */
bool fileInAllFiles(struct FileDetails *all_files[], int num_files, char *filename)
{
    for (int i = 0; i < num_files; ++i)
    {
        if (compareFilenames(all_files[i]->path, filename))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns the index of a file contained in the masterlist of files
 *
 * Will return -1 if no match found, however this should never happen as
 * the file should be checked for existence before calling this function.
 *
 * @param all_files The masterlist of allfiles being built
 * @param num_files The number of files present in the masterlist
 * @param filename  The filename to be checked against the masterlist
 * @return `int` The index of the matching file
 */
int indexOfMatchingFile(struct FileDetails *all_files[], int num_files, char *filename)
{
    for (int i = 0; i < num_files; ++i)
    {
        if (strcmp(basename(all_files[i]->path), filename) == 0)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Writes a list of all files to all specified directories
 *
 * @param dir_list The list of destination directories
 * @param num_dirs The number of destination directories
 * @param all_files The masterlist of files
 * @param file_counter The number of files present in the masterlist
 */
void overwriteAllDirsWithAllFiles(struct Directory dir_list[], int num_dirs, struct FileDetails *all_files[], int file_counter)
{
    for (int i = 0; i < num_dirs; ++i)
    {
        for (int j = 0; j < file_counter; ++j)
        {
            char *filename = basename(all_files[j]->path);
            char *destination = (char *)calloc(MAXPATHLEN, sizeof(char));
            strcpy(destination, dir_list[i].dirname);
            strcat(destination, "/");
            strcat(destination, filename);
            copyFile(all_files[j]->path, destination);
            free(destination);
        }
    }
}

/**
 * @brief Syncs files between a list of directories
 *
 * @param dir_list The list of directories to be synced
 * @param num_dirs The number of directories in dir_list
 * @param regex_list The list of regexes to be applied to the directories
 */
void synchronizeDirectories(struct Directory dir_list[], int num_dirs, struct regexList *regex_list) {
    regexList = regex_list;
    int max_files = 0;
    int file_counter = 0;
    for (int i = 0; i < num_dirs; ++i) {
        max_files += dir_list[i].file_count;
    }

    struct FileDetails **all_files = (struct FileDetails **)calloc(max_files, sizeof(struct FileDetails *));

    // add first directory to the list so can recursively check the rest
    for (int i = 0; i < dir_list[0].file_count; ++i) {
        all_files[i] = &dir_list[0].files[i];
        file_counter++;
    }

    // skip first dir hence start at 1
    for (int i = 1; i < num_dirs; ++i) {
        for (int j = 0; j < dir_list[i].file_count; ++j) {
            bool excludeFile = false;
            bool includeFile = false;

            for (int k = 0; k < regex_list->include_count; ++k) {
                regex_t regex;
                if (regcomp(&regex, regex_list->include_patterns[k].regex, REG_EXTENDED) == 0) {
                    if (regexec(&regex, basename(dir_list[i].files[j].path), 0, NULL, 0) == 0) {
                        includeFile = true;
                    }
                    regfree(&regex);
                }
            }

            for (int k = 0; k < regex_list->exclude_count; ++k) {
                regex_t regex;
                if (regcomp(&regex, regex_list->exclude_patterns[k].regex, REG_EXTENDED) == 0) {
                    if (regexec(&regex, basename(dir_list[i].files[j].path), 0, NULL, 0) == 0) {
                        excludeFile = true;
                    }
                    regfree(&regex);
                }
            }

            if (includeFile && !excludeFile) {
                if (!fileInAllFiles(all_files, file_counter, basename(dir_list[i].files[j].path))) {
                    all_files[file_counter] = &dir_list[i].files[j];
                    file_counter++;
                } else {
                    int index = indexOfMatchingFile(all_files, file_counter, basename(dir_list[i].files[j].path));
                    if (index == -1) {
                        puts("indexing error");
                        exit(EXIT_FAILURE);
                    } else if (dir_list[i].files[j].lastModified < all_files[index]->lastModified) {
                        all_files[index] = &dir_list[i].files[j];
                    }
                }
            }
        }
    }
    if (flag_v) {
        printf("%d, %d\n", file_counter, max_files);
        for (int i = 0; i < file_counter; ++i) {
            printf("File Path: %s\n", all_files[i]->path);
            printf("Last Modified: %s", ctime(&all_files[i]->lastModified));
        }
    }
    overwriteAllDirsWithAllFiles(dir_list, num_dirs, all_files, file_counter);
}
