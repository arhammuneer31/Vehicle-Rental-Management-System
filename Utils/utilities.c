#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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

// Generates the next id
int nextId(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    char line[256];
    int last_id = 0;
    int current_id;

    if (file == NULL)
    {
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,", &current_id) == 1)
        {
            last_id = current_id;
        }
    }

    fclose(file);
    return last_id + 1;
}

// Takes string inputs
void readLine(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
}