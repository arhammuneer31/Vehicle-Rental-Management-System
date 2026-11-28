#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../env.h"
#include "../../struct.c"
#include "./utilities.c"

// Add a user to the csv file
void addUser(User user)
{
    user.userId = nextId(USER_DATA);

    FILE *file = fopen(USER_DATA, "a");

    if (file == NULL)
    {
        printf("Error opening user.csv\n");
        return;
    }

    fprintf(file, "%d,%s,%s,%s,%d\n",
            user.userId,
            user.username,
            user.email,
            user.password,
            user.isAdmin);

    fclose(file);

    printf("User added successfully. ID: %d\n", user.userId);
}

// Delete a user using userId through temp file
void deleteUser(int userId)
{
    // Open user.csv in read mode and skip first line (header)
    FILE *file = openFile(USER_DATA);

    if (file == NULL)
    {
        printf("Error opening user.csv\n");
        return;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    char line[512];
    int found = 0;
    int currentId;

    // Write CSV header to temporary file
    fputs("userId,username,email,password,isAdmin\n", temp);

    // Copy all users to the temporary file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Skip copying if the user to be deleted is found
        if (sscanf(line, "%d,", &currentId) == 1 && currentId == userId)
        {
            found = 1;
            continue;
        }

        fputs(line, temp);
    }

    fclose(file);
    fclose(temp);

    if (found)
    {
        remove(USER_DATA);
        rename(TEMP_DATA, USER_DATA);
        printf("User deleted successfully.\n");
    }
    else
    {
        remove(TEMP_DATA);
        printf("User not found.\n");
    }
}

// Update username using userId
void updateUsername(const char newUsername[])
{
    FILE *file = openFile(USER_DATA);

    if (file == NULL)
    {
        return;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    char line[512];
    int currentId, userId = getCurrentUserId();
    int found = 0;
    char username[50], email[100], password[50];
    int isAdmin;

    fputs("userId,username,email,password,isAdmin\n", temp);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d", &currentId, username, email, password, &isAdmin) != 5)
        {
            fputs(line, temp);
            continue;
        }

        if (currentId == userId)
        {
            found = 1;
            fprintf(temp, "%d,%s,%s,%s,%d\n", currentId, newUsername, email, password, isAdmin);
        }
        else
        {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);

    if (found)
    {
        remove(USER_DATA);
        rename(TEMP_DATA, USER_DATA);
        printf("Username updated successfully.\n");
    }
    else
    {
        remove(TEMP_DATA);
        printf("User not found.\n");
    }
}

// Update password using userId
void updatePassword(const char newPassword[])
{
    FILE *file = openFile(USER_DATA);

    if (file == NULL)
    {
        return;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    char line[512];
    int currentId, userId = getCurrentUserId();
    int found = 0;
    char username[50], email[100], password[50];
    int isAdmin;

    fputs("userId,username,email,password,isAdmin\n", temp);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d", &currentId, username, email, password, &isAdmin) != 5)
        {
            fputs(line, temp);
            continue;
        }

        if (currentId == userId)
        {
            found = 1;
            fprintf(temp, "%d,%s,%s,%s,%d\n", currentId, username, email, newPassword, isAdmin);
        }
        else
        {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);

    if (found)
    {
        remove(USER_DATA);
        rename(TEMP_DATA, USER_DATA);
        printf("Password updated successfully.\n");
    }
    else
    {
        remove(TEMP_DATA);
        printf("User not found.\n");
    }
}

// Checks the structure of the email entered
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
        {
            return false;
        }

        if (email[i] == '@')
        {
            atCount++;
            atPosition = i;
        }
    }

    if (atCount != 1)
    {
        return false;
    }

    if (atPosition <= 0 || atPosition == length - 1)
    {
        return false;
    }

    for (int i = atPosition + 1; i < length; i++)
    {
        if (email[i] == '.')
        {
            dotPosition = i;
        }
    }

    if (dotPosition == -1)
    {
        return false;
    }

    if (dotPosition == atPosition + 1 || dotPosition == length - 1)
    {
        return false;
    }

    return true;
}

// Checks if the email is already registered with an existing user
bool emailExists(const char *email)
{
    FILE *file = openFile(USER_DATA);

    if (file == NULL)
    {
        return false;
    }

    char line[512];
    char storedEmail[100];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%*d,%*[^,],%99[^,]", storedEmail) == 1 && strcmp(storedEmail, email) == 0)
        {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

// Save logged in user's ID
bool saveCurrentUser(int userId)
{
    FILE *file = fopen(CURRENT_USER, "w");

    if (file == NULL)
    {
        printf("Error saving current user.\n");
        return false;
    }

    fprintf(file, "%d\n", userId);
    fclose(file);

    return true;
}