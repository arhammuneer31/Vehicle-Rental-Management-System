#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../env.h"
#include "../../struct.h"
#include "../../Utils/utilities.c"

// Add a row of user details to file. The id is taken from the file, not the caller.
void addUser(User user)
{
    FILE *file = fopen(USER_DATA, "a");

    if (file == NULL)
    {
        printf("Error opening user.csv\n");
        return;
    }

    user.user_id = nextId(USER_DATA);

    fprintf(file, "%d,%s,%s,%s,%d\n",
            user.user_id,
            user.username,
            user.email,
            user.password,
            user.isAdmin);

    fclose(file);

    printf("User added successfully. ID: %d\n", user.user_id);
}

// Delete a user using user_id through temp file
void deleteUser(int user_id)
{
    FILE *file = fopen(USER_DATA, "r");

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
    int current_id;

    if (fgets(line, sizeof(line), file) != NULL)
    {
        fputs(line, temp);
    }

    // Copy all users except the one being deleted
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,", &current_id) == 1 && current_id == user_id)
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

// Update username using user_id
void updateUsername(int user_id, const char newUsername[])
{
    FILE *file = fopen(USER_DATA, "r");

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
    int current_id;
    int found = 0;
    char username[50], email[100], password[50];
    int isAdmin;

    if (fgets(line, sizeof(line), file) != NULL)
    {
        fputs(line, temp);
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d",
                   &current_id, username, email, password, &isAdmin) != 5)
        {
            fputs(line, temp);
            continue;
        }

        if (current_id == user_id)
        {
            found = 1;
            fprintf(temp, "%d,%s,%s,%s,%d\n",
                    current_id, newUsername, email, password, isAdmin);
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

// Update password using user_id
void updatePassword(int user_id, const char newPassword[])
{
    FILE *file = fopen(USER_DATA, "r");

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
    int current_id;
    int found = 0;
    char username[50], email[100], password[50];
    int isAdmin;

    if (fgets(line, sizeof(line), file) != NULL)
    {
        fputs(line, temp);
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d",
                   &current_id, username, email, password, &isAdmin) != 5)
        {
            fputs(line, temp);
            continue;
        }

        if (current_id == user_id)
        {
            found = 1;
            fprintf(temp, "%d,%s,%s,%s,%d\n",
                    current_id, username, email, newPassword, isAdmin);
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

// Checks if the current email is already registered with an exisiting user
bool emailExists(const char *email)
{
    FILE *file = fopen(USER_DATA, "r");
    char line[512];
    char storedEmail[100];

    if (file == NULL)
    {
        return false;
    }

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

// Save logged in user's details
bool saveCurrentUser(int user_id)
{
    FILE *file = fopen(CURRENT_USER, "w");

    if (file == NULL)
    {
        printf("Error saving current user.\n");
        return false;
    }

    fprintf(file, "%d\n", user_id);
    fclose(file);

    return true;
}
