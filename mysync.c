#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    struct regexList *regex_list = calloc(1, sizeof(struct regexList));
    int index = argPassing(argc, argv, regex_list);
    struct Directory *dir_list = directoryInfo(argc, argv, index);
    int num_dirs = argc - index;

    puts("printing all regexes");
    for (int i = 0; i < regex_list->include_count; ++i) {
        printf("Include Pattern: %s\n", regex_list->include_patterns[i].regex);
    }
    for (int i = 0; i < regex_list->exclude_count; ++i) {
        printf("Exclude Pattern: %s\n", regex_list->exclude_patterns[i].regex);
    }
    puts("-----------------------------------");

    synchronizeDirectories(dir_list, num_dirs, regex_list);

    for (int x = 0; x < num_dirs; ++x) {
        free(dir_list[x].files);
    }
    free(dir_list);
    free(regex_list->include_patterns);
    free(regex_list->exclude_patterns);
    free(regex_list);
    exit(EXIT_SUCCESS);
}
