#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CARS 20
#define MAX_LEN 50
#define MAX_DAYS 365
#define DISCOUNT_DAYS 7
#define DISCOUNT_PERCENT 10

typedef struct {
    int id;
    char model[MAX_LEN];
    int rate;
    int rented;
    char customer[MAX_LEN];
    int days;
} Car;

Car cars[MAX_CARS];
int carCount = 0;
int totalEarnings = 0;

void clearBuffer() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getNumber(int *num) {
    int result = scanf("%d", num);
    if (result == EOF) {
        printf("\nInput closed. Exiting.\n");
        exit(0);
    }
    clearBuffer();
    return result == 1;
}

void getText(char text[], int size) {
    int len = 0;
    if (fgets(text, size, stdin) == NULL) {
        text[0] = '\0';
        return;
    }
    len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
    } else {
        clearBuffer();
    }
}

void addCar(int id, const char model[], int rate) {
    cars[carCount].id = id;
    strcpy(cars[carCount].model, model);
    cars[carCount].rate = rate;
    cars[carCount].rented = 0;
    strcpy(cars[carCount].customer, "");
    cars[carCount].days = 0;
    carCount++;
}

void loadCars() {
    addCar(101, "Sedan Economy", 50);
    addCar(102, "Sedan Comfort", 60);
    addCar(103, "SUV Compact", 70);
    addCar(104, "SUV Premium", 80);
    addCar(105, "Luxury Sports", 90);
}

int findCar(int id) {
    for (int i = 0; i < carCount; i++) {
        if (cars[i].id == id) {
            return i;
        }
    }
    return -1;
}

int getCost(int rate, int days) {
    int cost = rate * days;
    if (days >= DISCOUNT_DAYS) {
        cost = cost - cost * DISCOUNT_PERCENT / 100;
    }
    return cost;
}

void showMenu() {
    printf("\n========================================\n");
    printf("     CAR RENTAL MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf(" 1. Show all cars\n");
    printf(" 2. Show available cars\n");
    printf(" 3. Rent a car\n");
    printf(" 4. Return a car\n");
    printf(" 5. Search for a car\n");
    printf(" 6. Add a new car\n");
    printf(" 7. Show report\n");
    printf(" 8. Exit\n");
    printf("========================================\n");
    printf("Enter your choice (1-8): ");
}

void printHeader() {
    printf("\n%-10s %-20s %-15s %-12s\n", "Car ID", "Model", "Rate Per Day", "Status");
    printf("------------------------------------------------------------\n");
}

void printCar(int i) {
    printf("%-10d %-20s $%-14d %-12s\n", cars[i].id, cars[i].model, cars[i].rate,
           cars[i].rented ? "Rented" : "Available");
}

void showAllCars() {
    printHeader();
    for (int i = 0; i < carCount; i++) {
        printCar(i);
    }
    printf("------------------------------------------------------------\n");
}

void showAvailableCars() {
    int count = 0;

    printHeader();
    for (int i = 0; i < carCount; i++) {
        if (!cars[i].rented) {
            printCar(i);
            count++;
        }
    }
    printf("------------------------------------------------------------\n");

    if (count == 0) {
        printf("No cars are available right now.\n");
    } else {
        printf("%d car(s) available.\n", count);
    }
}

void rentCar() {
    int id = 0, index = -1, days = 0, cost = 0;
    char name[MAX_LEN] = "";

    printf("\n--- Rent a Car ---\n");
    printf("Enter car ID: ");
    if (!getNumber(&id)) {
        printf("Please enter a number.\n");
        return;
    }

    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    if (cars[index].rented) {
        printf("Car %d is already rented by %s.\n", id, cars[index].customer);
        return;
    }

    printf("Enter customer name: ");
    getText(name, MAX_LEN);
    if (strlen(name) == 0) {
        printf("Name cannot be empty.\n");
        return;
    }

    printf("Enter number of days (1-%d): ", MAX_DAYS);
    if (!getNumber(&days) || days <= 0 || days > MAX_DAYS) {
        printf("Invalid number of days.\n");
        return;
    }

    cars[index].rented = 1;
    cars[index].days = days;
    strcpy(cars[index].customer, name);

    cost = getCost(cars[index].rate, days);
    printf("\nCar %d (%s) rented successfully.\n", id, cars[index].model);
    printf("Customer: %s\n", name);
    printf("Days: %d\n", days);
    if (days >= DISCOUNT_DAYS) {
        printf("%d%% discount applied for long rental.\n", DISCOUNT_PERCENT);
    }
    printf("Total cost: $%d\n", cost);
}

void returnCar() {
    int id = 0, index = -1, bill = 0;

    printf("\n--- Return a Car ---\n");
    printf("Enter car ID: ");
    if (!getNumber(&id)) {
        printf("Please enter a number.\n");
        return;
    }

    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    if (!cars[index].rented) {
        printf("Car %d is not rented.\n", id);
        return;
    }

    bill = getCost(cars[index].rate, cars[index].days);
    printf("\n----------------------------------------\n");
    printf("RECEIPT\n");
    printf("----------------------------------------\n");
    printf("Car:           %s\n", cars[index].model);
    printf("Customer:      %s\n", cars[index].customer);
    printf("Days:          %d\n", cars[index].days);
    printf("Rate per day:  $%d\n", cars[index].rate);
    if (cars[index].days >= DISCOUNT_DAYS) {
        printf("Discount:      %d%%\n", DISCOUNT_PERCENT);
    }
    printf("Total:         $%d\n", bill);
    printf("----------------------------------------\n");

    totalEarnings += bill;
    cars[index].rented = 0;
    cars[index].days = 0;
    strcpy(cars[index].customer, "");

    printf("Car returned successfully.\n");
}

void searchCar() {
    int id = 0, index = -1;

    printf("\n--- Search for a Car ---\n");
    printf("Enter car ID: ");
    if (!getNumber(&id)) {
        printf("Please enter a number.\n");
        return;
    }

    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }

    printf("\nCar ID:    %d\n", cars[index].id);
    printf("Model:     %s\n", cars[index].model);
    printf("Rate:      $%d per day\n", cars[index].rate);
    if (cars[index].rented) {
        printf("Status:    Rented\n");
        printf("Customer:  %s\n", cars[index].customer);
        printf("Days:      %d\n", cars[index].days);
    } else {
        printf("Status:    Available\n");
    }
}

void addNewCar() {
    int id = 0, rate = 0;
    char model[MAX_LEN] = "";

    printf("\n--- Add a New Car ---\n");
    if (carCount >= MAX_CARS) {
        printf("The fleet is full (max %d cars).\n", MAX_CARS);
        return;
    }

    printf("Enter new car ID: ");
    if (!getNumber(&id) || id <= 0) {
        printf("Invalid ID. It must be a positive number.\n");
        return;
    }
    if (findCar(id) != -1) {
        printf("Car ID %d already exists.\n", id);
        return;
    }

    printf("Enter car model: ");
    getText(model, MAX_LEN);
    if (strlen(model) == 0) {
        printf("Model cannot be empty.\n");
        return;
    }

    printf("Enter rate per day: $");
    if (!getNumber(&rate) || rate <= 0) {
        printf("Invalid rate. It must be a positive number.\n");
        return;
    }

    addCar(id, model, rate);
    printf("Car %d (%s) added to the fleet.\n", id, model);
}

void showReport() {
    int rentedCount = 0, expected = 0;

    for (int i = 0; i < carCount; i++) {
        if (cars[i].rented) {
            rentedCount++;
            expected += getCost(cars[i].rate, cars[i].days);
        }
    }

    printf("\n========================================\n");
    printf("               REPORT\n");
    printf("========================================\n");
    printf("Total cars:                %d\n", carCount);
    printf("Rented:                    %d\n", rentedCount);
    printf("Available:                 %d\n", carCount - rentedCount);
    printf("Earned (returned cars):    $%d\n", totalEarnings);
    printf("Expected (active rentals): $%d\n", expected);
    printf("========================================\n");
}

int main() {
    int choice = 0;

    loadCars();
    printf("Car rental system started with %d cars.\n", carCount);

    while (choice != 8) {
        showMenu();

        if (!getNumber(&choice)) {
            printf("Please enter a number from 1 to 8.\n");
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1:
                showAllCars();
                break;
            case 2:
                showAvailableCars();
                break;
            case 3:
                rentCar();
                break;
            case 4:
                returnCar();
                break;
            case 5:
                searchCar();
                break;
            case 6:
                addNewCar();
                break;
            case 7:
                showReport();
                break;
            case 8:
                printf("\nGoodbye!\n");
                break;
            default:
                printf("Invalid choice. Enter a number from 1 to 8.\n");
        }
    }

    return 0;
}
