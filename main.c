#include<stdio.h>

#include "paths.h"
#include "./Modules/user.c"

int main()
{
    int choice = -1;

    do
    {
        printf("\n====================================\n");
        printf("       VEHICLE RENTAL SYSTEM       \n");
        printf("====================================\n");
        printf("1. Admin Login\n");
        printf("2. Customer Login\n");
        printf("3. Customer Registration\n");
        printf("4. Edit Username\n");
        printf("5. Edit Password\n");
        printf("6. View Customer Profile\n");
        printf("7. Logout\n");
        printf("0. Exit\n");
        printf("====================================\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");

            while (getchar() != '\n')
            {
            }

            choice = -1;
            continue;
        }

        while (getchar() != '\n')
        {
        }

        switch (choice)
        {
            case 1:
                loginAdmin();
                break;

            case 2:
                loginCustomer();
                break;

            case 3:
                registerCustomer();
                break;

            case 4:
                editUsername();
                break;

            case 5:
                editPassword();
                break;

            case 6:
                viewProfile();
                break;

            case 7:
                logout();
                break;

            case 0:
                printf("Exiting program. Goodbye!\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 0);

    return 0;
}