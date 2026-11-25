#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../env.h"
#include "../struct.h"
#include "../Utils/Database Utils/userUtils.c"
#include "../Utils/Database Utils/vehicleUtils.c"

// Add a vehicle
void addVehiclesAdmin()
{
    Vehicle vehicle;
    char input[100];

    readLine("Enter Brand: ", vehicle.brand, sizeof(vehicle.brand));
    readLine("Enter Model: ", vehicle.model, sizeof(vehicle.model));

    readLine("Enter Year: ", input, sizeof(input));
    vehicle.year = atoi(input);

    readLine("Enter Registration Number: ", vehicle.registration_number, sizeof(vehicle.registration_number));
    readLine("Enter Category: ", vehicle.category, sizeof(vehicle.category));
    readLine("Enter Fuel Type: ", vehicle.fuel_type, sizeof(vehicle.fuel_type));
    readLine("Enter Transmission: ", vehicle.transmission, sizeof(vehicle.transmission));

    readLine("Enter Daily Rate: ", input, sizeof(input));
    vehicle.daily_rate = atof(input);

    if (strlen(vehicle.brand) == 0 ||
        strlen(vehicle.model) == 0 ||
        strlen(vehicle.registration_number) == 0 ||
        strlen(vehicle.category) == 0 ||
        strlen(vehicle.fuel_type) == 0 ||
        strlen(vehicle.transmission) == 0 ||
        vehicle.year <= 0 ||
        vehicle.daily_rate <= 0)
    {
        printf("Invalid vehicle details. Please try again.\n");
        return;
    }

    addVehicles(vehicle);
}

// Delete a vehicle
void deleteVehiclesAdmin()
{
    char input[20];

    readLine("Enter Vehicle ID: ", input, sizeof(input));

    int vehicle_id = atoi(input);

    if (vehicle_id <= 0)
    {
        printf("Invalid Vehicle ID.\n");
        return;
    }

    deleteVehicles(vehicle_id);
}

// Vehicle filter criteria menu for admin
void filterVehiclesGeneric()
{
    int choice;
    char input[20];
    char filterField[30];
    char filterValue[100];

    bool isUserAdmin = isCurrentUserAdmin();

    Vehicle results[MAX_VEHICLES];

    while (1)
    {
        printf("\n========== SEARCH VEHICLES ==========\n");
        printf("1. Brand\n");
        printf("2. Model\n");
        printf("3. Year\n");
        printf("4. Registration Number\n");
        printf("5. Category\n");
        printf("6. Fuel Type\n");
        printf("7. Transmission\n");
        printf("8. Daily Rate\n");

        if (isUserAdmin) {
            printf("9. Availability\n");
        }

        printf("0. Back\n");

        readLine("Select filter: ", input, sizeof(input));

        choice = atoi(input);

        if (choice == 0)
            return;

        switch (choice)
        {
            case 1:
                strcpy(filterField, "brand");
                break;

            case 2:
                strcpy(filterField, "model");
                break;

            case 3:
                strcpy(filterField, "year");
                break;

            case 4:
                strcpy(filterField, "registration_number");
                break;

            case 5:
                strcpy(filterField, "category");
                break;

            case 6:
                strcpy(filterField, "fuel_type");
                break;

            case 7:
                strcpy(filterField, "transmission");
                break;

            case 8:
                strcpy(filterField, "daily_rate");
                break;

            case 9:
                strcpy(filterField, "isAvailable");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
                continue;
        }

        readLine("Enter filter value: ", filterValue, sizeof(filterValue));

        if (filterValue[0] == '\0')
        {
            printf("Filter value cannot be empty.\n");
            continue;
        }

        if (choice == 9 && strcmp(filterValue, "available") != 0 && strcmp(filterValue, "unavailable") != 0)
        {
            printf("Enter either 'available' or 'unavailable'.\n");
            continue;
        }

        int count = filterVehicles(filterField, filterValue, results, MAX_VEHICLES);

        if (!isUserAdmin) {
            count = extractAvailableVehicles(results, count);
        }

        if (count == -1)
        {
            printf("Unable to search vehicles.\n");
            continue;
        }

        if (count == 0)
        {
            printf("No matching vehicles found.\n");
            continue;
        }

        displayFilteredVehicles(results, count);
    }
}

// View all the vehicles in the database
void viewVehiclesGeneric()
{
    viewVehicles();
}

// Update any field of the vehicle
void updateVehiclesAdmin()
{
    char input[100];
    char field[30];
    char newValue[100];

    int vehicle_id;
    int choice;

    printf("\n========== UPDATE VEHICLE ==========\n");

    readLine("Enter Vehicle ID: ", input, sizeof(input));

    vehicle_id = atoi(input);

    if (vehicle_id <= 0)
    {
        printf("Invalid Vehicle ID.\n");
        return;
    }

    printf("\nSelect the field you want to update:\n");

    printf("1. Brand\n");
    printf("2. Model\n");
    printf("3. Year\n");
    printf("4. Registration Number\n");
    printf("5. Category\n");
    printf("6. Fuel Type\n");
    printf("7. Transmission\n");
    printf("8. Daily Rate\n");
    printf("9. Availability\n");

    readLine("Enter your choice: ", input, sizeof(input));

    choice = atoi(input);

    switch (choice)
    {
        case 1:
            strcpy(field, "brand");
            break;

        case 2:
            strcpy(field, "model");
            break;

        case 3:
            strcpy(field, "year");
            break;

        case 4:
            strcpy(field, "registration_number");
            break;

        case 5:
            strcpy(field, "category");
            break;

        case 6:
            strcpy(field, "fuel_type");
            break;

        case 7:
            strcpy(field, "transmission");
            break;

        case 8:
            strcpy(field, "daily_rate");
            break;

        case 9:
            strcpy(field, "isAvailable");
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    readLine("Enter updated value: ", newValue, sizeof(newValue));

    if (newValue[0] == '\0')
    {
        printf("Updated value cannot be empty.\n");
        return;
    }

    if (choice == 3 && atoi(newValue) <= 0)
    {
        printf("Invalid year.\n");
        return;
    }

    if (choice == 8 && atof(newValue) <= 0)
    {
        printf("Invalid daily rate.\n");
        return;
    }

    if (choice == 9 &&
        strcmp(newValue, "available") != 0 &&
        strcmp(newValue, "unavailable") != 0)
    {
        printf("Enter either 'available' or 'unavailable'.\n");
        return;
    }

    int result = updateVehicles(vehicle_id, field, newValue);

    if (result == 1)
        printf("Vehicle updated successfully.\n");

    else if (result == 0)
        printf("Vehicle ID not found.\n");

    else
        printf("Unable to update vehicle.\n");
}

// View available vehicles
void viewAvailableVehicles()
{
    char filterField[30];
    char filterValue[100];

    Vehicle results[MAX_VEHICLES];

    strcpy(filterField, "isAvailable");
    strcpy(filterValue, "available");

    int count = filterVehicles(filterField, filterValue, results, MAX_VEHICLES);

    if (count == -1)
    {
        printf("Unable to search vehicles.\n");
    }

    if (count == 0)
    {
        printf("No matching vehicles found.\n");
    }

    displayFilteredVehicles(results, count);
}