#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VEHICLES_FILE "../../Database/vehicle.csv"
#define TEMP_FILE "../../Database/temp.csv"
#include "../../Utils/utilities.c"

typedef struct
{
    int vehicle_id;
    char brand[50];
    char model[50];
    int year;
    char registration_number[50];
    char category[50];
    char fuel_type[50];
    char transmission[50];
    float daily_rate;
    bool isAvailable;
} Vehicle;

// Add a row of user details to file
void addVehicle(Vehicle vehicle)
{
    FILE *file = fopen(VEHICLES_FILE, "a");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return;
    }

    if (recordExists(vehicle.vehicle_id, VEHICLES_FILE))
    {
        printf("Vehicle with this ID already exists.\n");
        return;
    }

    fprintf(file, "%d,%s,%s,%d,%s,%s,%s,%s,%f,%d\n",
            vehicle.vehicle_id,
            vehicle.brand,
            vehicle.model,
            vehicle.year,
            vehicle.registration_number,
            vehicle.category,
            vehicle.fuel_type,
            vehicle.transmission,
            vehicle.daily_rate,
            vehicle.isAvailable);

    fclose(file);

    printf("Vehicle added successfully.\n");
}

// Delete a user using user_id through temp file
void deleteVehicle(int vehicle_id)
{
    FILE *file = fopen(VEHICLES_FILE, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
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
        if (sscanf(line, "%d,", &current_id) == 1 && current_id == vehicle_id)
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
        remove(VEHICLES_FILE);
        rename(TEMP_FILE, VEHICLES_FILE);
        printf("Vehicle deleted successfully.\n");
    }
    else
    {
        remove(TEMP_FILE);
        printf("Vehicle not found.\n");
    }
}