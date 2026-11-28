#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../env.h"
#include "../struct.c"
#include "../Helper/userHelper.c"

// Login for customer
bool loginCustomer()
{
    char email[100];
    char password[50];
    char line[512];
    char username[50];
    char storedEmail[100];
    char storedPassword[50];
    int userId;
    int isAdmin;

    readLine("Email: ", email, sizeof(email));
    readLine("Password: ", password, sizeof(password));

    FILE *file = openFile(USER_DATA);

    if (file == NULL)
    {
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d", &userId, username, storedEmail, storedPassword, &isAdmin) != 5)
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

        if (!saveCurrentUser(userId))
        {
            return false;
        }

        printf("Customer Login successful.\n");
        return true;
    }

    fclose(file);
    printf("Invalid email or password.\n");
    return false;
}

// Login for admin
bool loginAdmin()
{
    char email[100];
    char password[50];
    char line[512];
    char username[50];
    char storedEmail[100];
    char storedPassword[50];
    int userId;
    int isAdmin;

    readLine("Email: ", email, sizeof(email));
    readLine("Password: ", password, sizeof(password));

    FILE *file = openFile(USER_DATA);

    if (file == NULL)
    {
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d", &userId, username, storedEmail, storedPassword, &isAdmin) != 5)
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

        if (!saveCurrentUser(userId))
        {
            return false;
        }

        printf("Admin Login successful.\n");
        return true;
    }

    fclose(file);
    printf("Invalid email or password.\n");
    return false;
}

// Register a new customer and automatically log them in
void registerCustomer()
{
    User user;

    user.userId = 0;
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
        printf("Invalid email. Please retry.\n");
        return;
    }

    if (emailExists(user.email))
    {
        printf("An account with this email already exists.\n");
        return;
    }

    if (strlen(user.password) < 8)
    {
        printf("Password must be at least 8 characters long.\n");
        return;
    }

    formatUsername(user.username);
    user.userId = nextId(USER_DATA);

    addUser(user);

    if (saveCurrentUser(user.userId))
    {
        printf("Registration successful. You are now logged in.\n");
    }
}

// Update profile options (username)
void editUsername()
{
    char newUsername[50];

    readLine("New Username: ", newUsername, sizeof(newUsername));

    if (newUsername[0] == '\0')
    {
        printf("Username cannot be empty.\n");
        return;
    }

    updateUsername(newUsername);

    printf("Username updated successfully.\n");
}

// Update profile options (password)
void editPassword()
{
    char newPassword[50];
    char confirmPassword[50];

    readLine("New Password: ", newPassword, sizeof(newPassword));
    readLine("Retype New Password: ", confirmPassword, sizeof(confirmPassword));

    if (newPassword[0] == '\0' || confirmPassword[0] == '\0')
    {
        printf("Password cannot be empty.\n");
        return;
    }

    if (strlen(newPassword) < 8 || strlen(confirmPassword) < 8)
    {
        printf("Password must be at least 8 characters long.\n");
        return;
    }

    if (strcmp(newPassword, confirmPassword) != 0)
    {
        printf("Passwords do not match. Please try again.\n");
        return;
    }

    updatePassword(newPassword);

    printf("Password updated successfully.\n");
}

// View user profile
void viewProfile()
{
    int currentId = getCurrentUserId();

    if (currentId == -1)
    {
        printf("No user is currently logged in.\n");
        return;
    }

    FILE *file = openFile(USER_DATA);

    if (file == NULL)
        return;

    char line[512];
    char username[50];
    char email[100];
    char password[50];
    int userId, isAdmin;
    bool found = false;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%99[^,],%49[^,],%d", &userId, username, email, password, &isAdmin) != 5)
            continue;

        if (userId == currentId)
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
    printf("  User ID       : %-25d\n", userId);
    printf("  Username      : %-25s\n", username);
    printf("  Email         : %-25s\n", email);
    printf("  Account Type  : %-25s\n", isAdmin ? "Administrator" : "Customer");
    printf("============================================\n\n");
}

// Logout current user
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