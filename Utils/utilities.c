#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../env.h"

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

// Retrieve current user
int getCurrentUserId()
{
    FILE *file = fopen(CURRENT_USER, "r");
    int user_id;

    if (file == NULL)
        return -1;

    if (fscanf(file, "%d", &user_id) != 1)
    {
        fclose(file);
        return -1;
    }

    fclose(file);
    return user_id;
}

// Check whether current user is admin or not
bool isCurrentUserAdmin()
{
    int currentUserId = getCurrentUserId();

    if (currentUserId == -1) {
        return false;
    }

    FILE *file = fopen(USER_DATA, "r");
    bool isUserAdmin, found = false;

    if (file == NULL)
    {
        return false;
    }

    char line[512];
    char username[50];
    char email[100];
    char password[50];
    int isAdmin, user_id;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d",
                   &user_id, username, email, password, &isAdmin) != 5)
            continue;

        if (user_id == currentUserId)
        {
            found = true;
            break;
        }
    }

    fclose(file);

    if (!found)
    {
        printf("User record not found.\n");
        return false;
    }

    isUserAdmin = isAdmin == 0 ? false : true;

    return isUserAdmin;
}
