#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define USERS_FILE "../../Database/users.csv"
#define TEMP_FILE "../../Database/temp.csv"
#include "../../Utils/utilities.c"

typedef struct
{
    int user_id;
    char username[50];
    char password[50];
    bool isAdmin;
} User;


// Add a row of user details to file
void addUser(User user)
{
    FILE *file = fopen(USERS_FILE, "a");

    if (file == NULL)
    {
        printf("Error opening users.csv\n");
        return;
    }

    if (recordExists(user.user_id, USERS_FILE))
    {
        printf("User with this ID already exists.\n");
        return;
    }

    fprintf(file, "%d,%s,%s,%d\n",
            user.user_id,
            user.username,
            user.password,
            user.isAdmin);

    fclose(file);

    printf("User added successfully.\n");
}


// Delete a user using user_id through temp file
void deleteUser(int user_id)
{
    FILE *file = fopen(USERS_FILE, "r");

    if (file == NULL)
    {
        printf("Error opening users.csv\n");
        return;
    }

    FILE *temp = fopen(TEMP_FILE, "w");

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
        remove(USERS_FILE);
        rename(TEMP_FILE, USERS_FILE);
        printf("User deleted successfully.\n");
    }
    else
    {
        remove(TEMP_FILE);
        printf("User not found.\n");
    }
}