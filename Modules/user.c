#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../env.h"
#include "../struct.h"
#include "../Utils/Database Utils/userUtils.c"

bool loginCustomer()
{
    char email[100];
    char password[50];
    char line[512];
    char username[50];
    char storedEmail[100];
    char storedPassword[50];
    int user_id;
    int isAdmin;
    FILE *file;

    readLine("Email: ", email, sizeof(email));
    readLine("Password: ", password, sizeof(password));

    file = fopen(USER_DATA, "r");
    if (file == NULL)
    {
        printf("Error opening user.csv\n");
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d",
                   &user_id, username, storedEmail, storedPassword, &isAdmin) != 5)
        {
            continue;
        }

        if (strcmp(storedEmail, email) != 0 || strcmp(storedPassword, password) != 0)
        {
            continue;
        }

        if (isAdmin)
        {
            continue;
        }

        fclose(file);

        if (!saveCurrentUser(user_id))
        {
            return false;
        }

        printf("Login successful.\n");
        return true;
    }

    fclose(file);
    printf("Invalid email or password.\n");
    return false;
}

bool loginAdmin()
{
    char email[100];
    char password[50];
    char line[512];
    char username[50];
    char storedEmail[100];
    char storedPassword[50];
    int user_id;
    int isAdmin;
    FILE *file;

    readLine("Email: ", email, sizeof(email));
    readLine("Password: ", password, sizeof(password));

    file = fopen(USER_DATA, "r");
    if (file == NULL)
    {
        printf("Error opening user.csv\n");
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d",
                   &user_id, username, storedEmail, storedPassword, &isAdmin) != 5)
        {
            continue;
        }

        if (strcmp(storedEmail, email) != 0 || strcmp(storedPassword, password) != 0)
        {
            continue;
        }

        if (!isAdmin)
        {
            continue;
        }

        fclose(file);

        if (!saveCurrentUser(user_id))
        {
            return false;
        }

        printf("Login successful.\n");
        return true;
    }

    fclose(file);
    printf("Invalid email or password.\n");
    return false;
}

void registerCustomer()
{
    User user;

    user.user_id = 0;
    user.isAdmin = false;

    readLine("Username: ", user.username, sizeof(user.username));
    readLine("Email: ", user.email, sizeof(user.email));
    readLine("Password: ", user.password, sizeof(user.password));

    if (user.username[0] == '\0' || user.email[0] == '\0' || user.password[0] == '\0')
    {
        printf("Username, email, and password are required.\n");
        return;
    }

    if (!checkEmailStructure(user.email))
    {
        printf("Invalid email.\n");
        return;
    }

    if (emailExists(user.email))
    {
        printf("An account with this email already exists.\n");
        return;
    }

    addUser(user);
}

void editUsername()
{
    int user_id = getCurrentUserId();
    char newUsername[50];

    if (user_id == -1)
    {
        return;
    }

    readLine("New Username: ", newUsername, sizeof(newUsername));

    if (newUsername[0] == '\0')
    {
        printf("Username cannot be empty.\n");
        return;
    }

    updateUsername(user_id, newUsername);

    printf("Username updated successfully.\n");
}

void editPassword()
{
    int user_id = getCurrentUserId();
    char newPassword[50];
    char confirmPassword[50];

    if (user_id == -1)
        return;

    readLine("New Password: ", newPassword, sizeof(newPassword));
    readLine("Retype New Password: ", confirmPassword, sizeof(confirmPassword));

    if (newPassword[0] == '\0')
    {
        printf("Password cannot be empty.\n");
        return;
    }

    if (strcmp(newPassword, confirmPassword) != 0)
    {
        printf("Passwords do not match. Please try again.\n");
        return;
    }

    updatePassword(user_id, newPassword);

    printf("Password updated successfully.\n");
}

void viewProfile()
{
    int current_id = getCurrentUserId();

    if (current_id == -1)
    {
        printf("No user is currently logged in.\n");
        return;
    }

    FILE *file = fopen(USER_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening user.csv\n");
        return;
    }

    char line[512];
    char username[50];
    char email[100];
    char password[50];
    int user_id, isAdmin;
    bool found = false;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d",
                   &user_id, username, email, password, &isAdmin) != 5)
            continue;

        if (user_id == current_id)
        {
            found = true;
            break;
        }
    }

    fclose(file);

    if (!found)
    {
        printf("User record not found.\n");
        return;
    }

    printf("\n============================================\n");
    printf("               MY PROFILE                  \n");
    printf("============================================\n");
    printf("  User ID       : %-25d\n", user_id);
    printf("  Username      : %-25s\n", username);
    printf("  Email         : %-25s\n", email);
    printf("  Account Type  : %-25s\n", isAdmin ? "Administrator" : "Customer");
    printf("============================================\n\n");
}

void logout()
{
    FILE *file = fopen(CURRENT_USER, "w");

    if (file == NULL)
    {
        printf("Error opening currentUser.txt\n");
        return;
    }

    fclose(file);
    printf("Logged out successfully.\n");
}