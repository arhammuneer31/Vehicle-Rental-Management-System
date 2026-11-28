#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../env.h"
#include "../struct.h"
#include "../Utils/utilities.c"
#include "../Utils/Database Utils/vehicleUtils.c"
#include "../Utils/Database Utils/rentalUtils.c"


void createRentalModule()
{
    Rental rental = {0};

    char input[100];
    int vehicle_id, days;
    int existingIndex;
    char choice[10];

    rental.user_id = getCurrentUserId();

    if (rental.user_id == -1)
    {
        printf("Please login first.\n");
        return;
    }

    printf("\n========== CREATE RENTAL ==========\n");

    while (rental.vehicleCount < MAX_RENTAL_VEHICLES)
    {
        printf("\nEnter Vehicle ID (0 to finish): ");
        readLine("", input, sizeof(input));

        vehicle_id = atoi(input);

        if (vehicle_id == 0)
            break;

        if (vehicle_id < 0)
        {
            printf("Invalid vehicle ID.\n");
            continue;
        }

        // Check vehicle availability
        if (!checkVehicleAvailability(vehicle_id))
        {
            continue;
        }

        printf("Enter Number of Rental Days: ");
        readLine("", input, sizeof(input));

        days = atoi(input);

        if (days <= 0)
        {
            printf("Invalid number of days.\n");
            continue;
        }

        // Check whether vehicle already exists in rental
        existingIndex = -1;

        for (int i = 0; i < rental.vehicleCount; i++)
        {
            if (rental.vehicle_ids[i] == vehicle_id)
            {
                existingIndex = i;
                break;
            }
        }

        if (existingIndex != -1)
        {
            rental.totalDays[existingIndex] += days;

            printf("\nRental days updated for Vehicle ID %d.\n",
                   vehicle_id);
        }
        else
        {
            int index = rental.vehicleCount;

            rental.vehicle_ids[index] = vehicle_id;
            rental.totalDays[index] = days;

            rental.vehicleCount++;

            printf("\nVehicle added successfully.\n");
        }

        // Display current rental order
        printf("\n========== CURRENT RENTAL ==========\n");

        for (int i = 0; i < rental.vehicleCount; i++)
        {
            printf("Vehicle ID: %d | Days: %d\n",
                   rental.vehicle_ids[i],
                   rental.totalDays[i]);
        }

        printf("====================================\n");

        if (rental.vehicleCount == MAX_RENTAL_VEHICLES)
        {
            printf("\nMaximum vehicle limit reached.\n");
            break;
        }

        printf("\nAdd another vehicle? (yes/no): ");
        readLine("", choice, sizeof(choice));

        if (strcmp(choice, "no") == 0 ||
            strcmp(choice, "NO") == 0 ||
            strcmp(choice, "No") == 0)
        {
            break;
        }
    }

    if (rental.vehicleCount == 0)
    {
        printf("\nNo vehicles selected. Rental cancelled.\n");
        return;
    }

    // Create rental in database
    int result = createRental(rental);

    if (result == 1)
    {
        printf("\nRental completed successfully!\n");
    }
    else if (result == 0)
    {
        printf("\nRental could not be created. Please check your details.\n");
    }
    else
    {
        printf("\nAn error occurred while creating the rental.\n");
    }
}