#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Check if a record with given id exists in the file
bool recordExists(int id, char* fileName) {
    FILE* file = fopen(fileName, "r");

    if (file == NULL) {
        printf("Error opening file: %s\n", fileName);
        return false;
    }

    char line[100];
    int current_id;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d,", &current_id) == 1 && current_id == id) {
            fclose(file);
            return true;
        }
    }

    fclose(file);

    return false;
}