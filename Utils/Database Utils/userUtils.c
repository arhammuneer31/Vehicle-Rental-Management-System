#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../Utils/utilities.c"

#define USER_DATA "../../Database/user.csv"
#define TEMP_DATA "../../Database/temp.csv"

typedef struct
{
    int user_id;
    char username[50];
    char email[100];
    char password[50];
    bool isAdmin;
} User;

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

    if (!checkEmailStructure(user.email))
    {
        printf("Invalid email address entered\n");
        return;
    }

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