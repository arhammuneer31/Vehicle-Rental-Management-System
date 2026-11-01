/*
 * VEHICLE RENTAL MANAGEMENT SYSTEM  (Week 11)
 * Programming Fundamentals - FAST BSCS
 *
 * New in this version:
 *  - Vehicle types (Car, SUV, Van, Bike, Bus) with filter and type report
 *  - Data saved to files (vehicles.txt, history.txt) and loaded on start
 *  - Customer phone number + input validation (re-asks until valid)
 *  - Late return charge (extra days cost 150% of the daily rate)
 *  - Rental history with receipt numbers and dates
 *  - Search by ID / model keyword / customer name
 *  - Sort fleet by ID, model or rate
 *  - Detailed report (best vehicle, utilisation, revenue per vehicle)
 *  - Admin panel with password (add / edit / remove vehicles, clear history)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_VEHICLES 50
#define MAX_HISTORY 500
#define MAX_LEN 50
#define PHONE_LEN 20
#define MAX_DAYS 365
#define MAX_RATE 100000
#define DISCOUNT_DAYS 7
#define DISCOUNT_PERCENT 10
#define LATE_FEE_PERCENT 50   /* extra days cost rate + 50% */
#define TYPE_COUNT 5
#define ADMIN_PASSWORD "admin123"
#define VEHICLES_FILE "vehicles.txt"
#define HISTORY_FILE "history.txt"

const char *typeNames[TYPE_COUNT + 1] = {"", "Car", "SUV", "Van", "Bike", "Bus"};

typedef struct {
    int id;
    int type;
    char model[MAX_LEN];
    int rate;
    int rented;
    char customer[MAX_LEN];
    char phone[PHONE_LEN];
    int days;
    int timesRented;
    int revenue;
} Vehicle;

typedef struct {
    int receiptNo;
    int vehicleId;
    char model[MAX_LEN];
    char customer[MAX_LEN];
    char date[11];
    int bookedDays;
    int actualDays;
    int baseCost;
    int lateFee;
    int total;
} Rental;

Vehicle vehicles[MAX_VEHICLES];
int vehicleCount = 0;
Rental history[MAX_HISTORY];
int historyCount = 0;

/* ---------------------------------------------------------------- */
/*                       FILE HANDLING                              */
/* ---------------------------------------------------------------- */

void saveVehicles() {
    FILE *fp = fopen(VEHICLES_FILE, "w");
    if (fp == NULL) {
        printf("Warning: could not save to %s.\n", VEHICLES_FILE);
        return;
    }
    for (int i = 0; i < vehicleCount; i++) {
        fprintf(fp, "%d|%d|%s|%d|%d|%s|%s|%d|%d|%d\n",
                vehicles[i].id, vehicles[i].type, vehicles[i].model, vehicles[i].rate, vehicles[i].rented,
                vehicles[i].customer[0] ? vehicles[i].customer : "-",
                vehicles[i].phone[0] ? vehicles[i].phone : "-",
                vehicles[i].days, vehicles[i].timesRented, vehicles[i].revenue);
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
                history[i].receiptNo, history[i].vehicleId, history[i].model,
                history[i].customer, history[i].date, history[i].bookedDays,
                history[i].actualDays, history[i].baseCost, history[i].lateFee,
                history[i].total);
    }
    fclose(fp);
}

void saveAll() {
    saveVehicles();
    saveHistory();
}

int loadVehicles() {
    FILE *fp = fopen(VEHICLES_FILE, "r");
    char line[256];

    if (fp == NULL) {
        return 0;
    }
    while (vehicleCount < MAX_VEHICLES && fgets(line, sizeof(line), fp) != NULL) {
        Vehicle c;
        int n = sscanf(line, "%d|%d|%49[^|]|%d|%d|%49[^|]|%19[^|]|%d|%d|%d",
                       &c.id, &c.type, c.model, &c.rate, &c.rented, c.customer,
                       c.phone, &c.days, &c.timesRented, &c.revenue);
        if (n == 10 && c.type >= 1 && c.type <= TYPE_COUNT) {
            if (strcmp(c.customer, "-") == 0) c.customer[0] = '\0';
            if (strcmp(c.phone, "-") == 0) c.phone[0] = '\0';
            vehicles[vehicleCount++] = c;
        }
    }
    fclose(fp);
    return vehicleCount;
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
                       &r.receiptNo, &r.vehicleId, r.model, r.customer, r.date,
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
/*                       VEHICLE HELPERS                                */
/* ---------------------------------------------------------------- */

int readType() {
    printf("Vehicle types:\n");
    for (int i = 1; i <= TYPE_COUNT; i++) {
        printf("  %d. %s\n", i, typeNames[i]);
    }
    return readInt("Choose type: ", 1, TYPE_COUNT);
}

void addVehicle(int id, int type, const char model[], int rate) {
    vehicles[vehicleCount].id = id;
    vehicles[vehicleCount].type = type;
    strcpy(vehicles[vehicleCount].model, model);
    vehicles[vehicleCount].rate = rate;
    vehicles[vehicleCount].rented = 0;
    vehicles[vehicleCount].customer[0] = '\0';
    vehicles[vehicleCount].phone[0] = '\0';
    vehicles[vehicleCount].days = 0;
    vehicles[vehicleCount].timesRented = 0;
    vehicles[vehicleCount].revenue = 0;
    vehicleCount++;
}

void loadDefaultVehicles() {
    addVehicle(101, 1, "Sedan Economy", 50);
    addVehicle(102, 1, "Sedan Comfort", 60);
    addVehicle(103, 2, "SUV Compact", 70);
    addVehicle(104, 2, "SUV Premium", 80);
    addVehicle(105, 1, "Luxury Sports", 90);
    addVehicle(106, 1, "Mini Hatchback", 40);
    addVehicle(107, 3, "Van 12-Seater", 100);
    addVehicle(108, 4, "Honda CD70", 15);
    addVehicle(109, 4, "Yamaha YBR 125", 25);
    addVehicle(110, 5, "Coaster 30-Seater", 200);
}

int findVehicle(int id) {
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].id == id) {
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
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].rented) count++;
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
    printf("     VEHICLE RENTAL MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf(" 1. Show all vehicles\n");
    printf(" 2. Show available vehicles\n");
    printf(" 3. Rent a vehicle\n");
    printf(" 4. Return a vehicle\n");
    printf(" 5. Search\n");
    printf(" 6. Sort fleet\n");
    printf(" 7. Rental history\n");
    printf(" 8. Show report\n");
    printf(" 9. Admin panel\n");
    printf(" 0. Save and exit\n");
    printf("========================================\n");
}

void printHeader() {
    printf("\n%-11s %-6s %-20s %-12s %-10s %-6s\n", "Vehicle ID", "Type", "Model",
           "Rate/Day", "Status", "Trips");
    printf("-----------------------------------------------------------------------\n");
}

void printVehicle(int i) {
    printf("%-11d %-6s %-20s Rs %-9d %-10s %-6d\n", vehicles[i].id,
           typeNames[vehicles[i].type], vehicles[i].model,
           vehicles[i].rate, vehicles[i].rented ? "Rented" : "Available",
           vehicles[i].timesRented);
}

void showAllVehicles() {
    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        printVehicle(i);
    }
    printf("-----------------------------------------------------------------------\n");
    printf("%d vehicle(s) in fleet.\n", vehicleCount);
}

void showAvailableVehicles() {
    int count = 0;

    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        if (!vehicles[i].rented) {
            printVehicle(i);
            count++;
        }
    }
    printf("-----------------------------------------------------------------------\n");
    if (count == 0) {
        printf("No vehicles are available right now.\n");
    } else {
        printf("%d vehicle(s) available.\n", count);
    }
}

void printVehicleDetails(int index) {
    printf("\nVehicle ID:       %d\n", vehicles[index].id);
    printf("Type:         %s\n", typeNames[vehicles[index].type]);
    printf("Model:        %s\n", vehicles[index].model);
    printf("Rate:         Rs %d per day\n", vehicles[index].rate);
    printf("Times rented: %d\n", vehicles[index].timesRented);
    if (vehicles[index].rented) {
        printf("Status:       Rented\n");
        printf("Customer:     %s\n", vehicles[index].customer);
        printf("Phone:        %s\n", vehicles[index].phone);
        printf("Booked days:  %d\n", vehicles[index].days);
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
    printf("\n%-6s %-11s %-18s %-16s %-8s %-11s\n", "Rcpt#", "Date", "Vehicle",
           "Customer", "Days", "Total");
    printf("-------------------------------------------------------------------------\n");
}

/* ---------------------------------------------------------------- */
/*                       RENT / RETURN                              */
/* ---------------------------------------------------------------- */

void rentVehicle() {
    int id = 0, index = -1, days = 0, cost = 0;
    char name[MAX_LEN] = "", phone[PHONE_LEN] = "";

    printf("\n--- Rent a Vehicle ---\n");
    if (countRented() == vehicleCount) {
        printf("All vehicles are rented. Please try later.\n");
        return;
    }
    showAvailableVehicles();

    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].rented) {
        printf("Vehicle %d is already rented by %s.\n", id, vehicles[index].customer);
        return;
    }

    readText("Enter customer name: ", name, MAX_LEN);
    readPhone(phone);
    days = readInt("Enter number of days: ", 1, MAX_DAYS);

    cost = getCost(vehicles[index].rate, days);
    printf("\n--- Booking Summary ---\n");
    printf("Vehicle:    %s\n", vehicles[index].model);
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

    vehicles[index].rented = 1;
    vehicles[index].days = days;
    strcpy(vehicles[index].customer, name);
    strcpy(vehicles[index].phone, phone);
    saveVehicles();
    printf("Vehicle %d (%s) rented successfully.\n", id, vehicles[index].model);
}

void returnVehicle() {
    int id = 0, index = -1, actual = 0, base = 0, extra = 0, lateFee = 0;
    int total = 0;
    Rental r;

    printf("\n--- Return a Vehicle ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);

    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (!vehicles[index].rented) {
        printf("Vehicle %d is not rented.\n", id);
        return;
    }

    printf("Rented by %s for %d day(s).\n", vehicles[index].customer,
           vehicles[index].days);
    actual = readInt("Actual number of days the vehicle was kept: ", 1, MAX_DAYS);

    base = getCost(vehicles[index].rate, vehicles[index].days);
    if (actual > vehicles[index].days) {
        extra = actual - vehicles[index].days;
        lateFee = extra * vehicles[index].rate * (100 + LATE_FEE_PERCENT) / 100;
    }
    total = base + lateFee;

    printf("\n----------------------------------------\n");
    printf("RECEIPT #%d\n", nextReceiptNo());
    printf("----------------------------------------\n");
    printf("Vehicle:        %s\n", vehicles[index].model);
    printf("Customer:       %s\n", vehicles[index].customer);
    printf("Phone:          %s\n", vehicles[index].phone);
    printf("Booked days:    %d\n", vehicles[index].days);
    printf("Actual days:    %d\n", actual);
    printf("Rate per day:   Rs %d\n", vehicles[index].rate);
    if (vehicles[index].days >= DISCOUNT_DAYS) {
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
    r.vehicleId = vehicles[index].id;
    strcpy(r.model, vehicles[index].model);
    strcpy(r.customer, vehicles[index].customer);
    getToday(r.date);
    r.bookedDays = vehicles[index].days;
    r.actualDays = actual;
    r.baseCost = base;
    r.lateFee = lateFee;
    r.total = total;
    history[historyCount++] = r;

    vehicles[index].timesRented++;
    vehicles[index].revenue += total;
    vehicles[index].rented = 0;
    vehicles[index].days = 0;
    vehicles[index].customer[0] = '\0';
    vehicles[index].phone[0] = '\0';

    saveAll();
    printf("Vehicle returned successfully.\n");
}

/* ---------------------------------------------------------------- */
/*                       SEARCH / SORT                              */
/* ---------------------------------------------------------------- */

void searchById() {
    int id = readInt("Enter vehicle ID: ", 1, 999999);
    int index = findVehicle(id);

    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    printVehicleDetails(index);
}

void searchByModel() {
    char key[MAX_LEN];
    int found = 0;

    readText("Enter part of the model name: ", key, MAX_LEN);
    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        if (containsIgnoreCase(vehicles[i].model, key)) {
            printVehicle(i);
            found++;
        }
    }
    printf("-----------------------------------------------------------------------\n");
    printf("%d match(es) found.\n", found);
}

void searchByCustomer() {
    char key[MAX_LEN];
    int found = 0;

    readText("Enter customer name (or part of it): ", key, MAX_LEN);

    printf("\nActive rentals:\n");
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].rented && containsIgnoreCase(vehicles[i].customer, key)) {
            printf("  %s has vehicle %d (%s) for %d day(s), phone %s\n",
                   vehicles[i].customer, vehicles[i].id, vehicles[i].model, vehicles[i].days,
                   vehicles[i].phone);
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

void searchByType() {
    int type = readType();
    int found = 0;

    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].type == type) {
            printVehicle(i);
            found++;
        }
    }
    printf("-----------------------------------------------------------------------\n");
    printf("%d %s(s) in fleet.\n", found, typeNames[type]);
}

void searchMenu() {
    printf("\n--- Search ---\n");
    printf("1. By vehicle ID\n2. By model name\n3. By customer name\n4. By vehicle type\n");
    int choice = readInt("Choice (1-4): ", 1, 4);

    if (choice == 1) searchById();
    else if (choice == 2) searchByModel();
    else if (choice == 3) searchByCustomer();
    else searchByType();
}

/* mode: 1 = ID, 2 = model A-Z, 3 = rate low-high, 4 = rate high-low */
int shouldSwap(int a, int b, int mode) {
    switch (mode) {
        case 1: return vehicles[a].id > vehicles[b].id;
        case 2: return strcmp(vehicles[a].model, vehicles[b].model) > 0;
        case 3: return vehicles[a].rate > vehicles[b].rate;
        default: return vehicles[a].rate < vehicles[b].rate;
    }
}

void sortFleet() {
    Vehicle temp;

    printf("\n--- Sort Fleet ---\n");
    printf("1. By ID\n2. By model (A-Z)\n3. By rate (low to high)\n");
    printf("4. By rate (high to low)\n");
    int mode = readInt("Choice (1-4): ", 1, 4);

    for (int i = 0; i < vehicleCount - 1; i++) {
        int best = i;
        for (int j = i + 1; j < vehicleCount; j++) {
            if (shouldSwap(best, j, mode)) {
                best = j;
            }
        }
        if (best != i) {
            temp = vehicles[i];
            vehicles[i] = vehicles[best];
            vehicles[best] = temp;
        }
    }
    saveVehicles();
    showAllVehicles();
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

    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].rented) {
            rentedCount++;
            expected += getCost(vehicles[i].rate, vehicles[i].days);
        }
        if (vehicles[i].timesRented > 0) {
            if (mostRented == -1 || vehicles[i].timesRented > vehicles[mostRented].timesRented) {
                mostRented = i;
            }
            if (topEarner == -1 || vehicles[i].revenue > vehicles[topEarner].revenue) {
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
    printf("Total vehicles:            %d\n", vehicleCount);
    printf("Rented:                    %d\n", rentedCount);
    printf("Available:                 %d\n", vehicleCount - rentedCount);
    if (vehicleCount > 0) {
        printf("Fleet utilisation:         %d%%\n", rentedCount * 100 / vehicleCount);
    }
    printf("Completed rentals:         %d\n", historyCount);
    printf("Earned (returned):         Rs %d\n", earned);
    printf("  of which late charges:   Rs %d\n", lateTotal);
    printf("Expected (active rentals): Rs %d\n", expected);
    if (historyCount > 0) {
        printf("Average per rental:        Rs %d\n", earned / historyCount);
    }
    if (mostRented != -1) {
        printf("Most rented vehicle:       %s (%d trips)\n",
               vehicles[mostRented].model, vehicles[mostRented].timesRented);
        printf("Top earning vehicle:       %s (Rs %d)\n",
               vehicles[topEarner].model, vehicles[topEarner].revenue);
    }
    printf("========================================\n");

    if (vehicleCount > 0) {
        printf("\nFleet by type:\n");
    for (int t = 1; t <= TYPE_COUNT; t++) {
        int total = 0, out = 0, money = 0;
        for (int i = 0; i < vehicleCount; i++) {
            if (vehicles[i].type == t) {
                total++;
                money += vehicles[i].revenue;
                if (vehicles[i].rented) out++;
            }
        }
        if (total > 0) {
            printf("  %-6s %2d vehicle(s), %d rented, revenue Rs %d\n",
                   typeNames[t], total, out, money);
        }
    }

    printf("\nRevenue per vehicle:\n");
        for (int i = 0; i < vehicleCount; i++) {
            printf("  %-4d %-20s %3d trip(s)  Rs %d\n", vehicles[i].id,
                   vehicles[i].model, vehicles[i].timesRented, vehicles[i].revenue);
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

void addNewVehicle() {
    int id = 0, rate = 0, type = 1;
    char model[MAX_LEN] = "";

    printf("\n--- Add a New Vehicle ---\n");
    if (vehicleCount >= MAX_VEHICLES) {
        printf("The fleet is full (max %d vehicles).\n", MAX_VEHICLES);
        return;
    }

    id = readInt("Enter new vehicle ID: ", 1, 999999);
    if (findVehicle(id) != -1) {
        printf("Vehicle ID %d already exists.\n", id);
        return;
    }
    type = readType();
    readText("Enter vehicle model: ", model, MAX_LEN);
    rate = readInt("Enter rate per day: Rs ", 1, MAX_RATE);

    addVehicle(id, type, model, rate);
    saveVehicles();
    printf("%s %d (%s) added to the fleet.\n", typeNames[type], id, model);
}

void editVehicle() {
    int id = 0, index = -1, choice = 0;
    char model[MAX_LEN] = "";

    printf("\n--- Edit a Vehicle ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].rented) {
        printf("Vehicle %d is currently rented. Edit it after it is returned.\n", id);
        return;
    }

    printf("1. Change model name\n2. Change daily rate\n");
    choice = readInt("Choice (1-2): ", 1, 2);
    if (choice == 1) {
        readText("Enter new model name: ", model, MAX_LEN);
        strcpy(vehicles[index].model, model);
    } else {
        vehicles[index].rate = readInt("Enter new rate per day: Rs ", 1, MAX_RATE);
    }
    saveVehicles();
    printf("Vehicle updated.\n");
}

void removeVehicle() {
    int id = 0, index = -1;

    printf("\n--- Remove a Vehicle ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].rented) {
        printf("Vehicle %d is currently rented and cannot be removed.\n", id);
        return;
    }
    printf("About to remove %s.\n", vehicles[index].model);
    if (!readYesNo("Are you sure?")) {
        printf("Cancelled.\n");
        return;
    }
    for (int i = index; i < vehicleCount - 1; i++) {
        vehicles[i] = vehicles[i + 1];
    }
    vehicleCount--;
    saveVehicles();
    printf("Vehicle removed.\n");
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
        printf("1. Add a new vehicle\n2. Edit a vehicle\n3. Remove a vehicle\n");
        printf("4. Clear rental history\n0. Back\n");
        choice = readInt("Choice (0-4): ", 0, 4);
        switch (choice) {
            case 1: addNewVehicle(); break;
            case 2: editVehicle(); break;
            case 3: removeVehicle(); break;
            case 4: clearHistory(); break;
        }
    }
}

/* ---------------------------------------------------------------- */
/*                       MAIN                                       */
/* ---------------------------------------------------------------- */

int main() {
    int choice = -1;

    if (loadVehicles() == 0) {
        loadDefaultVehicles();
        saveVehicles();
        printf("No saved data found. Loaded default fleet.\n");
    }
    loadHistory();
    printf("Vehicle rental system started with %d vehicles and %d past rentals.\n",
           vehicleCount, historyCount);

    while (choice != 0) {
        showMenu();
        choice = readInt("Enter your choice (0-9): ", 0, 9);

        switch (choice) {
            case 1: showAllVehicles(); break;
            case 2: showAvailableVehicles(); break;
            case 3: rentVehicle(); break;
            case 4: returnVehicle(); break;
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

