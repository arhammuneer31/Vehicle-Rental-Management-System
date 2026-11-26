#ifndef RENTAL_UTILS_C
#define RENTAL_UTILS_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "../../env.h"
#include "../../struct.h"
#include "../../Utils/utilities.c"


// Function already defined in vehicleUtils.c
void updateVehicleAvailability(int vehicle_id);


// Get daily rate of an available vehicle
bool getVehicleDailyRate(int vehicle_id, float *dailyRate)
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicle database.\n");
        return false;
    }

    char line[512];
    Vehicle vehicle;
    int availability;

    // Skip CSV header
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line,
                   "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%d",
                   &vehicle.vehicle_id,
                   vehicle.brand,
                   vehicle.model,
                   &vehicle.year,
                   vehicle.registration_number,
                   vehicle.category,
                   vehicle.fuel_type,
                   vehicle.transmission,
                   &vehicle.daily_rate,
                   &availability) != 10)
        {
            continue;
        }

        if (vehicle.vehicle_id == vehicle_id)
        {
            fclose(file);

            if (availability == 0)
                return false;

            *dailyRate = vehicle.daily_rate;
            return true;
        }
    }

    fclose(file);
    return false;
}


// Create rental record
int createRental(Rental rental)
{
    if (rental.vehicleCount <= 0 ||
        rental.vehicleCount > MAX_RENTAL_VEHICLES)
    {
        return 0;
    }

    // Check whether user exists
    if (!recordExists(rental.user_id, USER_DATA))
    {
        printf("User does not exist.\n");
        return 0;
    }

    // Validate vehicles and calculate individual amounts
    rental.grandTotal = 0;

    for (int i = 0; i < rental.vehicleCount; i++)
    {
        if (rental.vehicle_ids[i] <= 0 ||
            rental.totalDays[i] <= 0)
        {
            return 0;
        }

        // Prevent duplicate vehicle IDs
        for (int j = 0; j < i; j++)
        {
            if (rental.vehicle_ids[i] == rental.vehicle_ids[j])
            {
                printf("Duplicate vehicle found.\n");
                return 0;
            }
        }

        float dailyRate;

        if (!getVehicleDailyRate(rental.vehicle_ids[i], &dailyRate))
        {
            printf("Vehicle %d is unavailable or does not exist.\n",
                   rental.vehicle_ids[i]);

            return 0;
        }

        rental.totalAmounts[i] =
            dailyRate * rental.totalDays[i];

        rental.grandTotal += rental.totalAmounts[i];
    }

    // Get current date
    time_t currentTime = time(NULL);
    struct tm *currentDate = localtime(&currentTime);

    if (currentDate == NULL)
        return -1;

    struct tm startingDate = *currentDate;

    strftime(rental.starting_date,
             sizeof(rental.starting_date),
             "%Y-%m-%d",
             &startingDate);

    // Calculate return dates for each vehicle
    for (int i = 0; i < rental.vehicleCount; i++)
    {
        struct tm returnDate = startingDate;

        returnDate.tm_mday += rental.totalDays[i];

        mktime(&returnDate);

        strftime(rental.returnDates[i],
                 sizeof(rental.returnDates[i]),
                 "%Y-%m-%d",
                 &returnDate);
    }

    rental.isActive = true;

    // Create rental file and header if it does not exist or is empty
    FILE *file = fopen(RENTAL_DATA, "a+");

    if (file == NULL)
    {
        printf("Error opening rental database.\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);

    if (ftell(file) == 0)
    {
        fprintf(file,
                "rental_id,user_id,starting_date,vehicleCount,"
                "vehicle_ids,totalDays,returnDates,totalAmounts,"
                "grandTotal,isActive\n");
    }

    fclose(file);

    // Generate rental ID
    rental.rental_id = nextId(RENTAL_DATA);

    // Open file to insert rental
    file = fopen(RENTAL_DATA, "a");

    if (file == NULL)
    {
        printf("Error opening rental database.\n");
        return -1;
    }

    // Insert basic rental information
    fprintf(file, "%d,%d,%s,%d,",
            rental.rental_id,
            rental.user_id,
            rental.starting_date,
            rental.vehicleCount);

    // Insert vehicle IDs
    for (int i = 0; i < rental.vehicleCount; i++)
    {
        fprintf(file, "%s%d",
                i == 0 ? "" : ";",
                rental.vehicle_ids[i]);
    }

    fprintf(file, ",");

    // Insert rental days
    for (int i = 0; i < rental.vehicleCount; i++)
    {
        fprintf(file, "%s%d",
                i == 0 ? "" : ";",
                rental.totalDays[i]);
    }

    fprintf(file, ",");

    // Insert return dates
    for (int i = 0; i < rental.vehicleCount; i++)
    {
        fprintf(file, "%s%s",
                i == 0 ? "" : ";",
                rental.returnDates[i]);
    }

    fprintf(file, ",");

    // Insert individual amounts
    for (int i = 0; i < rental.vehicleCount; i++)
    {
        fprintf(file, "%s%.2f",
                i == 0 ? "" : ";",
                rental.totalAmounts[i]);
    }

    // Insert grand total and rental status
    fprintf(file, ",%.2f,%d\n",
            rental.grandTotal,
            rental.isActive);

    fclose(file);

    // Mark all rented vehicles as unavailable
    for (int i = 0; i < rental.vehicleCount; i++)
    {
        updateVehicleAvailability(rental.vehicle_ids[i]);
    }

    printf("\nRental ID: %d\n", rental.rental_id);
    printf("Starting Date: %s\n", rental.starting_date);

    for (int i = 0; i < rental.vehicleCount; i++)
    {
        printf("\nVehicle ID: %d\n", rental.vehicle_ids[i]);
        printf("Rental Days: %d\n", rental.totalDays[i]);
        printf("Return Date: %s\n", rental.returnDates[i]);
        printf("Amount: %.2f\n", rental.totalAmounts[i]);
    }

    printf("\nGrand Total: %.2f\n", rental.grandTotal);

    return 1;
}

#endif