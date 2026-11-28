#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../env.h"
#include "../struct.c"
#include "./utilities.c"

// Add a vehicle to the database
void addVehicleDb(Vehicle vehicle)
{
    FILE *file = fopen(VEHICLE_DATA, "a");

    if (file == NULL)
    {
        printf("Error opening vehicle.csv\n");
        return;
    }

    vehicle.vehicleId = nextId(VEHICLE_DATA);
    vehicle.isAvailable = true;
    vehicle.onMaintainance = false;

    fprintf(file, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d\n",
            vehicle.vehicleId,
            vehicle.brand,
            vehicle.model,
            vehicle.year,
            vehicle.registrationNumber,
            vehicle.category,
            vehicle.fuelType,
            vehicle.transmission,
            vehicle.dailyRate,
            vehicle.dailyLateFee,
            vehicle.isAvailable,
            vehicle.onMaintainance);

    fclose(file);

    printf("Vehicle added successfully. ID: %d\n", vehicle.vehicleId);
}

// Delete a vehicle using its vehicleId
void deleteVehicleDb(int vehicleId)
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
        return;

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    fprintf(temp, "vehicleId,brand,model,year,registrationNumber,category,fuelType,transmission,dailyRate,dailyLateFee,isAvailable,onMaintainance\n");

    char line[512];
    int found = 0;
    int currentId;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,", &currentId) == 1 && currentId == vehicleId)
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
        remove(VEHICLE_DATA);
        rename(TEMP_DATA, VEHICLE_DATA);

        printf("Vehicle deleted successfully.\n");
    }
    else
    {
        remove(TEMP_DATA);
        printf("Vehicle not found.\n");
    }
}

// Check whether a vehicle is available or not
bool checkVehicleAvailability(int vehicleId)
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
    {
        return false;
    }

    char line[512];
    int currentId, year, isAvailable, onMaintainance;
    float dailyRate, dailyLateFee;

    char brand[50], model[50], registrationNumber[30];
    char category[30], fuelType[20], transmission[20];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%f,%d,%d", 
                   &currentId, brand, model, &year,
                   registrationNumber, category, fuelType,
                   transmission, &dailyRate, &dailyLateFee,
                   &isAvailable, &onMaintainance) != 12)
        {
            continue;
        }

        if (currentId == vehicleId)
        {
            fclose(file);

            if (isAvailable)
            {
                printf("Vehicle is available.\n");
                return true;
            }

            printf("Vehicle is not available.\n");
            return false;
        }
    }

    fclose(file);

    printf("Vehicle not found.\n");
    return false;
}

// Update the vehicle availability status
void updateVehicleAvailability(int vehicleId, bool isAvailable)
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
        return;

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    fprintf(temp, "vehicleId,brand,model,year,registrationNumber,category,fuelType,transmission,dailyRate,dailyLateFee,isAvailable,onMaintainance\n");

    char line[512];
    int currentId, year, currentAvailability, onMaintainance;
    float dailyRate, dailyLateFee;

    char brand[50], model[50], registrationNumber[30];
    char category[30], fuelType[20], transmission[20];

    int found = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%f,%d,%d",
                   &currentId, brand, model, &year,
                   registrationNumber, category, fuelType,
                   transmission, &dailyRate, &dailyLateFee,
                   &currentAvailability, &onMaintainance) != 12)
        {
            fputs(line, temp);
            continue;
        }

        if (currentId == vehicleId)
        {
            found = 1;
            currentAvailability = isAvailable;
        }

        fprintf(temp, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d\n",
                currentId, brand, model, year,
                registrationNumber, category, fuelType,
                transmission, dailyRate, dailyLateFee,
                currentAvailability, onMaintainance);
    }

    fclose(file);
    fclose(temp);

    if (found)
    {
        remove(VEHICLE_DATA);
        rename(TEMP_DATA, VEHICLE_DATA);

        printf("Vehicle availability updated to: %s\n", isAvailable ? "Available" : "Unavailable");
    }
    else
    {
        remove(TEMP_DATA);
        printf("Vehicle not found.\n");
    }
}

// Toggle vehicle maintenance status
void updateVehicleMaintenanceStatus(int vehicleId)
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
    {
        return;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    fprintf(temp, "vehicleId,brand,model,year,registrationNumber,category,fuelType,transmission,dailyRate,dailyLateFee,isAvailable,onMaintainance\n");

    char line[512];
    int currentId, year, isAvailable, onMaintainance;
    float dailyRate, dailyLateFee;

    char brand[50], model[50], registrationNumber[30];
    char category[30], fuelType[20], transmission[20];

    int found = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%f,%d,%d",
                   &currentId, brand, model, &year,
                   registrationNumber, category, fuelType,
                   transmission, &dailyRate, &dailyLateFee,
                   &isAvailable, &onMaintainance) != 12)
        {
            fputs(line, temp);
            continue;
        }

        if (currentId == vehicleId)
        {
            found = 1;

            onMaintainance = !onMaintainance;
            isAvailable = !onMaintainance;

            printf("Maintenance status updated to: %s\n", onMaintainance ? "On Maintenance" : "Not on Maintenance");

            printf("Vehicle availability updated to: %s\n", isAvailable ? "Available" : "Unavailable");
        }

        fprintf(temp, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d\n",
                currentId, brand, model, year,
                registrationNumber, category, fuelType,
                transmission, dailyRate, dailyLateFee,
                isAvailable, onMaintainance);
    }

    fclose(file);
    fclose(temp);

    if (found)
    {
        remove(VEHICLE_DATA);
        rename(TEMP_DATA, VEHICLE_DATA);
    }
    else
    {
        remove(TEMP_DATA);
        printf("Vehicle not found.\n");
    }
}

// View all vehicles
void viewVehicleDb()
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
    {
        return;
    }

    char line[512];

    int vehicleId, year, isAvailable, onMaintainance;
    float dailyRate, dailyLateFee;

    char brand[50], model[50], registrationNumber[30];
    char category[30], fuelType[20], transmission[20];

    printf("\n");
    printf("====================================================================================================================\n");
    printf("                                           VRental Vehicle Inventory                                                \n");
    printf("====================================================================================================================\n");

    printf("%-5s %-12s %-12s %-6s %-13s %-12s %-10s %-12s %-10s %-10s %-12s %-12s\n",
           "ID", "Brand", "Model", "Year", "Registration",
           "Category", "Fuel", "Transmission", "Rate/Day",
           "Late Fee", "Availability", "Maintenance");

    printf("--------------------------------------------------------------------------------------------------------------------\n");

    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%f,%d,%d",
                   &vehicleId, brand, model, &year,
                   registrationNumber, category, fuelType,
                   transmission, &dailyRate, &dailyLateFee,
                   &isAvailable, &onMaintainance) != 12)
        {
            continue;
        }

        printf("%-5d %-12s %-12s %-6d %-13s %-12s %-10s %-12s %-10.2f %-10.2f %-12s %-12s\n",
               vehicleId, brand, model, year,
               registrationNumber, category, fuelType,
               transmission, dailyRate, dailyLateFee,
               isAvailable ? "Available" : "Unavailable",
               onMaintainance ? "Yes" : "No");

        count++;
    }

    printf("====================================================================================================================\n");

    if (count == 0)
    {
        printf("No vehicles found.\n");
    }
    else
    {
        printf("Total Vehicles: %d\n", count);
    }

    fclose(file);
}

// Vehicle filters
int filterVehicleDb(const char filterField[], const char filterValue[], Vehicle results[], int maxResults)
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
    {
        return -1;
    }

    char line[512];
    Vehicle vehicle;
    int count = 0;

    char searchValue[100];
    snprintf(searchValue, sizeof(searchValue), "%s", filterValue);

    // Convert search value to lowercase for text-based filters
    if (strcmp(filterField, "brand") == 0 ||
        strcmp(filterField, "model") == 0 ||
        strcmp(filterField, "registrationNumber") == 0 ||
        strcmp(filterField, "category") == 0 ||
        strcmp(filterField, "fuelType") == 0 ||
        strcmp(filterField, "transmission") == 0)
    {
        for (int i = 0; searchValue[i] != '\0'; i++)
            searchValue[i] = tolower((unsigned char)searchValue[i]);
    }

    char statusValue[100];
    snprintf(statusValue, sizeof(statusValue), "%s", filterValue);

    for (int i = 0; statusValue[i] != '\0'; i++)
        statusValue[i] = tolower((unsigned char)statusValue[i]);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line,
                   "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%f,%d,%d",
                   &vehicle.vehicleId, vehicle.brand, vehicle.model,
                   &vehicle.year, vehicle.registrationNumber, vehicle.category,
                   vehicle.fuelType, vehicle.transmission,
                   &vehicle.dailyRate, &vehicle.dailyLateFee,
                   &vehicle.isAvailable, &vehicle.onMaintainance) != 12)
            continue;

        // Convert vehicle text fields to lowercase before comparing
        char vehicleValue[100];

        bool match = false;

        if (strcmp(filterField, "brand") == 0)
        {
            snprintf(vehicleValue, sizeof(vehicleValue), "%s", vehicle.brand);
        }
        else if (strcmp(filterField, "model") == 0)
        {
            snprintf(vehicleValue, sizeof(vehicleValue), "%s", vehicle.model);
        }
        else if (strcmp(filterField, "registrationNumber") == 0)
        {
            snprintf(vehicleValue, sizeof(vehicleValue), "%s", vehicle.registrationNumber);
        }
        else if (strcmp(filterField, "category") == 0)
        {
            snprintf(vehicleValue, sizeof(vehicleValue), "%s", vehicle.category);
        }
        else if (strcmp(filterField, "fuelType") == 0)
        {
            snprintf(vehicleValue, sizeof(vehicleValue), "%s", vehicle.fuelType);
        }
        else if (strcmp(filterField, "transmission") == 0)
        {
            snprintf(vehicleValue, sizeof(vehicleValue), "%s", vehicle.transmission);
        }

        if (strcmp(filterField, "brand") == 0 ||
            strcmp(filterField, "model") == 0 ||
            strcmp(filterField, "registrationNumber") == 0 ||
            strcmp(filterField, "category") == 0 ||
            strcmp(filterField, "fuelType") == 0 ||
            strcmp(filterField, "transmission") == 0)
        {
            for (int i = 0; vehicleValue[i] != '\0'; i++)
                vehicleValue[i] = tolower((unsigned char)vehicleValue[i]);

            match = (strcmp(vehicleValue, searchValue) == 0);
        }
        else if (strcmp(filterField, "year") == 0)
            match = (vehicle.year == atoi(filterValue));
        else if (strcmp(filterField, "dailyRateEqual") == 0)
            match = vehicle.dailyRate == (float)atof(filterValue);
        else if (strcmp(filterField, "dailyRateLess") == 0)
            match = vehicle.dailyRate < (float)atof(filterValue);
        else if (strcmp(filterField, "dailyRateGreater") == 0)
            match = vehicle.dailyRate > (float)atof(filterValue);
        else if (strcmp(filterField, "dailyLateFeeEqual") == 0)
            match = vehicle.dailyLateFee == (float)atof(filterValue);
        else if (strcmp(filterField, "dailyLateFeeLess") == 0)
            match = vehicle.dailyLateFee < (float)atof(filterValue);
        else if (strcmp(filterField, "dailyLateFeeGreater") == 0)
            match = vehicle.dailyLateFee > (float)atof(filterValue);
        else if (strcmp(filterField, "isAvailable") == 0)
        {
            if (strcmp(statusValue, "yes") == 0)
                match = vehicle.isAvailable == 1;
            else if (strcmp(statusValue, "no") == 0)
                match = vehicle.isAvailable == 0;
        }
        else if (strcmp(filterField, "onMaintainance") == 0)
        {
            if (strcmp(statusValue, "yes") == 0)
                match = vehicle.onMaintainance == 1;
            else if (strcmp(statusValue, "no") == 0)
                match = vehicle.onMaintainance == 0;
        }

        if (match && count < maxResults)
        {
            results[count] = vehicle;
            count++;
        }
    }

    fclose(file);
    return count;
}

// Display matching vehicles
void displayFilteredVehicle(Vehicle vehicles[], int count)
{
    printf("\n");
    printf("====================================================================================================================\n");
    printf("                                           VRental Vehicle Inventory                                                \n");
    printf("====================================================================================================================\n");

    printf("%-5s %-12s %-12s %-6s %-13s %-12s %-10s %-12s %-10s %-10s %-12s %-12s\n",
           "ID", "Brand", "Model", "Year", "Registration",
           "Category", "Fuel", "Transmission", "Rate/Day",
           "Late Fee", "Availability", "Maintenance");

    printf("--------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        printf("%-5d %-12s %-12s %-6d %-13s %-12s %-10s %-12s %-10.2f %-10.2f %-12s %-12s\n",
               vehicles[i].vehicleId,
               vehicles[i].brand,
               vehicles[i].model,
               vehicles[i].year,
               vehicles[i].registrationNumber,
               vehicles[i].category,
               vehicles[i].fuelType,
               vehicles[i].transmission,
               vehicles[i].dailyRate,
               vehicles[i].dailyLateFee,
               vehicles[i].isAvailable ? "Available" : "Unavailable",
               vehicles[i].onMaintainance ? "Yes" : "No");
    }

    printf("====================================================================================================================\n");
    printf("Total Vehicles: %d\n", count);
}

// Update vehicle attribute
int updateVehicleDb(int vehicleId, const char field[], const char newValue[])
{
    FILE *file = openFile(VEHICLE_DATA);

    if (file == NULL)
    {
        return -1;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return -1;
    }

    fprintf(temp, "vehicleId,brand,model,year,registrationNumber,category,fuelType,transmission,dailyRate,dailyLateFee,isAvailable,onMaintainance\n");

    char line[512];

    int currentId, year, isAvailable, onMaintainance;
    float dailyRate, dailyLateFee;

    char brand[50], model[50], registrationNumber[30];
    char category[30], fuelType[20], transmission[20];

    int found = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%f,%d,%d",
                   &currentId, brand, model, &year,
                   registrationNumber, category, fuelType,
                   transmission, &dailyRate, &dailyLateFee,
                   &isAvailable, &onMaintainance) != 12)
        {
            fputs(line, temp);
            continue;
        }

        if (currentId == vehicleId)
        {
            found = 1;

            if (strcmp(field, "brand") == 0)
                snprintf(brand, sizeof(brand), "%s", newValue);

            else if (strcmp(field, "model") == 0)
                snprintf(model, sizeof(model), "%s", newValue);

            else if (strcmp(field, "year") == 0)
                year = atoi(newValue);

            else if (strcmp(field, "registrationNumber") == 0)
                snprintf(registrationNumber, sizeof(registrationNumber), "%s", newValue);

            else if (strcmp(field, "category") == 0)
                snprintf(category, sizeof(category), "%s", newValue);

            else if (strcmp(field, "fuelType") == 0)
                snprintf(fuelType, sizeof(fuelType), "%s", newValue);

            else if (strcmp(field, "transmission") == 0)
                snprintf(transmission, sizeof(transmission), "%s", newValue);

            else if (strcmp(field, "dailyRate") == 0)
                dailyRate = atof(newValue);

            else if (strcmp(field, "dailyLateFee") == 0)
                dailyLateFee = atof(newValue);

            else if (strcmp(field, "isAvailable") == 0)
                isAvailable = strcmp(newValue, "yes") == 0 ? 1 : 0;

            else if (strcmp(field, "onMaintainance") == 0)
                onMaintainance = strcmp(newValue, "yes") == 0 ? 1 : 0;

            else
            {
                fclose(file);
                fclose(temp);
                remove(TEMP_DATA);
                return -1;
            }
        }

        fprintf(temp, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d\n",
                currentId, brand, model, year,
                registrationNumber, category, fuelType,
                transmission, dailyRate, dailyLateFee,
                isAvailable, onMaintainance);
    }

    fclose(file);
    fclose(temp);

    if (!found)
    {
        remove(TEMP_DATA);
        return 0;
    }

    if (remove(VEHICLE_DATA) != 0)
    {
        remove(TEMP_DATA);
        return -1;
    }

    if (rename(TEMP_DATA, VEHICLE_DATA) != 0)
        return -1;

    return 1;
}

// Extract only available vehicles from the list
int extractAvailableVehicle(Vehicle vehicles[], int count)
{
    int availableCount = 0;

    for (int i = 0; i < count; i++)
    {
        if (vehicles[i].isAvailable)
        {
            vehicles[availableCount] = vehicles[i];
            availableCount++;
        }
    }

    return availableCount;
}

