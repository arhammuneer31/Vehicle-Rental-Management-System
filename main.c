#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "./env.h"
#include "./struct.h"
#include "./Modules/rental.c"


int main()
{
    int choice;

    while (1)
    {
        printf("\n========== VEHICLE RENTAL SYSTEM ==========\n");
        printf("1. Create Rental\n");
        printf("2. View Vehicles\n");
        printf("3. Exit\n");
        printf("===========================================\n");

        printf("Enter Choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");

            while (getchar() != '\n');
            continue;
        }

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                createRentalModule();
                break;

            case 2:
                viewVehicles();
                break;

            case 3:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}