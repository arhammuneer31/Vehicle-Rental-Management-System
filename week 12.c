/*
 * VEHICLE RENTAL MANAGEMENT SYSTEM  (Week 12)
 * Programming Fundamentals - FAST BSCS
 *
 * New in this version (Week 12):
 *  - Customer records (phone, CNIC), auto-registered at first rental
 *  - Loyalty discount for repeat customers + blacklist option
 *  - Rental dates, due dates, overdue alerts, "Active rentals" screen
 *  - Days can be calculated from the system clock on return
 *  - Extend a running rental
 *  - Refundable security deposit, 5% service tax, damage and fuel charges
 *  - Vehicle status: Available / Rented / Maintenance
 *  - Customer rating (1-5) for every vehicle
 *  - Search by budget; fleet, customer and report sub-menus
 *  - Receipts saved to receipts.txt, report can be saved to report.txt
 *  - Admin can change the password (stored in admin.txt)
 *
 * From earlier weeks: vehicle types, file saving, history, late fees,
 * search, sort, admin panel and input validation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_VEHICLES 50
#define MAX_CUSTOMERS 200
#define MAX_HISTORY 500
#define MAX_LEN 50
#define PHONE_LEN 20
#define CNIC_LEN 20
#define MAX_DAYS 365
#define MAX_RATE 100000
#define MAX_EXTRA_CHARGE 1000000
#define DISCOUNT_DAYS 7
#define DISCOUNT_PERCENT 10
#define LOYALTY_RENTALS 3      /* completed rentals needed for loyalty discount */
#define LOYALTY_PERCENT 5
#define LATE_FEE_PERCENT 50    /* late days cost rate + 50% */
#define TAX_PERCENT 5
#define DEPOSIT_PERCENT 20
#define MAX_ACTIVE_PER_CUSTOMER 2
#define SECONDS_PER_DAY 86400
#define TYPE_COUNT 5
#define DEFAULT_PASSWORD "admin123"

#define VEHICLES_FILE "vehicles.txt"
#define CUSTOMERS_FILE "customers.txt"
#define HISTORY_FILE "history.txt"
#define RECEIPTS_FILE "receipts.txt"
#define REPORT_FILE "report.txt"
#define PASSWORD_FILE "admin.txt"

#define AVAILABLE 0
#define RENTED 1
#define MAINTENANCE 2

const char *typeNames[TYPE_COUNT + 1] = {"", "Car", "SUV", "Van", "Bike", "Bus"};
const char *statusNames[3] = {"Available", "Rented", "Maintenance"};

typedef struct {
    int id;
    int type;
    char model[MAX_LEN];
    int rate;
    int status;
    int customerId;      /* who has it right now (0 = nobody) */
    int days;            /* booked days */
    time_t rentTime;     /* when it was rented */
    int deposit;
    int timesRented;
    int revenue;
    int ratingSum;
    int ratingCount;
} Vehicle;

typedef struct {
    int id;
    char name[MAX_LEN];
    char phone[PHONE_LEN];
    char cnic[CNIC_LEN];
    int rentals;
    int spent;
    int blacklisted;
} Customer;

typedef struct {
    int receiptNo;
    int vehicleId;
    char model[MAX_LEN];
    int customerId;
    char customer[MAX_LEN];
    char date[11];
    int rate;
    int bookedDays;
    int actualDays;
    int baseCost;
    int loyalty;
    int lateFee;
    int damage;
    int fuel;
    int tax;
    int deposit;
    int total;
} Rental;

Vehicle vehicles[MAX_VEHICLES];
int vehicleCount = 0;
Customer customers[MAX_CUSTOMERS];
int customerCount = 0;
Rental history[MAX_HISTORY];
int historyCount = 0;
char adminPassword[MAX_LEN] = DEFAULT_PASSWORD;

/* ================================================================ */
/*                          FILE HANDLING                           */
/* ================================================================ */

void saveVehicles() {
    FILE *fp = fopen(VEHICLES_FILE, "w");
    if (fp == NULL) {
        printf("Warning: could not save to %s.\n", VEHICLES_FILE);
        return;
    }
    for (int i = 0; i < vehicleCount; i++) {
        fprintf(fp, "%d|%d|%s|%d|%d|%d|%d|%ld|%d|%d|%d|%d|%d\n",
                vehicles[i].id, vehicles[i].type, vehicles[i].model,
                vehicles[i].rate, vehicles[i].status, vehicles[i].customerId,
                vehicles[i].days, (long)vehicles[i].rentTime,
                vehicles[i].deposit, vehicles[i].timesRented,
                vehicles[i].revenue, vehicles[i].ratingSum,
                vehicles[i].ratingCount);
    }
    fclose(fp);
}

void saveCustomers() {
    FILE *fp = fopen(CUSTOMERS_FILE, "w");
    if (fp == NULL) {
        printf("Warning: could not save to %s.\n", CUSTOMERS_FILE);
        return;
    }
    for (int i = 0; i < customerCount; i++) {
        fprintf(fp, "%d|%s|%s|%s|%d|%d|%d\n", customers[i].id,
                customers[i].name, customers[i].phone, customers[i].cnic,
                customers[i].rentals, customers[i].spent,
                customers[i].blacklisted);
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
        fprintf(fp, "%d|%d|%s|%d|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
                history[i].receiptNo, history[i].vehicleId, history[i].model,
                history[i].customerId, history[i].customer, history[i].date,
                history[i].rate, history[i].bookedDays, history[i].actualDays,
                history[i].baseCost, history[i].loyalty, history[i].lateFee,
                history[i].damage, history[i].fuel, history[i].tax,
                history[i].deposit, history[i].total);
    }
    fclose(fp);
}

void savePassword() {
    FILE *fp = fopen(PASSWORD_FILE, "w");
    if (fp == NULL) {
        printf("Warning: could not save the password.\n");
        return;
    }
    fprintf(fp, "%s\n", adminPassword);
    fclose(fp);
}

void saveAll() {
    saveVehicles();
    saveCustomers();
    saveHistory();
}

int loadVehicles() {
    FILE *fp = fopen(VEHICLES_FILE, "r");
    char line[256];

    if (fp == NULL) {
        return 0;
    }
    while (vehicleCount < MAX_VEHICLES && fgets(line, sizeof(line), fp) != NULL) {
        Vehicle v;
        long t = 0;
        int n = sscanf(line, "%d|%d|%49[^|]|%d|%d|%d|%d|%ld|%d|%d|%d|%d|%d",
                       &v.id, &v.type, v.model, &v.rate, &v.status,
                       &v.customerId, &v.days, &t, &v.deposit, &v.timesRented,
                       &v.revenue, &v.ratingSum, &v.ratingCount);
        if (n == 13 && v.type >= 1 && v.type <= TYPE_COUNT &&
            v.status >= 0 && v.status <= 2) {
            v.rentTime = (time_t)t;
            vehicles[vehicleCount++] = v;
        }
    }
    fclose(fp);
    return vehicleCount;
}

void loadCustomers() {
    FILE *fp = fopen(CUSTOMERS_FILE, "r");
    char line[256];

    if (fp == NULL) {
        return;
    }
    while (customerCount < MAX_CUSTOMERS && fgets(line, sizeof(line), fp) != NULL) {
        Customer c;
        int n = sscanf(line, "%d|%49[^|]|%19[^|]|%19[^|]|%d|%d|%d", &c.id,
                       c.name, c.phone, c.cnic, &c.rentals, &c.spent,
                       &c.blacklisted);
        if (n == 7) {
            customers[customerCount++] = c;
        }
    }
    fclose(fp);
}

void loadHistory() {
    FILE *fp = fopen(HISTORY_FILE, "r");
    char line[400];

    if (fp == NULL) {
        return;
    }
    while (historyCount < MAX_HISTORY && fgets(line, sizeof(line), fp) != NULL) {
        Rental r;
        int n = sscanf(line,
                       "%d|%d|%49[^|]|%d|%49[^|]|%10[^|]|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
                       &r.receiptNo, &r.vehicleId, r.model, &r.customerId,
                       r.customer, r.date, &r.rate, &r.bookedDays,
                       &r.actualDays, &r.baseCost, &r.loyalty, &r.lateFee,
                       &r.damage, &r.fuel, &r.tax, &r.deposit, &r.total);
        if (n == 17) {
            history[historyCount++] = r;
        }
    }
    fclose(fp);
}

void loadPassword() {
    FILE *fp = fopen(PASSWORD_FILE, "r");
    char line[MAX_LEN];
    int len = 0;

    if (fp == NULL) {
        return;
    }
    if (fgets(line, sizeof(line), fp) != NULL) {
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (strlen(line) > 0) {
            strcpy(adminPassword, line);
        }
    }
    fclose(fp);
}

/* ================================================================ */
/*                          INPUT HELPERS                           */
/* ================================================================ */

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

/* Removes spaces from the start and end of a string. */
void trim(char s[]) {
    int start = 0, end = strlen(s) - 1, i = 0;

    while (s[start] != '\0' && isspace((unsigned char)s[start])) {
        start++;
    }
    while (end >= start && isspace((unsigned char)s[end])) {
        end--;
    }
    for (i = 0; start + i <= end; i++) {
        s[i] = s[start + i];
    }
    s[i] = '\0';
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
        trim(text);
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

/* Asks for digits only, with a length between minLen and maxLen. */
void readDigits(const char prompt[], char text[], int size, int minLen, int maxLen) {
    int ok = 0, len = 0;

    while (1) {
        readText(prompt, text, size);
        len = strlen(text);
        ok = 1;
        for (int i = 0; i < len; i++) {
            if (!isdigit((unsigned char)text[i])) {
                ok = 0;
            }
        }
        if (ok && len >= minLen && len <= maxLen) {
            return;
        }
        printf("Invalid input. Use digits only (%d-%d digits).\n", minLen, maxLen);
    }
}

void readPhone(char phone[]) {
    readDigits("Enter phone number (10-15 digits): ", phone, PHONE_LEN, 10, 15);
}

void readCnic(char cnic[]) {
    readDigits("Enter CNIC (13 digits, no dashes): ", cnic, CNIC_LEN, 13, 13);
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

void printLine(int length, char ch) {
    for (int i = 0; i < length; i++) {
        putchar(ch);
    }
    putchar('\n');
}

/* ================================================================ */
/*                          DATE HELPERS                            */
/* ================================================================ */

void formatDate(time_t t, char out[]) {
    struct tm *info = localtime(&t);
    if (info == NULL) {
        strcpy(out, "unknown");
        return;
    }
    strftime(out, 11, "%Y-%m-%d", info);
}

void getToday(char out[]) {
    formatDate(time(NULL), out);
}

time_t dueTime(int i) {
    return vehicles[i].rentTime + (time_t)vehicles[i].days * SECONDS_PER_DAY;
}

int isOverdue(int i) {
    return vehicles[i].status == RENTED && difftime(time(NULL), dueTime(i)) > 0;
}

/* Whole days since the vehicle was rented (a started day counts), min 1. */
int elapsedDays(time_t start) {
    double seconds = difftime(time(NULL), start);
    int days = (int)(seconds / SECONDS_PER_DAY);

    if (seconds - (double)days * SECONDS_PER_DAY > 0) {
        days++;
    }
    if (days < 1) days = 1;
    if (days > MAX_DAYS) days = MAX_DAYS;
    return days;
}

/* ================================================================ */
/*                  VEHICLE / CUSTOMER HELPERS                      */
/* ================================================================ */

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
    vehicles[vehicleCount].status = AVAILABLE;
    vehicles[vehicleCount].customerId = 0;
    vehicles[vehicleCount].days = 0;
    vehicles[vehicleCount].rentTime = 0;
    vehicles[vehicleCount].deposit = 0;
    vehicles[vehicleCount].timesRented = 0;
    vehicles[vehicleCount].revenue = 0;
    vehicles[vehicleCount].ratingSum = 0;
    vehicles[vehicleCount].ratingCount = 0;
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

int findCustomerById(int id) {
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].id == id) {
            return i;
        }
    }
    return -1;
}

int findCustomerByPhone(const char phone[]) {
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].phone, phone) == 0) {
            return i;
        }
    }
    return -1;
}

/* Returns the customer's name, or "Unknown" if the record is missing. */
const char *customerName(int id) {
    int index = findCustomerById(id);
    if (index == -1) return "Unknown";
    return customers[index].name;
}

int countByStatus(int status) {
    int count = 0;
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].status == status) count++;
    }
    return count;
}

int countOverdue() {
    int count = 0;
    for (int i = 0; i < vehicleCount; i++) {
        if (isOverdue(i)) count++;
    }
    return count;
}

int countActiveRentals(int customerId) {
    int count = 0;
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].status == RENTED && vehicles[i].customerId == customerId) {
            count++;
        }
    }
    return count;
}

int nextReceiptNo() {
    if (historyCount == 0) return 1;
    return history[historyCount - 1].receiptNo + 1;
}

int nextCustomerId() {
    int max = 1000;
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].id > max) max = customers[i].id;
    }
    return max + 1;
}

/* ================================================================ */
/*                          BILLING                                 */
/* ================================================================ */

/* Rental cost with the long-rental discount. */
int getCost(int rate, int days) {
    int cost = rate * days;
    if (days >= DISCOUNT_DAYS) {
        cost = cost - cost * DISCOUNT_PERCENT / 100;
    }
    return cost;
}

/* Extra discount for customers who have rented before. */
int getLoyaltyDiscount(int cost, int customerIndex) {
    if (customerIndex != -1 && customers[customerIndex].rentals >= LOYALTY_RENTALS) {
        return cost * LOYALTY_PERCENT / 100;
    }
    return 0;
}

/* ================================================================ */
/*                          DISPLAY                                 */
/* ================================================================ */

void printHeader() {
    printf("\n%-11s %-6s %-20s %-12s %-12s %-6s %-6s\n", "Vehicle ID", "Type",
           "Model", "Rate/Day", "Status", "Trips", "Rating");
    printLine(79, '-');
}

void printVehicle(int i) {
    char rating[10];

    if (vehicles[i].ratingCount == 0) {
        strcpy(rating, "-");
    } else {
        sprintf(rating, "%.1f", (double)vehicles[i].ratingSum / vehicles[i].ratingCount);
    }
    printf("%-11d %-6s %-20.20s Rs %-9d %-12s %-6d %-6s\n", vehicles[i].id,
           typeNames[vehicles[i].type], vehicles[i].model, vehicles[i].rate,
           statusNames[vehicles[i].status], vehicles[i].timesRented, rating);
}

void showAllVehicles() {
    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        printVehicle(i);
    }
    printLine(79, '-');
    printf("%d vehicle(s) in fleet.\n", vehicleCount);
}

void showAvailableVehicles() {
    int count = 0;

    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].status == AVAILABLE) {
            printVehicle(i);
            count++;
        }
    }
    printLine(79, '-');
    if (count == 0) {
        printf("No vehicles are available right now.\n");
    } else {
        printf("%d vehicle(s) available.\n", count);
    }
}

void printVehicleDetails(int index) {
    char rented[11], due[11];

    printf("\nVehicle ID:   %d\n", vehicles[index].id);
    printf("Type:         %s\n", typeNames[vehicles[index].type]);
    printf("Model:        %s\n", vehicles[index].model);
    printf("Rate:         Rs %d per day\n", vehicles[index].rate);
    printf("Times rented: %d\n", vehicles[index].timesRented);
    if (vehicles[index].ratingCount > 0) {
        printf("Rating:       %.1f / 5 (%d review(s))\n",
               (double)vehicles[index].ratingSum / vehicles[index].ratingCount,
               vehicles[index].ratingCount);
    } else {
        printf("Rating:       not rated yet\n");
    }
    printf("Status:       %s\n", statusNames[vehicles[index].status]);
    if (vehicles[index].status == RENTED) {
        formatDate(vehicles[index].rentTime, rented);
        formatDate(dueTime(index), due);
        printf("Customer:     %s\n", customerName(vehicles[index].customerId));
        printf("Rented on:    %s\n", rented);
        printf("Due on:       %s (%d day booking)\n", due, vehicles[index].days);
        if (isOverdue(index)) {
            printf("*** THIS RENTAL IS OVERDUE ***\n");
        }
    }
}

void printHistoryHeader() {
    printf("\n%-6s %-11s %-18s %-16s %-8s %-11s\n", "Rcpt#", "Date", "Vehicle",
           "Customer", "Days", "Total");
    printLine(75, '-');
}

void printHistoryRow(int i) {
    printf("%-6d %-11s %-18.18s %-16.16s %3d/%-3d  Rs %-8d\n",
           history[i].receiptNo, history[i].date, history[i].model,
           history[i].customer, history[i].bookedDays, history[i].actualDays,
           history[i].total);
}

void printCustomerHeader() {
    printf("\n%-6s %-18s %-14s %-14s %-8s %-10s %-11s\n", "ID", "Name", "Phone",
           "CNIC", "Rentals", "Spent", "Status");
    printLine(87, '-');
}

void printCustomer(int i) {
    printf("%-6d %-18.18s %-14s %-14s %-8d Rs %-7d %-11s\n", customers[i].id,
           customers[i].name, customers[i].phone, customers[i].cnic,
           customers[i].rentals, customers[i].spent,
           customers[i].blacklisted ? "BLACKLISTED" : "Active");
}

/* Works for the screen (stdout) and for files, so one function does both. */
void printReceipt(FILE *out, const Rental *r) {
    int balance = r->total - r->deposit;

    fprintf(out, "\n----------------------------------------\n");
    fprintf(out, "RECEIPT #%d\n", r->receiptNo);
    fprintf(out, "----------------------------------------\n");
    fprintf(out, "Date:            %s\n", r->date);
    fprintf(out, "Vehicle:         %s (ID %d)\n", r->model, r->vehicleId);
    fprintf(out, "Customer:        %s\n", r->customer);
    fprintf(out, "Booked days:     %d\n", r->bookedDays);
    fprintf(out, "Actual days:     %d\n", r->actualDays);
    fprintf(out, "Rate per day:    Rs %d\n", r->rate);
    fprintf(out, "Rental cost:     Rs %d", r->baseCost);
    if (r->bookedDays >= DISCOUNT_DAYS) {
        fprintf(out, "  (%d%% long-rental discount incl.)", DISCOUNT_PERCENT);
    }
    fprintf(out, "\n");
    if (r->loyalty > 0) {
        fprintf(out, "Loyalty (%d%%):    -Rs %d\n", LOYALTY_PERCENT, r->loyalty);
    }
    if (r->lateFee > 0) {
        fprintf(out, "Late charge:     Rs %d\n", r->lateFee);
    }
    fprintf(out, "Service tax %d%%:  Rs %d\n", TAX_PERCENT, r->tax);
    if (r->damage > 0) {
        fprintf(out, "Damage charge:   Rs %d\n", r->damage);
    }
    if (r->fuel > 0) {
        fprintf(out, "Fuel charge:     Rs %d\n", r->fuel);
    }
    fprintf(out, "TOTAL:           Rs %d\n", r->total);
    fprintf(out, "Deposit paid:    Rs %d\n", r->deposit);
    if (balance >= 0) {
        fprintf(out, "BALANCE TO PAY:  Rs %d\n", balance);
    } else {
        fprintf(out, "REFUND DUE:      Rs %d\n", -balance);
    }
    fprintf(out, "----------------------------------------\n");
}

void saveReceipt(const Rental *r) {
    FILE *fp = fopen(RECEIPTS_FILE, "a");
    if (fp == NULL) {
        printf("Warning: could not write to %s.\n", RECEIPTS_FILE);
        return;
    }
    printReceipt(fp, r);
    fclose(fp);
}

/* ================================================================ */
/*                          CUSTOMERS                               */
/* ================================================================ */

/* Registers a new customer with the given phone. Returns index or -1. */
int registerCustomer(const char phone[]) {
    Customer c;

    if (customerCount >= MAX_CUSTOMERS) {
        printf("Customer list is full.\n");
        return -1;
    }
    c.id = nextCustomerId();
    strcpy(c.phone, phone);
    readText("Enter customer name: ", c.name, MAX_LEN);
    readCnic(c.cnic);
    c.rentals = 0;
    c.spent = 0;
    c.blacklisted = 0;

    customers[customerCount] = c;
    customerCount++;
    saveCustomers();
    printf("Customer registered with ID %d.\n", c.id);
    return customerCount - 1;
}

/* Asks for a phone number; finds the customer or registers a new one. */
int pickCustomer() {
    char phone[PHONE_LEN];
    int index = -1;

    printf("\n-- Customer --\n");
    readPhone(phone);
    index = findCustomerByPhone(phone);
    if (index != -1) {
        printf("Welcome back, %s! (%d previous rental(s))\n",
               customers[index].name, customers[index].rentals);
        return index;
    }
    printf("New customer. Please register.\n");
    return registerCustomer(phone);
}

void registerCustomerMenu() {
    char phone[PHONE_LEN];

    printf("\n--- Register Customer ---\n");
    readPhone(phone);
    if (findCustomerByPhone(phone) != -1) {
        printf("A customer with this phone number already exists.\n");
        return;
    }
    registerCustomer(phone);
}

void showAllCustomers() {
    printf("\n--- All Customers ---\n");
    if (customerCount == 0) {
        printf("No customers registered yet.\n");
        return;
    }
    printCustomerHeader();
    for (int i = 0; i < customerCount; i++) {
        printCustomer(i);
    }
    printLine(87, '-');
    printf("%d customer(s).\n", customerCount);
}

void findCustomer() {
    char key[MAX_LEN];
    int found = 0, rows = 0;

    printf("\n--- Find Customer ---\n");
    readText("Enter customer name (or part of it): ", key, MAX_LEN);

    for (int i = 0; i < customerCount; i++) {
        if (!containsIgnoreCase(customers[i].name, key)) {
            continue;
        }
        found++;
        printCustomerHeader();
        printCustomer(i);

        printf("\nCurrently renting:\n");
        rows = 0;
        for (int v = 0; v < vehicleCount; v++) {
            if (vehicles[v].status == RENTED && vehicles[v].customerId == customers[i].id) {
                printf("  %s (ID %d)%s\n", vehicles[v].model, vehicles[v].id,
                       isOverdue(v) ? "  *** OVERDUE ***" : "");
                rows++;
            }
        }
        if (rows == 0) printf("  Nothing.\n");

        printf("\nPast rentals:\n");
        rows = 0;
        for (int h = 0; h < historyCount; h++) {
            if (history[h].customerId == customers[i].id) {
                if (rows == 0) printHistoryHeader();
                printHistoryRow(h);
                rows++;
            }
        }
        if (rows == 0) printf("  None.\n");
    }
    if (found == 0) {
        printf("No customer matches \"%s\".\n", key);
    }
}

void customerMenu() {
    int choice = -1;

    while (choice != 0) {
        printf("\n---- CUSTOMERS ----\n");
        printf("1. Show all customers\n2. Find a customer\n");
        printf("3. Register a new customer\n0. Back\n");
        choice = readInt("Choice (0-3): ", 0, 3);
        switch (choice) {
            case 1: showAllCustomers(); break;
            case 2: findCustomer(); break;
            case 3: registerCustomerMenu(); break;
        }
    }
}

/* ================================================================ */
/*                       RENT / RETURN / EXTEND                     */
/* ================================================================ */

void rentVehicle() {
    int id = 0, index = -1, cIndex = -1, days = 0;
    int base = 0, loyalty = 0, tax = 0, estimate = 0, deposit = 0;
    char due[11];

    printf("\n--- Rent a Vehicle ---\n");
    if (countByStatus(AVAILABLE) == 0) {
        printf("No vehicles are available. Please try later.\n");
        return;
    }
    showAvailableVehicles();

    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].status == RENTED) {
        printf("Vehicle %d is already rented by %s.\n", id,
               customerName(vehicles[index].customerId));
        return;
    }
    if (vehicles[index].status == MAINTENANCE) {
        printf("Vehicle %d is under maintenance.\n", id);
        return;
    }

    cIndex = pickCustomer();
    if (cIndex == -1) {
        return;
    }
    if (customers[cIndex].blacklisted) {
        printf("%s is blacklisted and cannot rent vehicles.\n", customers[cIndex].name);
        return;
    }
    if (countActiveRentals(customers[cIndex].id) >= MAX_ACTIVE_PER_CUSTOMER) {
        printf("%s already has %d active rentals (the maximum).\n",
               customers[cIndex].name, MAX_ACTIVE_PER_CUSTOMER);
        return;
    }

    days = readInt("Enter number of days: ", 1, MAX_DAYS);

    base = getCost(vehicles[index].rate, days);
    loyalty = getLoyaltyDiscount(base, cIndex);
    tax = (base - loyalty) * TAX_PERCENT / 100;
    estimate = base - loyalty + tax;
    deposit = estimate * DEPOSIT_PERCENT / 100;
    if (deposit < 1) deposit = 1;
    formatDate(time(NULL) + (time_t)days * SECONDS_PER_DAY, due);

    printf("\n--- Booking Summary ---\n");
    printf("Vehicle:       %s\n", vehicles[index].model);
    printf("Customer:      %s (%s)\n", customers[cIndex].name, customers[cIndex].phone);
    printf("Days:          %d (due back on %s)\n", days, due);
    if (days >= DISCOUNT_DAYS) {
        printf("Discount:      %d%% long-rental discount applied\n", DISCOUNT_PERCENT);
    }
    if (loyalty > 0) {
        printf("Loyalty:       %d%% repeat-customer discount (-Rs %d)\n",
               LOYALTY_PERCENT, loyalty);
    }
    printf("Est. cost:     Rs %d (includes %d%% service tax)\n", estimate, TAX_PERCENT);
    printf("Deposit now:   Rs %d (refundable)\n", deposit);
    printf("Late days are charged at %d%% of the daily rate.\n", 100 + LATE_FEE_PERCENT);

    if (!readYesNo("Confirm booking?")) {
        printf("Booking cancelled.\n");
        return;
    }

    vehicles[index].status = RENTED;
    vehicles[index].customerId = customers[cIndex].id;
    vehicles[index].days = days;
    vehicles[index].rentTime = time(NULL);
    vehicles[index].deposit = deposit;
    saveVehicles();
    printf("Vehicle %d (%s) rented to %s. Please collect Rs %d deposit.\n", id,
           vehicles[index].model, customers[cIndex].name, deposit);
}

void returnVehicle() {
    int id = 0, index = -1, cIndex = -1, actual = 0, extra = 0;
    int damage = 0, fuel = 0, rating = 0;
    char rented[11], due[11];
    Rental r;

    printf("\n--- Return a Vehicle ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);

    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].status != RENTED) {
        printf("Vehicle %d is not rented.\n", id);
        return;
    }
    cIndex = findCustomerById(vehicles[index].customerId);

    formatDate(vehicles[index].rentTime, rented);
    formatDate(dueTime(index), due);
    printf("Rented by %s on %s for %d day(s), due %s.\n",
           customerName(vehicles[index].customerId), rented,
           vehicles[index].days, due);

    if (readYesNo("Calculate days from the system clock?")) {
        actual = elapsedDays(vehicles[index].rentTime);
        printf("Days kept (from clock): %d\n", actual);
    } else {
        actual = readInt("Actual number of days the vehicle was kept: ", 1, MAX_DAYS);
    }
    damage = readInt("Damage charge (0 if none): Rs ", 0, MAX_EXTRA_CHARGE);
    fuel = readInt("Fuel charge (0 if none): Rs ", 0, MAX_EXTRA_CHARGE);

    /* build the rental record */
    r.receiptNo = nextReceiptNo();
    r.vehicleId = vehicles[index].id;
    strcpy(r.model, vehicles[index].model);
    r.customerId = vehicles[index].customerId;
    strcpy(r.customer, customerName(vehicles[index].customerId));
    getToday(r.date);
    r.rate = vehicles[index].rate;
    r.bookedDays = vehicles[index].days;
    r.actualDays = actual;
    r.baseCost = getCost(r.rate, r.bookedDays);
    r.loyalty = getLoyaltyDiscount(r.baseCost, cIndex);
    r.lateFee = 0;
    if (actual > r.bookedDays) {
        extra = actual - r.bookedDays;
        r.lateFee = extra * r.rate * (100 + LATE_FEE_PERCENT) / 100;
    }
    r.tax = (r.baseCost - r.loyalty + r.lateFee) * TAX_PERCENT / 100;
    r.damage = damage;
    r.fuel = fuel;
    r.deposit = vehicles[index].deposit;
    r.total = r.baseCost - r.loyalty + r.lateFee + r.tax + damage + fuel;

    printReceipt(stdout, &r);
    saveReceipt(&r);

    /* store in history (drop the oldest one if full) */
    if (historyCount >= MAX_HISTORY) {
        memmove(&history[0], &history[1], sizeof(Rental) * (MAX_HISTORY - 1));
        historyCount--;
    }
    history[historyCount++] = r;

    /* update customer and vehicle */
    if (cIndex != -1) {
        customers[cIndex].rentals++;
        customers[cIndex].spent += r.total;
    }
    vehicles[index].timesRented++;
    vehicles[index].revenue += r.total;
    vehicles[index].status = AVAILABLE;
    vehicles[index].customerId = 0;
    vehicles[index].days = 0;
    vehicles[index].rentTime = 0;
    vehicles[index].deposit = 0;

    rating = readInt("Customer rating for this vehicle (1-5, 0 to skip): ", 0, 5);
    if (rating > 0) {
        vehicles[index].ratingSum += rating;
        vehicles[index].ratingCount++;
    }

    saveAll();
    printf("Vehicle returned successfully.\n");
}

void extendRental() {
    int id = 0, index = -1, extra = 0, room = 0;
    char prompt[80], due[11];

    printf("\n--- Extend a Rental ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].status != RENTED) {
        printf("Vehicle %d is not rented.\n", id);
        return;
    }
    room = MAX_DAYS - vehicles[index].days;
    if (room <= 0) {
        printf("This rental is already at the maximum of %d days.\n", MAX_DAYS);
        return;
    }
    printf("Rented by %s, currently booked for %d day(s).\n",
           customerName(vehicles[index].customerId), vehicles[index].days);
    snprintf(prompt, sizeof(prompt), "Extra days to add (1-%d): ", room);
    extra = readInt(prompt, 1, room);

    if (!readYesNo("Confirm extension?")) {
        printf("Cancelled.\n");
        return;
    }
    vehicles[index].days += extra;
    formatDate(dueTime(index), due);
    saveVehicles();
    printf("Rental extended to %d day(s). New due date: %s.\n",
           vehicles[index].days, due);
}

void showActiveRentals() {
    char due[11], state[30];
    double secs = 0;
    int count = 0, late = 0, left = 0, cIndex = -1;

    printf("\n--- Active Rentals ---\n");
    printf("%-6s %-18s %-16s %-14s %-11s %-16s\n", "ID", "Vehicle", "Customer",
           "Phone", "Due", "Status");
    printLine(86, '-');
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].status != RENTED) {
            continue;
        }
        cIndex = findCustomerById(vehicles[i].customerId);
        formatDate(dueTime(i), due);
        secs = difftime(time(NULL), dueTime(i));
        if (secs > 0) {
            late = (int)(secs / SECONDS_PER_DAY) + 1;
            sprintf(state, "OVERDUE %d day(s)", late);
        } else {
            left = (int)(-secs / SECONDS_PER_DAY) + 1;
            sprintf(state, "%d day(s) left", left);
        }
        printf("%-6d %-18.18s %-16.16s %-14s %-11s %-16s\n", vehicles[i].id,
               vehicles[i].model, customerName(vehicles[i].customerId),
               cIndex != -1 ? customers[cIndex].phone : "-", due, state);
        count++;
    }
    printLine(86, '-');
    if (count == 0) {
        printf("No vehicles are rented out right now.\n");
    } else {
        printf("%d active rental(s), %d overdue.\n", count, countOverdue());
    }
}

/* ================================================================ */
/*                          SEARCH / SORT                           */
/* ================================================================ */

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
    printLine(79, '-');
    printf("%d match(es) found.\n", found);
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
    printLine(79, '-');
    printf("%d %s(s) in fleet.\n", found, typeNames[type]);
}

void searchByBudget() {
    int maxRate = readInt("Maximum rate per day: Rs ", 1, MAX_RATE);
    int found = 0;

    printf("Available vehicles up to Rs %d per day:\n", maxRate);
    printHeader();
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].status == AVAILABLE && vehicles[i].rate <= maxRate) {
            printVehicle(i);
            found++;
        }
    }
    printLine(79, '-');
    printf("%d vehicle(s) within budget.\n", found);
}

void searchMenu() {
    int choice = 0;

    printf("\n--- Search ---\n");
    printf("1. By vehicle ID\n2. By model name\n3. By vehicle type\n");
    printf("4. By budget (available vehicles)\n");
    choice = readInt("Choice (1-4): ", 1, 4);

    if (choice == 1) searchById();
    else if (choice == 2) searchByModel();
    else if (choice == 3) searchByType();
    else searchByBudget();
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
    int mode = 0;

    printf("\n--- Sort Fleet ---\n");
    printf("1. By ID\n2. By model (A-Z)\n3. By rate (low to high)\n");
    printf("4. By rate (high to low)\n");
    mode = readInt("Choice (1-4): ", 1, 4);

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

void fleetMenu() {
    int choice = -1;

    while (choice != 0) {
        printf("\n---- FLEET ----\n");
        printf("1. Show all vehicles\n2. Show available vehicles\n");
        printf("3. Search\n4. Sort fleet\n0. Back\n");
        choice = readInt("Choice (0-4): ", 0, 4);
        switch (choice) {
            case 1: showAllVehicles(); break;
            case 2: showAvailableVehicles(); break;
            case 3: searchMenu(); break;
            case 4: sortFleet(); break;
        }
    }
}

/* ================================================================ */
/*                       HISTORY / REPORTS                          */
/* ================================================================ */

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
    printLine(75, '-');
    printf("%d rental(s) recorded. (Days column = booked/actual)\n", historyCount);
}

/* Prints the full report to the screen or to a file. */
void printReport(FILE *out) {
    int rented = countByStatus(RENTED), maint = countByStatus(MAINTENANCE);
    int usable = vehicleCount - maint;
    int earned = 0, lateTotal = 0, taxTotal = 0, extras = 0, today = 0;
    int expected = 0, mostRented = -1, topEarner = -1, best = -1;
    int used[MAX_CUSTOMERS] = {0};
    char todayStr[11];

    getToday(todayStr);
    for (int i = 0; i < vehicleCount; i++) {
        if (vehicles[i].status == RENTED) {
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
        taxTotal += history[i].tax;
        extras += history[i].damage + history[i].fuel;
        if (strcmp(history[i].date, todayStr) == 0) {
            today += history[i].total;
        }
    }

    fprintf(out, "\n========================================\n");
    fprintf(out, "      VEHICLE RENTAL REPORT (%s)\n", todayStr);
    fprintf(out, "========================================\n");
    fprintf(out, "Total vehicles:            %d\n", vehicleCount);
    fprintf(out, "Rented:                    %d\n", rented);
    fprintf(out, "Available:                 %d\n", countByStatus(AVAILABLE));
    fprintf(out, "Under maintenance:         %d\n", maint);
    if (usable > 0) {
        fprintf(out, "Fleet utilisation:         %d%%\n", rented * 100 / usable);
    }
    fprintf(out, "Overdue rentals:           %d\n", countOverdue());
    fprintf(out, "Registered customers:      %d\n", customerCount);
    fprintf(out, "Completed rentals:         %d\n", historyCount);
    fprintf(out, "----------------------------------------\n");
    fprintf(out, "Earned (all time):         Rs %d\n", earned);
    fprintf(out, "Earned today:              Rs %d\n", today);
    fprintf(out, "  late charges:            Rs %d\n", lateTotal);
    fprintf(out, "  damage + fuel charges:   Rs %d\n", extras);
    fprintf(out, "  service tax collected:   Rs %d\n", taxTotal);
    fprintf(out, "Expected (active, no tax): Rs %d\n", expected);
    if (historyCount > 0) {
        fprintf(out, "Average per rental:        Rs %d\n", earned / historyCount);
    }
    if (mostRented != -1) {
        fprintf(out, "Most rented vehicle:       %s (%d trips)\n",
                vehicles[mostRented].model, vehicles[mostRented].timesRented);
        fprintf(out, "Top earning vehicle:       %s (Rs %d)\n",
                vehicles[topEarner].model, vehicles[topEarner].revenue);
    }
    fprintf(out, "========================================\n");

    fprintf(out, "\nTop customers:\n");
    for (int rank = 1; rank <= 3; rank++) {
        best = -1;
        for (int c = 0; c < customerCount; c++) {
            if (!used[c] && customers[c].spent > 0 &&
                (best == -1 || customers[c].spent > customers[best].spent)) {
                best = c;
            }
        }
        if (best == -1) {
            if (rank == 1) fprintf(out, "  No completed rentals yet.\n");
            break;
        }
        used[best] = 1;
        fprintf(out, "  %d. %-18s Rs %d (%d rental(s))\n", rank,
                customers[best].name, customers[best].spent, customers[best].rentals);
    }

    fprintf(out, "\nFleet by type:\n");
    for (int t = 1; t <= TYPE_COUNT; t++) {
        int total = 0, rentedNow = 0, money = 0;
        for (int i = 0; i < vehicleCount; i++) {
            if (vehicles[i].type == t) {
                total++;
                money += vehicles[i].revenue;
                if (vehicles[i].status == RENTED) rentedNow++;
            }
        }
        if (total > 0) {
            fprintf(out, "  %-6s %2d vehicle(s), %d rented, revenue Rs %d\n",
                    typeNames[t], total, rentedNow, money);
        }
    }

    fprintf(out, "\nRevenue per vehicle:\n");
    for (int i = 0; i < vehicleCount; i++) {
        fprintf(out, "  %-4d %-20.20s %3d trip(s)  Rs %d\n", vehicles[i].id,
                vehicles[i].model, vehicles[i].timesRented, vehicles[i].revenue);
    }
}

void saveReport() {
    FILE *fp = fopen(REPORT_FILE, "w");
    if (fp == NULL) {
        printf("Could not create %s.\n", REPORT_FILE);
        return;
    }
    printReport(fp);
    fclose(fp);
    printf("Report saved to %s.\n", REPORT_FILE);
}

void reportsMenu() {
    int choice = -1;

    while (choice != 0) {
        printf("\n---- HISTORY & REPORTS ----\n");
        printf("1. Rental history\n2. Business report\n");
        printf("3. Save report to %s\n0. Back\n", REPORT_FILE);
        choice = readInt("Choice (0-3): ", 0, 3);
        switch (choice) {
            case 1: showHistory(); break;
            case 2: printReport(stdout); break;
            case 3: saveReport(); break;
        }
    }
}

/* ================================================================ */
/*                          ADMIN PANEL                             */
/* ================================================================ */

int adminLogin() {
    char pass[MAX_LEN];

    for (int attempt = 1; attempt <= 3; attempt++) {
        readText("Enter admin password: ", pass, MAX_LEN);
        if (strcmp(pass, adminPassword) == 0) {
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
    if (vehicles[index].status == RENTED) {
        printf("Vehicle %d is currently rented. Edit it after it is returned.\n", id);
        return;
    }

    printf("1. Change model name\n2. Change daily rate\n3. Change type\n");
    choice = readInt("Choice (1-3): ", 1, 3);
    if (choice == 1) {
        readText("Enter new model name: ", model, MAX_LEN);
        strcpy(vehicles[index].model, model);
    } else if (choice == 2) {
        vehicles[index].rate = readInt("Enter new rate per day: Rs ", 1, MAX_RATE);
    } else {
        vehicles[index].type = readType();
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
    if (vehicles[index].status == RENTED) {
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

void toggleMaintenance() {
    int id = 0, index = -1;

    printf("\n--- Maintenance ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);
    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].status == RENTED) {
        printf("Vehicle %d is currently rented.\n", id);
        return;
    }
    if (vehicles[index].status == AVAILABLE) {
        if (readYesNo("Send this vehicle to maintenance?")) {
            vehicles[index].status = MAINTENANCE;
            printf("%s is now under maintenance.\n", vehicles[index].model);
        }
    } else {
        if (readYesNo("Maintenance finished? Make it available?")) {
            vehicles[index].status = AVAILABLE;
            printf("%s is available again.\n", vehicles[index].model);
        }
    }
    saveVehicles();
}

void toggleBlacklist() {
    char phone[PHONE_LEN];
    int index = -1;

    printf("\n--- Blacklist ---\n");
    readPhone(phone);
    index = findCustomerByPhone(phone);
    if (index == -1) {
        printf("No customer with that phone number.\n");
        return;
    }
    printCustomerHeader();
    printCustomer(index);
    if (customers[index].blacklisted) {
        if (readYesNo("Remove this customer from the blacklist?")) {
            customers[index].blacklisted = 0;
            printf("Customer can rent again.\n");
        }
    } else {
        if (readYesNo("Blacklist this customer?")) {
            customers[index].blacklisted = 1;
            printf("Customer blacklisted.\n");
        }
    }
    saveCustomers();
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

void changePassword() {
    char current[MAX_LEN], first[MAX_LEN], second[MAX_LEN];

    printf("\n--- Change Password ---\n");
    readText("Enter current password: ", current, MAX_LEN);
    if (strcmp(current, adminPassword) != 0) {
        printf("Wrong password.\n");
        return;
    }
    readText("Enter new password (min 4 characters): ", first, MAX_LEN);
    if (strlen(first) < 4) {
        printf("Too short. Password not changed.\n");
        return;
    }
    readText("Re-enter new password: ", second, MAX_LEN);
    if (strcmp(first, second) != 0) {
        printf("Passwords do not match. Password not changed.\n");
        return;
    }
    strcpy(adminPassword, first);
    savePassword();
    printf("Password changed.\n");
}

void adminPanel() {
    int choice = -1;

    printf("\n--- Admin Panel ---\n");
    if (!adminLogin()) {
        return;
    }
    while (choice != 0) {
        printf("\n---- ADMIN ----\n");
        printf("1. Add a vehicle\n2. Edit a vehicle\n3. Remove a vehicle\n");
        printf("4. Maintenance on/off\n5. Blacklist / unblacklist customer\n");
        printf("6. Clear rental history\n7. Change admin password\n0. Back\n");
        choice = readInt("Choice (0-7): ", 0, 7);
        switch (choice) {
            case 1: addNewVehicle(); break;
            case 2: editVehicle(); break;
            case 3: removeVehicle(); break;
            case 4: toggleMaintenance(); break;
            case 5: toggleBlacklist(); break;
            case 6: clearHistory(); break;
            case 7: changePassword(); break;
        }
    }
}

/* ================================================================ */
/*                              MAIN                                */
/* ================================================================ */

void showMenu() {
    printf("\n========================================\n");
    printf("     VEHICLE RENTAL MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf(" 1. Fleet (view / search / sort)\n");
    printf(" 2. Rent a vehicle\n");
    printf(" 3. Return a vehicle\n");
    printf(" 4. Extend a rental\n");
    printf(" 5. Active rentals and overdue\n");
    printf(" 6. Customers\n");
    printf(" 7. History and reports\n");
    printf(" 8. Admin panel\n");
    printf(" 0. Save and exit\n");
    printf("========================================\n");
}

int main() {
    int choice = -1, overdue = 0;

    loadPassword();
    if (loadVehicles() == 0) {
        loadDefaultVehicles();
        saveVehicles();
        printf("No saved data found. Loaded default fleet.\n");
    }
    loadCustomers();
    loadHistory();
    printf("Vehicle rental system started: %d vehicles, %d customers, %d past rentals.\n",
           vehicleCount, customerCount, historyCount);

    overdue = countOverdue();
    if (overdue > 0) {
        printf("\n*** ALERT: %d rental(s) are overdue! Check option 5. ***\n", overdue);
    }

    while (choice != 0) {
        showMenu();
        choice = readInt("Enter your choice (0-8): ", 0, 8);

        switch (choice) {
            case 1: fleetMenu(); break;
            case 2: rentVehicle(); break;
            case 3: returnVehicle(); break;
            case 4: extendRental(); break;
            case 5: showActiveRentals(); break;
            case 6: customerMenu(); break;
            case 7: reportsMenu(); break;
            case 8: adminPanel(); break;
            case 0:
                saveAll();
                printf("\nData saved. Goodbye!\n");
                break;
        }
    }
    return 0;
}