#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../env.h"
#include "../../struct.h"
#include "../../Utils/utilities.c"

// Add a row of user details to file
void addVehicle(Vehicle vehicle)
{
    FILE *file = fopen(VEHICLE_DATA, "a");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return;
    }

    vehicle.vehicle_id = nextId(VEHICLE_DATA);
    vehicle.isAvailable = true;

    fprintf(file, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%d\n",
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

    printf("Vehicle added successfully. ID: %d\n", vehicle.vehicle_id);
}

// Delete a user using vehicle_id through temp file
void deleteVehicle(int vehicle_id)
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

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

// Check whether the vehicle is available or not
bool checkVehicleAvailability(int vehicle_id)
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return false;
    }

    char line[512];
    int current_id;
    int year;
    float daily_rate;
    int isAvailable;

    char brand[50];
    char model[50];
    char registration_number[30];
    char category[30];
    char fuel_type[20];
    char transmission[20];

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%d",
                   &current_id,
                   brand,
                   model,
                   &year,
                   registration_number,
                   category,
                   fuel_type,
                   transmission,
                   &daily_rate,
                   &isAvailable) != 10)
        {
            continue;
        }

        if (current_id == vehicle_id)
        {
            fclose(file);

            if (isAvailable)
            {
                printf("Vehicle is available.\n");
                return true;
            }
            else
            {
                printf("Vehicle is not available.\n");
                return false;
            }
        }
    }

    fclose(file);

    printf("Vehicle not found.\n");
    return false;
}

// Update the vehicle availability status
void updateVehicleAvailability(int vehicle_id)
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    char line[512];
    int current_id;
    int year;
    int isAvailable;
    float daily_rate;

    char brand[50];
    char model[50];
    char registration_number[30];
    char category[30];
    char fuel_type[20];
    char transmission[20];

    int found = 0;

    if (fgets(line, sizeof(line), file) != NULL)
    {
        fputs(line, temp);
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%d",
                   &current_id, brand, model, &year,
                   registration_number, category, fuel_type,
                   transmission, &daily_rate, &isAvailable) != 10)
        {
            fputs(line, temp);
            continue;
        }

        if (current_id == vehicle_id)
        {
            found = 1;

            isAvailable = !isAvailable;

            printf("Vehicle availability updated to: %s\n",
                   isAvailable ? "Available" : "Unavailable");
        }

        fprintf(temp, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%d\n",
                current_id, brand, model, year,
                registration_number, category, fuel_type,
                transmission, daily_rate, isAvailable);
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
void viewAllVehicle()
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return;
    }

    char line[512];

    int vehicle_id;
    int year;
    int isAvailable;
    float daily_rate;

    char brand[50];
    char model[50];
    char registration_number[30];
    char category[30];
    char fuel_type[20];
    char transmission[20];

    fgets(line, sizeof(line), file);

    printf("\n");
    printf("============================================================================================\n");
    printf("                                       VEHICLES                                             \n");
    printf("============================================================================================\n");

    printf("%-5s %-12s %-12s %-6s %-15s %-12s %-10s %-12s %-10s %-12s\n",
           "ID", "Brand", "Model", "Year", "Registration",
           "Category", "Fuel", "Transmission", "Rate/Day", "Availability");

    printf("--------------------------------------------------------------------------------------------\n");

    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%49[^,],%d,%29[^,],%29[^,],%19[^,],%19[^,],%f,%d",
                   &vehicle_id, brand, model, &year,
                   registration_number, category, fuel_type,
                   transmission, &daily_rate, &isAvailable) != 10)
        {
            continue;
        }

        printf("%-5d %-12s %-12s %-6d %-15s %-12s %-10s %-12s %-10.2f %-12s\n",
               vehicle_id,
               brand,
               model,
               year,
               registration_number,
               category,
               fuel_type,
               transmission,
               daily_rate,
               isAvailable ? "Available" : "Unavailable");

        count++;
    }

    printf("============================================================================================\n");

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
int filterVehicle(const char filterField[], const char filterValue[], Vehicle results[], int maxResults)
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return -1;
    }

    char line[512];

    Vehicle vehicle;
    int count = 0;

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
                   &vehicle.isAvailable) != 10)
        {
            continue;
        }

        bool match = false;

        if (strcmp(filterField, "brand") == 0)
            match = (strcmp(vehicle.brand, filterValue) == 0);

        else if (strcmp(filterField, "model") == 0)
            match = (strcmp(vehicle.model, filterValue) == 0);

        else if (strcmp(filterField, "year") == 0)
            match = (vehicle.year == atoi(filterValue));

        else if (strcmp(filterField, "registration_number") == 0)
            match = (strcmp(vehicle.registration_number, filterValue) == 0);

        else if (strcmp(filterField, "category") == 0)
            match = (strcmp(vehicle.category, filterValue) == 0);

        else if (strcmp(filterField, "fuel_type") == 0)
            match = (strcmp(vehicle.fuel_type, filterValue) == 0);

        else if (strcmp(filterField, "transmission") == 0)
            match = (strcmp(vehicle.transmission, filterValue) == 0);

        else if (strcmp(filterField, "daily_rate") == 0)
            match = (vehicle.daily_rate == (float)atof(filterValue));

        else if (strcmp(filterField, "isAvailable") == 0)
        {
            if (strcmp(filterValue, "available") == 0)
                match = (vehicle.isAvailable == 1);

            else if (strcmp(filterValue, "unavailable") == 0)
                match = (vehicle.isAvailable == 0);
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
void displayFilteredVehicles(Vehicle vehicles[], int count)
{
    printf("\n");
    printf("============================================================================================\n");
    printf("                                          VEHICLES                                          \n");
    printf("============================================================================================\n");

    printf("%-5s %-12s %-12s %-6s %-15s %-12s %-10s %-12s %-10s %-12s\n",
           "ID", "Brand", "Model", "Year", "Registration",
           "Category", "Fuel", "Transmission", "Rate/Day", "Availability");

    printf("--------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        printf("%-5d %-12s %-12s %-6d %-15s %-12s %-10s %-12s %-10.2f %-12s\n",
               vehicles[i].vehicle_id,
               vehicles[i].brand,
               vehicles[i].model,
               vehicles[i].year,
               vehicles[i].registration_number,
               vehicles[i].category,
               vehicles[i].fuel_type,
               vehicles[i].transmission,
               vehicles[i].daily_rate,
               vehicles[i].isAvailable ? "Available" : "Unavailable");
    }

    printf("============================================================================================\n");
    printf("Total Vehicles: %d\n", count);
}

// Update vehicle attribute
int updateVehicle(int vehicle_id, const char field[], const char newValue[])
{
    FILE *file = fopen(VEHICLE_DATA, "r");

    if (file == NULL)
    {
        printf("Error opening vehicles.csv\n");
        return -1;
    }

    FILE *temp = fopen(TEMP_DATA, "w");

    if (temp == NULL)
    {
        fclose(file);
        printf("Error creating temporary file\n");
        return -1;
    }

    char line[512];
    Vehicle vehicle;
    int found = 0;

    if (fgets(line, sizeof(line), file) != NULL)
        fputs(line, temp);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int isAvailable;

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
                   &isAvailable) != 10)
        {
            fputs(line, temp);
            continue;
        }

        vehicle.isAvailable = isAvailable;

        if (vehicle.vehicle_id == vehicle_id)
        {
            found = 1;

            if (strcmp(field, "brand") == 0)
                snprintf(vehicle.brand, sizeof(vehicle.brand), "%s", newValue);

            else if (strcmp(field, "model") == 0)
                snprintf(vehicle.model, sizeof(vehicle.model), "%s", newValue);

            else if (strcmp(field, "year") == 0)
                vehicle.year = atoi(newValue);

            else if (strcmp(field, "registration_number") == 0)
                snprintf(vehicle.registration_number, sizeof(vehicle.registration_number), "%s", newValue);

            else if (strcmp(field, "category") == 0)
                snprintf(vehicle.category, sizeof(vehicle.category), "%s", newValue);

            else if (strcmp(field, "fuel_type") == 0)
                snprintf(vehicle.fuel_type, sizeof(vehicle.fuel_type), "%s", newValue);

            else if (strcmp(field, "transmission") == 0)
                snprintf(vehicle.transmission, sizeof(vehicle.transmission), "%s", newValue);

            else if (strcmp(field, "daily_rate") == 0)
                vehicle.daily_rate = atof(newValue);

            else if (strcmp(field, "isAvailable") == 0)
                vehicle.isAvailable = (strcmp(newValue, "available") == 0);

            else
            {
                fclose(file);
                fclose(temp);
                remove(TEMP_DATA);
                return -1;
            }

            fprintf(temp, "%d,%s,%s,%d,%s,%s,%s,%s,%.2f,%d\n",
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
        }
        else
        {
            fputs(line, temp);
        }
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
    {
        return -1;
    }

    return 1;
}