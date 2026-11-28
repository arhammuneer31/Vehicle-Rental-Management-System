#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "../env.h"
#include "../struct.c"
#include "../Helper/vehicleHelper.c"

// Add a vehicle
void addVehicle()
{
    Vehicle vehicle;
    char input[100];

    readLine("Enter Brand: ", vehicle.brand, sizeof(vehicle.brand));
    readLine("Enter Model: ", vehicle.model, sizeof(vehicle.model));

    readLine("Enter Year: ", input, sizeof(input));
    vehicle.year = atoi(input);

    readLine("Enter Registration Number: ", vehicle.registrationNumber, sizeof(vehicle.registrationNumber));
    readLine("Enter Category: ", vehicle.category, sizeof(vehicle.category));
    readLine("Enter Fuel Type: ", vehicle.fuelType, sizeof(vehicle.fuelType));
    readLine("Enter Transmission: ", vehicle.transmission, sizeof(vehicle.transmission));

    readLine("Enter Daily Rate: ", input, sizeof(input));
    vehicle.dailyRate = atof(input);

    readLine("Enter Daily Late Fee: ", input, sizeof(input));
    vehicle.dailyLateFee = atof(input);

    // Validate vehicle details
    if (strlen(vehicle.brand) == 0 ||
        strlen(vehicle.model) == 0 ||
        !isAlphanumeric(vehicle.registrationNumber) ||
        strlen(vehicle.category) == 0 ||
        strlen(vehicle.fuelType) == 0 ||
        strlen(vehicle.transmission) == 0 ||
        vehicle.year < 1900 || vehicle.year > 2026 ||
        vehicle.dailyRate <= 0 ||
        vehicle.dailyLateFee <= 0)
    {
        printf("Invalid vehicle details. Please try again.\n");
        return;
    }

    // Format text fields before storing
    formatString(vehicle.brand);
    formatString(vehicle.model);
    formatString(vehicle.category);
    formatString(vehicle.fuelType);
    formatString(vehicle.transmission);

    // Set default values
    vehicle.isAvailable = true;
    vehicle.onMaintainance = false;

    addVehicleDb(vehicle);
}

// Delete a vehicle
void deleteVehicle()
{
    char input[20];

    readLine("Enter Vehicle ID: ", input, sizeof(input));

    int vehicleId = atoi(input);

    if (vehicleId <= 0)
    {
        printf("Invalid Vehicle ID.\n");
        return;
    }

    deleteVehicleDB(vehicleId);
}

// Vehicle filter criteria menu for admin
void filterVehicle()
{
    int choice;
    int comparisonChoice;
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
        printf("9. Daily Late Fee\n");

        if (isUserAdmin)
        {
            printf("10. Availability\n");
            printf("11. On Maintenance\n");
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
                strcpy(filterField, "registrationNumber");
                break;

            case 5:
                strcpy(filterField, "category");
                break;

            case 6:
                strcpy(filterField, "fuelType");
                break;

            case 7:
                strcpy(filterField, "transmission");
                break;

            case 8:
                strcpy(filterField, "dailyRate");
                break;

            case 9:
                strcpy(filterField, "dailyLateFee");
                break;

            case 10:
                if (!isUserAdmin)
                {
                    printf("Invalid choice. Please try again.\n");
                    continue;
                }

                strcpy(filterField, "isAvailable");
                break;

            case 11:
                if (!isUserAdmin)
                {
                    printf("Invalid choice. Please try again.\n");
                    continue;
                }

                strcpy(filterField, "onMaintainance");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
                continue;
        }

        // Ask for comparison when filtering by money
        if (choice == 8 || choice == 9)
        {
            printf("\n========== COMPARISON ==========\n");
            printf("1. Equal to\n");
            printf("2. Lower than\n");
            printf("3. Higher than\n");

            readLine("Select comparison: ", input, sizeof(input));
            comparisonChoice = atoi(input);

            if (comparisonChoice == 1)
                strcat(filterField, "Equal");
            else if (comparisonChoice == 2)
                strcat(filterField, "Less");
            else if (comparisonChoice == 3)
                strcat(filterField, "Greater");
            else
            {
                printf("Invalid comparison. Please try again.\n");
                continue;
            }
        }

        readLine("Enter filter value: ", filterValue, sizeof(filterValue));

        if (filterValue[0] == '\0')
        {
            printf("Filter value cannot be empty.\n");
            continue;
        }

        // Validate Yes/No for availability and maintenance
        if (strcmp(filterField, "isAvailable") == 0 ||
            strcmp(filterField, "onMaintainance") == 0)
        {
            for (int i = 0; filterValue[i] != '\0'; i++)
                filterValue[i] = tolower((unsigned char)filterValue[i]);

            if (strcmp(filterValue, "yes") != 0 && strcmp(filterValue, "no") != 0)
            {
                printf("Enter either 'Yes' or 'No'.\n");
                continue;
            }
        }

        // Validate positive numeric values for money filters
        if (choice == 8 || choice == 9)
        {
            if (atof(filterValue) <= 0)
            {
                printf("Please enter a valid amount greater than zero.\n");
                continue;
            }
        }

        int count = filterVehicleDb(filterField, filterValue, results, MAX_VEHICLES);

        if (count == -1)
        {
            printf("Unable to search vehicles.\n");
            continue;
        }

        // Customers can only view available vehicles
        if (!isUserAdmin)
            count = extractAvailableVehicles(results, count);

        if (count == 0)
        {
            printf("No matching vehicles found.\n");
            continue;
        }

        displayFilteredVehicle(results, count);
    }
}

// View all the vehicles in the database
void viewVehicle()
{
    viewVehicleDb();
}

// Update any field of the vehicle
void updateVehicle()
{
    char input[100];
    char field[30];
    char newValue[100];
    char *end;

    int vehicleId;
    int choice;

    readLine("Enter Vehicle ID: ", input, sizeof(input));
    vehicleId = atoi(input);

    if (vehicleId <= 0)
    {
        printf("Invalid Vehicle ID.\n");
        return;
    }

    printf("\n========== UPDATE VEHICLE ==========\n");
    printf("1. Brand\n");
    printf("2. Model\n");
    printf("3. Year\n");
    printf("4. Registration Number\n");
    printf("5. Category\n");
    printf("6. Fuel Type\n");
    printf("7. Transmission\n");
    printf("8. Daily Rate\n");
    printf("9. Daily Late Fee\n");
    printf("10. Availability\n");
    printf("11. On Maintenance\n");

    readLine("Enter your choice: ", input, sizeof(input));
    choice = atoi(input);

    // Set the field
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
            strcpy(field, "registrationNumber");
            break;

        case 5:
            strcpy(field, "category");
            break;

        case 6:
            strcpy(field, "fuelType");
            break;

        case 7:
            strcpy(field, "transmission");
            break;

        case 8:
            strcpy(field, "dailyRate");
            break;

        case 9:
            strcpy(field, "dailyLateFee");
            break;

        case 10:
            strcpy(field, "isAvailable");
            break;

        case 11:
            strcpy(field, "onMaintainance");
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    // Set the field value
    readLine("Enter updated value: ", newValue, sizeof(newValue));

    if (newValue[0] == '\0')
    {
        printf("Updated value cannot be empty.\n");
        return;
    }

    // Validate year
    if (choice == 3)
    {
        long year = strtol(newValue, &end, 10);

        if (*end != '\0' || year < 1900 || year > 2030)
        {
            printf("Invalid year.\n");
            return;
        }
    }

    // Validate registration number
    else if (choice == 4)
    {
        if (!isAlphanumeric(newValue))
        {
            printf("Registration number must contain only letters and numbers.\n");
            return;
        }
    }

    // Validate daily rate and daily late fee
    else if (choice == 8 || choice == 9)
    {
        float amount = strtof(newValue, &end);

        if (*end != '\0' || amount <= 0)
        {
            printf("Invalid amount. Enter a value greater than zero.\n");
            return;
        }
    }

    // Validate availability and maintenance status
    else if (choice == 10 || choice == 11)
    {
        for (int i = 0; newValue[i] != '\0'; i++)
        {
            newValue[i] = tolower((unsigned char)newValue[i]);
        }

        if (strcmp(newValue, "yes") != 0 && strcmp(newValue, "no") != 0)
        {
            printf("Enter either 'Yes' or 'No'.\n");
            return;
        }
    }

    // Format text fields before updating
    else
    {
        formatString(newValue);
    }

    int result = updateVehicleDb(vehicleId, field, newValue);

    if (result == 1)
        printf("Vehicle updated successfully.\n");
    else if (result == 0)
        printf("Vehicle ID not found.\n");
    else
        printf("Unable to update vehicle.\n");
}

// View available vehicles
void viewAvailableVehicle()
{
    char filterField[30] = "isAvailable";
    char filterValue[100] = "Yes";

    Vehicle results[MAX_VEHICLES];

    int count = filterVehicleDb(filterField, filterValue, results, MAX_VEHICLES);

    if (count == -1)
    {
        printf("Unable to search vehicles.\n");
        return;
    }

    if (count == 0)
    {
        printf("No available vehicles found.\n");
        return;
    }

    displayFilteredVehicle(results, count);
}

// Send a vehicle for maintenance
void sendVehicleForMaintenance()
{
    char input[20];

    readLine("Enter Vehicle ID: ", input, sizeof(input));

    int vehicleId = atoi(input);

    if (vehicleId <= 0)
    {
        printf("Invalid Vehicle ID.\n");
        return;
    }

    // Check if vehicle exists
    if (!recordExists(vehicleId, VEHICLE_DATA))
    {
        printf("Vehicle not found.\n");
        return;
    }

    // Check if vehicle is available to send for maintainance
    bool isVehicleAvailable = checkVehicleAvailability(vehicleId);

    if (!isVehicleAvailable)
    {
        printf("Vehicle is not available at the moment.\n");
        return;
    }

    // Send the vehicle for maintainance
    updateVehicleMaintenanceStatus(vehicleId);

    printf("Vehicle has been sent for maintainance sucessfully.");
}

// Return a vehicle from maintenance
void returnVehicleFromMaintenance()
{
    char input[20];

    readLine("Enter Vehicle ID: ", input, sizeof(input));

    int vehicleId = atoi(input);

    if (vehicleId <= 0)
    {
        printf("Invalid Vehicle ID.\n");
        return;
    }

    // Check if vehicle exists
    if (!recordExists(vehicleId, VEHICLE_DATA))
    {
        printf("Vehicle not found.\n");
        return;
    }

    // Check if vehicle is available to send for maintainance
    bool isVehicleAvailable = checkVehicleAvailability(vehicleId);

    if (isVehicleAvailable)
    {
        printf("Vehicle has already been returned from maintainance.\n");
        return;
    }

    // Receive the vehicle after maintainance
    updateVehicleMaintenanceStatus(vehicleId);

    printf("Vehicle has been received after maintainance sucessfully.");
}