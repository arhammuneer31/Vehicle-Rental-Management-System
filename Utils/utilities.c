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

// Checks the sturcture of the email entered
bool checkEmailStructure(const char email[])
{
    int atCount = 0;
    int atPosition = -1;
    int dotPosition = -1;
    int length = strlen(email);

    if (length == 0) 
    {
        return false;
    }

    for (int i = 0; i < length; i++)
    {
        if (isspace((unsigned char)email[i]))
            return false;

        if (email[i] == '@')
        {
            atCount++;
            atPosition = i;
        }
    }

    if (atCount != 1)
        return false;

    if (atPosition <= 0 || atPosition == length - 1)
        return false;

    for (int i = atPosition + 1; i < length; i++)
    {
        if (email[i] == '.')
            dotPosition = i;
    }

    if (dotPosition == -1)
        return false;

    if (dotPosition == atPosition + 1 || dotPosition == length - 1)
        return false;

    return true;
}