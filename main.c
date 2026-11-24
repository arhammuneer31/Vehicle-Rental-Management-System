#include<stdio.h>

#include "env.h"
#include "./Modules/vehicle.c"

int main()
{
    int choice;

    while (1)
    {
        printf("\n========== VEHICLE MANAGEMENT SYSTEM ==========\n");
        printf("1. Add Vehicle\n");
        printf("2. Delete Vehicle\n");
        printf("3. View All Vehicles\n");
        printf("4. Filter Vehicles\n");
        printf("5. Update Vehicles\n");
        printf("0. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case 1:
                addVehicles();
                break;

            case 2:
                deleteVehicles();
                break;

            case 3:
                viewAllVehicles();
                break;

            case 4:
                filterVehicles();
                break;

            case 5:
                updateVehicles();
                break;

            case 0:
                printf("Exiting Vehicle Management System...\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}