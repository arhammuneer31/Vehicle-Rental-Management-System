#include <stdio.h>
#include <string.h>

#define TOTAL_CARS 5
#define MAX_NAME_LEN 50

typedef struct {
    int carID;
    char model[MAX_NAME_LEN];
    int rentrateperday;
    int isrented;
    char customerName[MAX_NAME_LEN];
    int rentalDays;
} Car;

Car fleet[TOTAL_CARS];

void clearInputBuffer() {
    while (getchar() != '\n');
}

void initializeFleet() {
    fleet[0].carID = 101;
    strcpy(fleet[0].model, "Sedan Economy");
    fleet[0].rentrateperday = 50;
    fleet[0].isrented = 0;
    strcpy(fleet[0].customerName, "");
    fleet[0].rentalDays = 0;

    fleet[1].carID = 102;
    strcpy(fleet[1].model, "Sedan Comfort");
    fleet[1].rentrateperday = 60;
    fleet[1].isrented = 0;
    strcpy(fleet[1].customerName, "");
    fleet[1].rentalDays = 0;

    fleet[2].carID = 103;
    strcpy(fleet[2].model, "SUV Compact");
    fleet[2].rentrateperday = 70;
    fleet[2].isrented = 0;
    strcpy(fleet[2].customerName, "");
    fleet[2].rentalDays = 0;

    fleet[3].carID = 104;
    strcpy(fleet[3].model, "SUV Premium");
    fleet[3].rentrateperday = 80;
    fleet[3].isrented = 0;
    strcpy(fleet[3].customerName, "");
    fleet[3].rentalDays = 0;

    fleet[4].carID = 105;
    strcpy(fleet[4].model, "Luxury Sports");
    fleet[4].rentrateperday = 90;
    fleet[4].isrented = 0;
    strcpy(fleet[4].customerName, "");
    fleet[4].rentalDays = 0;
}

void displayMenu() {
    printf("\n========================================\n");
    printf("     CAR RENTAL MANAGEMENT SYSTEM       \n");
    printf("========================================\n");
    printf(" 1. Display Fleet & Availability\n");
    printf(" 2. Rent a Car\n");
    printf(" 3. Return a Car\n");
    printf(" 4. Search Car Details\n");
    printf(" 5. Generate System Report\n");
    printf(" 6. Exit System\n");
    printf("========================================\n");
    printf("Enter your choice (1-6): ");
}

void displayCars() {
    printf("\n%-10s %-20s %-15s %-12s\n", "Car ID", "Model Type", "Rate (Per Day)", "Status");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < TOTAL_CARS; i++) {
        printf("%-10d %-20s $%*-d ", fleet[i].carID, fleet[i].model, 13, fleet[i].rentrateperday);
        if (fleet[i].isrented) {
            printf("%-12s\n", "Rented");
        } else {
            printf("%-12s\n", "Available");
        }
    }
    printf("------------------------------------------------------------\n");
}

int findCarIndex(int id) {
    for (int i = 0; i < TOTAL_CARS; i++) {
        if (fleet[i].carID == id) {
            return i;
        }
    }
    return -1;
}

void rentCar() {
    int id;
    int index;
    int days;
    char name[MAX_NAME_LEN];

    printf("\n--- Rent a Car Process ---\n");
    printf("Enter the Car ID you want to rent: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid numeric input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findCarIndex(id);
    if (index == -1) {
        printf("Error: Car ID %d not found in database.\n", id);
        return;
    }

    if (fleet[index].isrented) {
        printf("Error: Car %d is already rented out to %s.\n", id, fleet[index].customerName);
        return;
    }

    printf("Enter Customer Name: ");
    if (fgets(name, MAX_NAME_LEN, stdin) != NULL) {
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n') {
            name[len - 1] = '\0';
        }
    }

    printf("Enter rental duration in days: ");
    if (scanf("%d", &days) != 1 || days <= 0) {
        printf("Invalid rental duration days.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fleet[index].isrented = 1;
    fleet[index].rentalDays = days;
    strcpy(fleet[index].customerName, name);

    int totalCost = fleet[index].rentrateperday * days;
    printf("\nSuccess: Car %d (%s) has been successfully rented!\n", id, fleet[index].model);
    printf("Customer: %s\n", fleet[index].customerName);
    printf("Duration: %d days\n", fleet[index].rentalDays);
    printf("Total Estimated Cost: $%d\n", totalCost);
}

void returnCar() {
    int id;
    int index;

    printf("\n--- Return a Car Process ---\n");
    printf("Enter the Car ID being returned: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input format.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findCarIndex(id);
    if (index == -1) {
        printf("Error: Car ID %d does not exist.\n", id);
        return;
    }

    if (!fleet[index].isrented) {
        printf("Verification System: Car %d is already in the garage.\n", id);
        return;
    }

    int totalBill = fleet[index].rentrateperday * fleet[index].rentalDays;
    printf("\nProcessing Invoice Generation...\n");
    printf("----------------------------------------\n");
    printf("Final Receipt Statement\n");
    printf("----------------------------------------\n");
    printf("Car Model:     %s\n", fleet[index].model);
    printf("Rented By:     %s\n", fleet[index].customerName);
    printf("Total Days:    %d days\n", fleet[index].rentalDays);
    printf("Rate per day:  $%d\n", fleet[index].rentrateperday);
    printf("Total Balance: $%d\n", totalBill);
    printf("----------------------------------------\n");

    fleet[index].isrented = 0;
    fleet[index].rentalDays = 0;
    strcpy(fleet[index].customerName, "");

    printf("Database Updated: Return logged successfully.\n");
}

void searchCar() {
    int id;
    int index;

    printf("\n--- Advanced Query Screen ---\n");
    printf("Enter Car ID to look up: ");
    if (scanf("%d", &id) != 1) {
        printf("Numeric input expected.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findCarIndex(id);
    if (index == -1) {
        printf("Record for Car ID %d could not be found.\n", id);
        return;
    }

    printf("\nMatches Found:\n");
    printf("Car Identification Code: %d\n", fleet[index].carID);
    printf("Manufacturer Category:   %s\n", fleet[index].model);
    printf("Standard Pricing Unit:   $%d / Day\n", fleet[index].rentrateperday);
    if (fleet[index].isrented) {
        printf("Current Operations State: Occupied\n");
        printf("Active Account Holder:   %s\n", fleet[index].customerName);
        printf("Contract Length:         %d days remaining\n", fleet[index].rentalDays);
    } else {
        printf("Current Operations State: Available for immediate hire\n");
    }
}

void generateReport() {
    int totalRentedCount = 0;
    int potentialRevenue = 0;

    printf("\n========================================\n");
    printf("          EXECUTIVE FLEET REPORT        \n");
    printf("========================================\n");

    for (int i = 0; i < TOTAL_CARS; i++) {
        if (fleet[i].isrented) {
            totalRentedCount++;
            potentialRevenue += (fleet[i].rentrateperday * fleet[i].rentalDays);
        }
    }

    printf("Total Vehicles Managed:       %d\n", TOTAL_CARS);
    printf("Active Rentals Out:           %d\n", totalRentedCount);
    printf("Available Vehicles In Stock:  %d\n", TOTAL_CARS - totalRentedCount);
    printf("Current Operational Revenue:  $%d\n", potentialRevenue);
    printf("========================================\n");
}

int main() {
    initializeFleet();

    printf("Car rental system initialized with %d cars.\n", TOTAL_CARS);
    printf("Car rental system is ready for operations.\n");

    int choice = 0;

    while (choice != 6) {
        displayMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Error reading choice. Resetting menu...\n");
            clearInputBuffer();
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1:
                displayCars();
                break;
            case 2:
                rentCar();
                break;
            case 3:
                returnCar();
                break;
            case 4:
                searchCar();
                break;
            case 5:
                generateReport();
                break;
            case 6:
                printf("\nClosing all database operations...\n");
                printf("Thank you for using the car rental system. Goodbye!\n");
                break;
            default:
                printf("Entry sequence unassigned. Please input a selection from 1 to 6.\n");
        }
    }

    return 0;
}

