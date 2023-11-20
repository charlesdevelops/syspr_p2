// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/param.h>

// #define MAX_DIRECTORIES 10
// #define MAX_FILES 100

// // Structure to represent a file
// struct File
// {
//     char filename[MAXPATHLEN];
//     int size;
// };

// // Structure to represent a directory
// struct Directory
// {
//     char dirname[MAXPATHLEN];
//     struct File *files;
//     int file_count;
// };

// // Structure to represent a file appearing in multiple directories
// struct MultiDirectoryFile
// {
//     char filename[MAXPATHLEN];
//     int size[MAX_DIRECTORIES];
//     char directories[MAX_DIRECTORIES][MAXPATHLEN];
//     int num_directories;
// };

// /**
//  * @brief Prints out a list of files and their data
//  *
//  * @param files The files to print
//  * @param num_files The number of files in the given array
//  */
// void printFileList(struct MultiDirectoryFile *files, int num_files)
// {
//     for (int i = 0; i < num_files; i++)
//     {
//         printf("File: %s - Sizes:", files[i].filename);
//         for (int j = 0; j < files[i].num_directories; j++)
//         {
//             printf(" %d", files[i].size[j]);
//         }
//         printf(" - Directories: ");
//         for (int j = 0; j < files[i].num_directories; j++)
//         {
//             printf("%s ", files[i].directories[j]);
//         }
//         printf("\n");
//     }
// }

// /**
//  * @brief Compare multiple directories and create a list of unique files across them.
//  *
//  * This function iterates through an array of directories and their files, identifying unique files
//  * and storing information about them in a result array. It also tracks which directories contain each file.
//  *
//  * @param directories An array of Directory structures representing the input directories.
//  * @param num_directories The number of directories in the 'directories' array.
//  * @param result_files An array of MultiDirectoryFile structures to store unique file information.
//  * @param num_result_files A pointer to an integer to track the number of unique files in 'result_files.'
//  */
// void compareMultipleDirectories(struct Directory *directories, int num_directories, struct MultiDirectoryFile *result_files, int *num_result_files)
// {
//     for (int i = 0; i < num_directories; i++)
//     {
//         for (int j = 0; j < directories[i].file_count; j++)
//         {
//             char currentFile[MAXPATHLEN];
//             strcpy(currentFile, directories[i].files[j].filename);

//             // Check if the file is already in the result array
//             int existingIndex = -1;
//             for (int k = 0; k < *num_result_files; k++)
//             {
//                 if (strcmp(result_files[k].filename, currentFile) == 0)
//                 {
//                     existingIndex = k;
//                     break;
//                 }
//             }

//             // If the file is not in the result array, add it
//             // If the file is not in the result array, add it
//             if (existingIndex == -1)
//             {
//                 strcpy(result_files[*num_result_files].filename, currentFile);
//                 result_files[*num_result_files].num_directories = 1;
//                 for (int k = 0; k < MAX_DIRECTORIES; k++)
//                 {
//                     result_files[*num_result_files].size[k] = -1; // Initialize to a default value
//                 }

//                 result_files[*num_result_files].size[0] = directories[i].files[j].size;
//                 strcpy(result_files[*num_result_files].directories[0], directories[i].dirname);
//                 (*num_result_files)++;
//             }
//             else
//             {
//                 // File already exists in the result array, add the current directory to its list
//                 int currentDirectoryIndex = result_files[existingIndex].num_directories;
//                 if (result_files[existingIndex].size[currentDirectoryIndex] != directories[i].files[j].size)
//                 {
//                     // Add the new size to the array
//                     result_files[existingIndex].size[currentDirectoryIndex] = directories[i].files[j].size;
//                 }
//                 strcpy(result_files[existingIndex].directories[currentDirectoryIndex], directories[i].dirname);
//                 result_files[existingIndex].num_directories++;
//             }
//         }
//     }
// }