/*
 * CAR RENTAL MANAGEMENT SYSTEM  (Week 2)
 * Programming Fundamentals - FAST BSCS
 *
 * New in this version:
 *  - Data saved to files (cars.txt, history.txt) and loaded on start
 *  - Customer phone number + input validation (re-asks until valid)
 *  - Late return charge (extra days cost 150% of the daily rate)
 *  - Rental history with receipt numbers and dates
 *  - Search by ID / model keyword / customer name
 *  - Sort fleet by ID, model or rate
 *  - Detailed report (best car, utilisation, revenue per car)
 *  - Admin panel with password (add / edit / remove cars, clear history)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_CARS 50
#define MAX_HISTORY 500
#define MAX_LEN 50
#define PHONE_LEN 20
#define MAX_DAYS 365
#define MAX_RATE 100000
#define DISCOUNT_DAYS 7
#define DISCOUNT_PERCENT 10
#define LATE_FEE_PERCENT 50   /* extra days cost rate + 50% */
#define ADMIN_PASSWORD "admin123"
#define CARS_FILE "cars.txt"
#define HISTORY_FILE "history.txt"

typedef struct {
    int id;
    char model[MAX_LEN];
    int rate;
    int rented;
    char customer[MAX_LEN];
    char phone[PHONE_LEN];
    int days;
    int timesRented;
    int revenue;
} Car;

typedef struct {
    int receiptNo;
    int carId;
    char model[MAX_LEN];
    char customer[MAX_LEN];
    char date[11];
    int bookedDays;
    int actualDays;
    int baseCost;
    int lateFee;
    int total;
} Rental;

Car cars[MAX_CARS];
int carCount = 0;
Rental history[MAX_HISTORY];
int historyCount = 0;

/* ---------------------------------------------------------------- */
/*                       FILE HANDLING                              */
/* ---------------------------------------------------------------- */

void saveCars() {
    FILE *fp = fopen(CARS_FILE, "w");
    if (fp == NULL) {
        printf("Warning: could not save to %s.\n", CARS_FILE);
        return;
    }
    for (int i = 0; i < carCount; i++) {
        fprintf(fp, "%d|%s|%d|%d|%s|%s|%d|%d|%d\n",
                cars[i].id, cars[i].model, cars[i].rate, cars[i].rented,
                cars[i].customer[0] ? cars[i].customer : "-",
                cars[i].phone[0] ? cars[i].phone : "-",
                cars[i].days, cars[i].timesRented, cars[i].revenue);
    }
    fclose(fp);
}

void saveHistory() {
    FILE *fp = fopen(HISTORY_FILE, "w");
    if (fp == NULL) {
        printf("Warning: could not save to %s.\n", HISTORY_FILE);
        return;
    }
    for (int i = 0; i < historyCount; i++) {
        fprintf(fp, "%d|%d|%s|%s|%s|%d|%d|%d|%d|%d\n",
                history[i].receiptNo, history[i].carId, history[i].model,
                history[i].customer, history[i].date, history[i].bookedDays,
                history[i].actualDays, history[i].baseCost, history[i].lateFee,
                history[i].total);
    }
    fclose(fp);
}

void saveAll() {
    saveCars();
    saveHistory();
}

int loadCars() {
    FILE *fp = fopen(CARS_FILE, "r");
    char line[256];

    if (fp == NULL) {
        return 0;
    }
    while (carCount < MAX_CARS && fgets(line, sizeof(line), fp) != NULL) {
        Car c;
        int n = sscanf(line, "%d|%49[^|]|%d|%d|%49[^|]|%19[^|]|%d|%d|%d",
                       &c.id, c.model, &c.rate, &c.rented, c.customer,
                       c.phone, &c.days, &c.timesRented, &c.revenue);
        if (n == 9) {
            if (strcmp(c.customer, "-") == 0) c.customer[0] = '\0';
            if (strcmp(c.phone, "-") == 0) c.phone[0] = '\0';
            cars[carCount++] = c;
        }
    }
    fclose(fp);
    return carCount;
}

void loadHistory() {
    FILE *fp = fopen(HISTORY_FILE, "r");
    char line[256];

    if (fp == NULL) {
        return;
    }
    while (historyCount < MAX_HISTORY && fgets(line, sizeof(line), fp) != NULL) {
        Rental r;
        int n = sscanf(line, "%d|%d|%49[^|]|%49[^|]|%10[^|]|%d|%d|%d|%d|%d",
                       &r.receiptNo, &r.carId, r.model, r.customer, r.date,
                       &r.bookedDays, &r.actualDays, &r.baseCost, &r.lateFee,
                       &r.total);
        if (n == 10) {
            history[historyCount++] = r;
        }
    }
    fclose(fp);
}

/* ---------------------------------------------------------------- */
/*                       INPUT HELPERS                              */
/* ---------------------------------------------------------------- */

void clearBuffer() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exitOnEOF() {
    saveAll();
    printf("\nInput closed. Data saved. Exiting.\n");
    exit(0);
}

/* Keeps asking until the user enters a number between min and max. */
int readInt(const char prompt[], int min, int max) {
    int value = 0, result = 0;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        if (result == EOF) {
            exitOnEOF();
        }
        clearBuffer();
        if (result == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Enter a number from %d to %d.\n", min, max);
    }
}

/* Keeps asking until the user enters non-empty text without '|'. */
void readText(const char prompt[], char text[], int size) {
    int len = 0;

    while (1) {
        printf("%s", prompt);
        if (fgets(text, size, stdin) == NULL) {
            exitOnEOF();
        }
        len = strlen(text);
        if (len > 0 && text[len - 1] == '\n') {
            text[len - 1] = '\0';
        } else {
            clearBuffer();
        }
        if (strlen(text) == 0) {
            printf("This field cannot be empty.\n");
            continue;
        }
        if (strchr(text, '|') != NULL) {
            printf("The '|' character is not allowed.\n");
            continue;
        }
        return;
    }
}

void readPhone(char phone[]) {
    int ok = 0, len = 0;

    while (1) {
        readText("Enter phone number (10-15 digits): ", phone, PHONE_LEN);
        len = strlen(phone);
        ok = 1;
        for (int i = 0; i < len; i++) {
            if (!isdigit((unsigned char)phone[i])) {
                ok = 0;
            }
        }
        if (ok && len >= 10 && len <= 15) {
            return;
        }
        printf("Invalid phone number. Use digits only (10-15).\n");
    }
}

int readYesNo(const char prompt[]) {
    char answer[10];

    while (1) {
        printf("%s (y/n): ", prompt);
        if (fgets(answer, sizeof(answer), stdin) == NULL) {
            exitOnEOF();
        }
        if (answer[0] == 'y' || answer[0] == 'Y') return 1;
        if (answer[0] == 'n' || answer[0] == 'N') return 0;
        printf("Please type y or n.\n");
    }
}

void toLowerStr(const char src[], char dest[]) {
    int i = 0;
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

int containsIgnoreCase(const char text[], const char key[]) {
    char a[MAX_LEN], b[MAX_LEN];
    toLowerStr(text, a);
    toLowerStr(key, b);
    return strstr(a, b) != NULL;
}

void getToday(char out[]) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL) {
        strcpy(out, "unknown");
        return;
    }
    strftime(out, 11, "%Y-%m-%d", t);
}

/* ---------------------------------------------------------------- */
/*                       CAR HELPERS                                */
/* ---------------------------------------------------------------- */

void addCar(int id, const char model[], int rate) {
    cars[carCount].id = id;
    strcpy(cars[carCount].model, model);
    cars[carCount].rate = rate;
    cars[carCount].rented = 0;
    cars[carCount].customer[0] = '\0';
    cars[carCount].phone[0] = '\0';
    cars[carCount].days = 0;
    cars[carCount].timesRented = 0;
    cars[carCount].revenue = 0;
    carCount++;
}

void loadDefaultCars() {
    addCar(101, "Sedan Economy", 50);
    addCar(102, "Sedan Comfort", 60);
    addCar(103, "SUV Compact", 70);
    addCar(104, "SUV Premium", 80);
    addCar(105, "Luxury Sports", 90);
    addCar(106, "Mini Hatchback", 40);
    addCar(107, "Van 12-Seater", 100);
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

int countRented() {
    int count = 0;
    for (int i = 0; i < carCount; i++) {
        if (cars[i].rented) count++;
    }
    return count;
}

int nextReceiptNo() {
    if (historyCount == 0) return 1;
    return history[historyCount - 1].receiptNo + 1;
}

/* ---------------------------------------------------------------- */
/*                       DISPLAY                                    */
/* ---------------------------------------------------------------- */

void showMenu() {
    printf("\n========================================\n");
    printf("     CAR RENTAL MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf(" 1. Show all cars\n");
    printf(" 2. Show available cars\n");
    printf(" 3. Rent a car\n");
    printf(" 4. Return a car\n");
    printf(" 5. Search\n");
    printf(" 6. Sort fleet\n");
    printf(" 7. Rental history\n");
    printf(" 8. Show report\n");
    printf(" 9. Admin panel\n");
    printf(" 0. Save and exit\n");
    printf("========================================\n");
}

void printHeader() {
    printf("\n%-8s %-20s %-12s %-10s %-6s\n", "Car ID", "Model", "Rate/Day",
           "Status", "Trips");
    printf("------------------------------------------------------------\n");
}

void printCar(int i) {
    printf("%-8d %-20s Rs %-9d %-10s %-6d\n", cars[i].id, cars[i].model,
           cars[i].rate, cars[i].rented ? "Rented" : "Available",
           cars[i].timesRented);
}

void showAllCars() {
    printHeader();
    for (int i = 0; i < carCount; i++) {
        printCar(i);
    }
    printf("------------------------------------------------------------\n");
    printf("%d car(s) in fleet.\n", carCount);
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

void printCarDetails(int index) {
    printf("\nCar ID:       %d\n", cars[index].id);
    printf("Model:        %s\n", cars[index].model);
    printf("Rate:         Rs %d per day\n", cars[index].rate);
    printf("Times rented: %d\n", cars[index].timesRented);
    if (cars[index].rented) {
        printf("Status:       Rented\n");
        printf("Customer:     %s\n", cars[index].customer);
        printf("Phone:        %s\n", cars[index].phone);
        printf("Booked days:  %d\n", cars[index].days);
    } else {
        printf("Status:       Available\n");
    }
}

void printHistoryRow(int i) {
    printf("%-6d %-11s %-18s %-16s %3d/%-3d  Rs %-8d\n", history[i].receiptNo,
           history[i].date, history[i].model, history[i].customer,
           history[i].bookedDays, history[i].actualDays, history[i].total);
}

void printHistoryHeader() {
    printf("\n%-6s %-11s %-18s %-16s %-8s %-11s\n", "Rcpt#", "Date", "Car",
           "Customer", "Days", "Total");
    printf("-------------------------------------------------------------------------\n");
}

/* ---------------------------------------------------------------- */
/*                       RENT / RETURN                              */
/* ---------------------------------------------------------------- */

void rentCar() {
    int id = 0, index = -1, days = 0, cost = 0;
    char name[MAX_LEN] = "", phone[PHONE_LEN] = "";

    printf("\n--- Rent a Car ---\n");
    if (countRented() == carCount) {
        printf("All cars are rented. Please try later.\n");
        return;
    }
    showAvailableCars();

    id = readInt("Enter car ID: ", 1, 999999);
    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    if (cars[index].rented) {
        printf("Car %d is already rented by %s.\n", id, cars[index].customer);
        return;
    }

    readText("Enter customer name: ", name, MAX_LEN);
    readPhone(phone);
    days = readInt("Enter number of days: ", 1, MAX_DAYS);

    cost = getCost(cars[index].rate, days);
    printf("\n--- Booking Summary ---\n");
    printf("Car:        %s\n", cars[index].model);
    printf("Customer:   %s (%s)\n", name, phone);
    printf("Days:       %d\n", days);
    if (days >= DISCOUNT_DAYS) {
        printf("Discount:   %d%% applied for long rental\n", DISCOUNT_PERCENT);
    }
    printf("Est. cost:  Rs %d\n", cost);
    printf("Late days are charged at %d%% of the daily rate.\n",
           100 + LATE_FEE_PERCENT);

    if (!readYesNo("Confirm booking?")) {
        printf("Booking cancelled.\n");
        return;
    }

    cars[index].rented = 1;
    cars[index].days = days;
    strcpy(cars[index].customer, name);
    strcpy(cars[index].phone, phone);
    saveCars();
    printf("Car %d (%s) rented successfully.\n", id, cars[index].model);
}

void returnCar() {
    int id = 0, index = -1, actual = 0, base = 0, extra = 0, lateFee = 0;
    int total = 0;
    Rental r;

    printf("\n--- Return a Car ---\n");
    id = readInt("Enter car ID: ", 1, 999999);

    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    if (!cars[index].rented) {
        printf("Car %d is not rented.\n", id);
        return;
    }

    printf("Rented by %s for %d day(s).\n", cars[index].customer,
           cars[index].days);
    actual = readInt("Actual number of days the car was kept: ", 1, MAX_DAYS);

    base = getCost(cars[index].rate, cars[index].days);
    if (actual > cars[index].days) {
        extra = actual - cars[index].days;
        lateFee = extra * cars[index].rate * (100 + LATE_FEE_PERCENT) / 100;
    }
    total = base + lateFee;

    printf("\n----------------------------------------\n");
    printf("RECEIPT #%d\n", nextReceiptNo());
    printf("----------------------------------------\n");
    printf("Car:            %s\n", cars[index].model);
    printf("Customer:       %s\n", cars[index].customer);
    printf("Phone:          %s\n", cars[index].phone);
    printf("Booked days:    %d\n", cars[index].days);
    printf("Actual days:    %d\n", actual);
    printf("Rate per day:   Rs %d\n", cars[index].rate);
    if (cars[index].days >= DISCOUNT_DAYS) {
        printf("Discount:       %d%%\n", DISCOUNT_PERCENT);
    }
    printf("Rental cost:    Rs %d\n", base);
    if (extra > 0) {
        printf("Late (%d day%s): Rs %d\n", extra, extra > 1 ? "s" : "", lateFee);
    }
    printf("TOTAL:          Rs %d\n", total);
    printf("----------------------------------------\n");

    /* save record in history (drop the oldest one if full) */
    if (historyCount >= MAX_HISTORY) {
        memmove(&history[0], &history[1], sizeof(Rental) * (MAX_HISTORY - 1));
        historyCount--;
    }
    r.receiptNo = nextReceiptNo();
    r.carId = cars[index].id;
    strcpy(r.model, cars[index].model);
    strcpy(r.customer, cars[index].customer);
    getToday(r.date);
    r.bookedDays = cars[index].days;
    r.actualDays = actual;
    r.baseCost = base;
    r.lateFee = lateFee;
    r.total = total;
    history[historyCount++] = r;

    cars[index].timesRented++;
    cars[index].revenue += total;
    cars[index].rented = 0;
    cars[index].days = 0;
    cars[index].customer[0] = '\0';
    cars[index].phone[0] = '\0';

    saveAll();
    printf("Car returned successfully.\n");
}

/* ---------------------------------------------------------------- */
/*                       SEARCH / SORT                              */
/* ---------------------------------------------------------------- */

void searchById() {
    int id = readInt("Enter car ID: ", 1, 999999);
    int index = findCar(id);

    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    printCarDetails(index);
}

void searchByModel() {
    char key[MAX_LEN];
    int found = 0;

    readText("Enter part of the model name: ", key, MAX_LEN);
    printHeader();
    for (int i = 0; i < carCount; i++) {
        if (containsIgnoreCase(cars[i].model, key)) {
            printCar(i);
            found++;
        }
    }
    printf("------------------------------------------------------------\n");
    printf("%d match(es) found.\n", found);
}

void searchByCustomer() {
    char key[MAX_LEN];
    int found = 0;

    readText("Enter customer name (or part of it): ", key, MAX_LEN);

    printf("\nActive rentals:\n");
    for (int i = 0; i < carCount; i++) {
        if (cars[i].rented && containsIgnoreCase(cars[i].customer, key)) {
            printf("  %s has car %d (%s) for %d day(s), phone %s\n",
                   cars[i].customer, cars[i].id, cars[i].model, cars[i].days,
                   cars[i].phone);
            found++;
        }
    }
    if (found == 0) printf("  None.\n");

    printf("\nPast rentals:\n");
    found = 0;
    for (int i = 0; i < historyCount; i++) {
        if (containsIgnoreCase(history[i].customer, key)) {
            if (found == 0) printHistoryHeader();
            printHistoryRow(i);
            found++;
        }
    }
    if (found == 0) printf("  None.\n");
}

void searchMenu() {
    printf("\n--- Search ---\n");
    printf("1. By car ID\n2. By model name\n3. By customer name\n");
    int choice = readInt("Choice (1-3): ", 1, 3);

    if (choice == 1) searchById();
    else if (choice == 2) searchByModel();
    else searchByCustomer();
}

/* mode: 1 = ID, 2 = model A-Z, 3 = rate low-high, 4 = rate high-low */
int shouldSwap(int a, int b, int mode) {
    switch (mode) {
        case 1: return cars[a].id > cars[b].id;
        case 2: return strcmp(cars[a].model, cars[b].model) > 0;
        case 3: return cars[a].rate > cars[b].rate;
        default: return cars[a].rate < cars[b].rate;
    }
}

void sortFleet() {
    Car temp;

    printf("\n--- Sort Fleet ---\n");
    printf("1. By ID\n2. By model (A-Z)\n3. By rate (low to high)\n");
    printf("4. By rate (high to low)\n");
    int mode = readInt("Choice (1-4): ", 1, 4);

    for (int i = 0; i < carCount - 1; i++) {
        int best = i;
        for (int j = i + 1; j < carCount; j++) {
            if (shouldSwap(best, j, mode)) {
                best = j;
            }
        }
        if (best != i) {
            temp = cars[i];
            cars[i] = cars[best];
            cars[best] = temp;
        }
    }
    saveCars();
    showAllCars();
}

/* ---------------------------------------------------------------- */
/*                       HISTORY / REPORT                           */
/* ---------------------------------------------------------------- */

void showHistory() {
    printf("\n--- Rental History ---\n");
    if (historyCount == 0) {
        printf("No completed rentals yet.\n");
        return;
    }
    printHistoryHeader();
    for (int i = 0; i < historyCount; i++) {
        printHistoryRow(i);
    }
    printf("-------------------------------------------------------------------------\n");
    printf("%d rental(s) recorded. (Days column = booked/actual)\n", historyCount);
}

void showReport() {
    int rentedCount = 0, expected = 0, earned = 0, lateTotal = 0;
    int mostRented = -1, topEarner = -1;

    for (int i = 0; i < carCount; i++) {
        if (cars[i].rented) {
            rentedCount++;
            expected += getCost(cars[i].rate, cars[i].days);
        }
        if (cars[i].timesRented > 0) {
            if (mostRented == -1 || cars[i].timesRented > cars[mostRented].timesRented) {
                mostRented = i;
            }
            if (topEarner == -1 || cars[i].revenue > cars[topEarner].revenue) {
                topEarner = i;
            }
        }
    }
    for (int i = 0; i < historyCount; i++) {
        earned += history[i].total;
        lateTotal += history[i].lateFee;
    }

    printf("\n========================================\n");
    printf("               REPORT\n");
    printf("========================================\n");
    printf("Total cars:                %d\n", carCount);
    printf("Rented:                    %d\n", rentedCount);
    printf("Available:                 %d\n", carCount - rentedCount);
    if (carCount > 0) {
        printf("Fleet utilisation:         %d%%\n", rentedCount * 100 / carCount);
    }
    printf("Completed rentals:         %d\n", historyCount);
    printf("Earned (returned cars):    Rs %d\n", earned);
    printf("  of which late charges:   Rs %d\n", lateTotal);
    printf("Expected (active rentals): Rs %d\n", expected);
    if (historyCount > 0) {
        printf("Average per rental:        Rs %d\n", earned / historyCount);
    }
    if (mostRented != -1) {
        printf("Most rented car:           %s (%d trips)\n",
               cars[mostRented].model, cars[mostRented].timesRented);
        printf("Top earning car:           %s (Rs %d)\n",
               cars[topEarner].model, cars[topEarner].revenue);
    }
    printf("========================================\n");

    if (carCount > 0) {
        printf("\nRevenue per car:\n");
        for (int i = 0; i < carCount; i++) {
            printf("  %-4d %-20s %3d trip(s)  Rs %d\n", cars[i].id,
                   cars[i].model, cars[i].timesRented, cars[i].revenue);
        }
    }
}

/* ---------------------------------------------------------------- */
/*                       ADMIN PANEL                                */
/* ---------------------------------------------------------------- */

int adminLogin() {
    char pass[MAX_LEN];

    for (int attempt = 1; attempt <= 3; attempt++) {
        readText("Enter admin password: ", pass, MAX_LEN);
        if (strcmp(pass, ADMIN_PASSWORD) == 0) {
            return 1;
        }
        printf("Wrong password (%d/3).\n", attempt);
    }
    printf("Too many wrong attempts. Access denied.\n");
    return 0;
}

void addNewCar() {
    int id = 0, rate = 0;
    char model[MAX_LEN] = "";

    printf("\n--- Add a New Car ---\n");
    if (carCount >= MAX_CARS) {
        printf("The fleet is full (max %d cars).\n", MAX_CARS);
        return;
    }

    id = readInt("Enter new car ID: ", 1, 999999);
    if (findCar(id) != -1) {
        printf("Car ID %d already exists.\n", id);
        return;
    }
    readText("Enter car model: ", model, MAX_LEN);
    rate = readInt("Enter rate per day: Rs ", 1, MAX_RATE);

    addCar(id, model, rate);
    saveCars();
    printf("Car %d (%s) added to the fleet.\n", id, model);
}

void editCar() {
    int id = 0, index = -1, choice = 0;
    char model[MAX_LEN] = "";

    printf("\n--- Edit a Car ---\n");
    id = readInt("Enter car ID: ", 1, 999999);
    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    if (cars[index].rented) {
        printf("Car %d is currently rented. Edit it after it is returned.\n", id);
        return;
    }

    printf("1. Change model name\n2. Change daily rate\n");
    choice = readInt("Choice (1-2): ", 1, 2);
    if (choice == 1) {
        readText("Enter new model name: ", model, MAX_LEN);
        strcpy(cars[index].model, model);
    } else {
        cars[index].rate = readInt("Enter new rate per day: Rs ", 1, MAX_RATE);
    }
    saveCars();
    printf("Car updated.\n");
}

void removeCar() {
    int id = 0, index = -1;

    printf("\n--- Remove a Car ---\n");
    id = readInt("Enter car ID: ", 1, 999999);
    index = findCar(id);
    if (index == -1) {
        printf("Car %d was not found.\n", id);
        return;
    }
    if (cars[index].rented) {
        printf("Car %d is currently rented and cannot be removed.\n", id);
        return;
    }
    printf("About to remove %s.\n", cars[index].model);
    if (!readYesNo("Are you sure?")) {
        printf("Cancelled.\n");
        return;
    }
    for (int i = index; i < carCount - 1; i++) {
        cars[i] = cars[i + 1];
    }
    carCount--;
    saveCars();
    printf("Car removed.\n");
}

void clearHistory() {
    printf("\nThis will delete all %d history record(s).\n", historyCount);
    if (!readYesNo("Are you sure?")) {
        printf("Cancelled.\n");
        return;
    }
    historyCount = 0;
    saveHistory();
    printf("History cleared.\n");
}

void adminPanel() {
    int choice = -1;

    printf("\n--- Admin Panel ---\n");
    if (!adminLogin()) {
        return;
    }
    while (choice != 0) {
        printf("\n---- ADMIN ----\n");
        printf("1. Add a new car\n2. Edit a car\n3. Remove a car\n");
        printf("4. Clear rental history\n0. Back\n");
        choice = readInt("Choice (0-4): ", 0, 4);
        switch (choice) {
            case 1: addNewCar(); break;
            case 2: editCar(); break;
            case 3: removeCar(); break;
            case 4: clearHistory(); break;
        }
    }
}

/* ---------------------------------------------------------------- */
/*                       MAIN                                       */
/* ---------------------------------------------------------------- */

int main() {
    int choice = -1;

    if (loadCars() == 0) {
        loadDefaultCars();
        saveCars();
        printf("No saved data found. Loaded default fleet.\n");
    }
    loadHistory();
    printf("Car rental system started with %d cars and %d past rentals.\n",
           carCount, historyCount);

    while (choice != 0) {
        showMenu();
        choice = readInt("Enter your choice (0-9): ", 0, 9);

        switch (choice) {
            case 1: showAllCars(); break;
            case 2: showAvailableCars(); break;
            case 3: rentCar(); break;
            case 4: returnCar(); break;
            case 5: searchMenu(); break;
            case 6: sortFleet(); break;
            case 7: showHistory(); break;
            case 8: showReport(); break;
            case 9: adminPanel(); break;
            case 0:
                saveAll();
                printf("\nData saved. Goodbye!\n");
                break;
        }
    }
    return 0;
}