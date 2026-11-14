/* ================================================================ */
/* EXTRAS MODULE: paste above MAIN (after adminPanel)               */
/* ================================================================ */

#define XT_CSV_FILE "customers.csv"
#define XT_SILVER 5
#define XT_GOLD 10

const char *xtMonths[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/* ---------------- price quote ---------------- */
/* Shows what a rental would cost without booking anything. */
void xtPriceQuote(void) {
    int id, index, days, cIndex = -1;
    int base, loyalty, tax, total, deposit, lateDayFee;
    char phone[PHONE_LEN];

    printf("\n--- Price Quote ---\n");
    id = readInt("Enter vehicle ID: ", 1, 999999);
    index = findVehicle(id);

    if (index == -1) {
        printf("Vehicle %d was not found.\n", id);
        return;
    }
    if (vehicles[index].status == MAINTENANCE) {
        printf("Vehicle %d is under maintenance.\n", id);
        return;
    }

    days = readInt("Number of days: ", 1, MAX_DAYS);

    if (readYesNo("Is the customer already registered?")) {
        readPhone(phone);
        cIndex = findCustomerByPhone(phone);
        if (cIndex == -1) {
            printf("Phone not found. No loyalty discount in this quote.\n");
        }
    }

    base = getCost(vehicles[index].rate, days);
    loyalty = getLoyaltyDiscount(base, cIndex);
    tax = (int)((long)(base - loyalty) * TAX_PERCENT / 100);
    total = base - loyalty + tax;
    deposit = (int)((long)total * DEPOSIT_PERCENT / 100);
    if (deposit < 1 && total > 0) {
        deposit = 1;
    }
    lateDayFee = (int)((long)vehicles[index].rate * (100 + LATE_FEE_PERCENT) / 100);

    printf("\n========== QUOTE ==========\n");
    printf("Vehicle:         %s\n", vehicles[index].model);
    printf("Rate x days:     Rs %d x %d\n", vehicles[index].rate, days);
    printf("Base cost:       Rs %d\n", base);
    if (days >= DISCOUNT_DAYS) {
        printf("                 (%d%% long-rental discount included)\n", DISCOUNT_PERCENT);
    }
    if (loyalty > 0) {
        printf("Loyalty:        -Rs %d\n", loyalty);
    }
    printf("Service tax:     Rs %d\n", tax);
    printf("Estimated total: Rs %d\n", total);
    printf("Deposit needed:  Rs %d\n", deposit);
    printf("Each late day:   Rs %d (before tax)\n", lateDayFee);
    printf("===========================\n");
}

/* ---------------- monthly revenue ---------------- */
void xtMonthlyRevenue(void) {
    int year, m, h, i, bar;
    int totals[12], counts[12];
    int max = 0, yearTotal = 0, yearCount = 0;
    char prefix[16];

    year = readInt("Enter year (for example 2026): ", 2000, 2100);

    for (m = 0; m < 12; m++) {
        totals[m] = 0;
        counts[m] = 0;
        snprintf(prefix, sizeof(prefix), "%04d-%02d", year, m + 1);

        for (h = 0; h < historyCount; h++) {
            if (strncmp(history[h].date, prefix, 7) == 0) {
                totals[m] += history[h].total;
                counts[m]++;
            }
        }
        yearTotal += totals[m];
        yearCount += counts[m];
        if (totals[m] > max) {
            max = totals[m];
        }
    }

    if (yearCount == 0) {
        printf("No completed rentals were recorded in %d.\n", year);
        return;
    }

    printf("\nRevenue for %d\n", year);
    printLine(62, '-');
    for (m = 0; m < 12; m++) {
        bar = (max > 0) ? (int)((long)totals[m] * 30 / max) : 0;
        printf("%-4s %3d rental(s) Rs %-8d ", xtMonths[m], counts[m], totals[m]);
        for (i = 0; i < bar; i++) {
            putchar('#');
        }
        putchar('\n');
    }
    printLine(62, '-');
    printf("Total: Rs %d from %d rental(s), average Rs %d.\n",
           yearTotal, yearCount, yearTotal / yearCount);
}

/* ---------------- history search ---------------- */
/* One search, three modes: 1 = customer name, 2 = date, 3 = vehicle ID. */
void xtSearchHistory(void) {
    char key[MAX_LEN];
    int mode, id = 0, i, found = 0, sum = 0, match;

    key[0] = '\0';
    printf("\n--- Search Rental History ---\n");
    printf("1. By customer name\n");
    printf("2. By date (2026-11-14, 2026-11 or 2026)\n");
    printf("3. By vehicle ID\n");
    mode = readInt("Choice (1-3): ", 1, 3);

    if (mode == 3) {
        id = readInt("Enter vehicle ID: ", 1, 999999);
    } else if (mode == 1) {
        readText("Customer name (or part of it): ", key, MAX_LEN);
    } else {
        readText("Date: ", key, MAX_LEN);
    }

    for (i = 0; i < historyCount; i++) {
        if (mode == 1) {
            match = containsIgnoreCase(history[i].customer, key);
        } else if (mode == 2) {
            match = (strncmp(history[i].date, key, strlen(key)) == 0);
        } else {
            match = (history[i].vehicleId == id);
        }

        if (match) {
            if (found == 0) {
                printHistoryHeader();
            }
            printHistoryRow(i);
            sum += history[i].total;
            found++;
        }
    }

    if (found == 0) {
        printf("Nothing matched.\n");
    } else {
        printLine(75, '-');
        printf("%d rental(s), Rs %d in total.\n", found, sum);
    }
}

/* ---------------- customer tiers ---------------- */
const char *xtTierName(int rentals) {
    if (rentals >= XT_GOLD) return "Gold";
    if (rentals >= XT_SILVER) return "Silver";
    if (rentals >= LOYALTY_RENTALS) return "Bronze";
    return "Basic";
}

void xtShowTiers(void) {
    int i;

    printf("\n--- Customer Tiers ---\n");
    printf("Gold %d+, Silver %d+, Bronze %d+ rentals (Bronze and up get the loyalty discount)\n",
           XT_GOLD, XT_SILVER, LOYALTY_RENTALS);

    if (customerCount == 0) {
        printf("No customers registered yet.\n");
        return;
    }

    printf("\n%-6s %-18s %-8s %-8s %-10s\n", "ID", "Name", "Tier", "Rentals", "Spent");
    printLine(54, '-');
    for (i = 0; i < customerCount; i++) {
        printf("%-6d %-18.18s %-8s %-8d Rs %-7d\n",
               customers[i].id,
               customers[i].name,
               xtTierName(customers[i].rentals),
               customers[i].rentals,
               customers[i].spent);
    }
    printLine(54, '-');
}

/* ---------------- overdue notices ---------------- */
void xtOverdueNotices(void) {
    char due[32];
    int i, c, count = 0, late, fee, totalFee = 0;

    printf("\n--- Overdue Notices ---\n");
    for (i = 0; i < vehicleCount; i++) {
        if (!isOverdue(i)) {
            continue;
        }
        c = findCustomerById(vehicles[i].customerId);
        formatDate(dueTime(i), due);
        late = elapsedDays(vehicles[i].rentTime) - vehicles[i].days;
        if (late < 1) {
            late = 1;
        }
        fee = (int)((long)late * vehicles[i].rate * (100 + LATE_FEE_PERCENT) / 100);
        totalFee += fee;
        count++;

        printf("\nNOTICE #%d\n", count);
        printf(" Customer:         %s (%s)\n",
               customerName(vehicles[i].customerId),
               c != -1 ? customers[c].phone : "-");
        printf(" Vehicle:          %s (ID %d)\n", vehicles[i].model, vehicles[i].id);
        printf(" Was due:          %s, late by %d day(s)\n", due, late);
        printf(" Late fee so far:  Rs %d (before tax)\n", fee);
    }

    if (count == 0) {
        printf("No overdue rentals. Everyone is on time!\n");
    } else {
        printf("\n%d overdue rental(s), Rs %d in late fees so far.\n", count, totalFee);
    }
}

/* ---------------- fleet statistics ---------------- */
void xtFleetStats(void) {
    int i, cheap = 0, pricey = 0, rateSum = 0, reviews = 0, points = 0;

    printf("\n--- Fleet Statistics ---\n");
    if (vehicleCount == 0) {
        printf("The fleet is empty.\n");
        return;
    }

    for (i = 0; i < vehicleCount; i++) {
        rateSum += vehicles[i].rate;
        reviews += vehicles[i].ratingCount;
        points += vehicles[i].ratingSum;
        if (vehicles[i].rate < vehicles[cheap].rate) cheap = i;
        if (vehicles[i].rate > vehicles[pricey].rate) pricey = i;
    }

    printf("Vehicles in fleet:  %d\n", vehicleCount);
    printf("Cheapest per day:   %s (Rs %d)\n", vehicles[cheap].model, vehicles[cheap].rate);
    printf("Priciest per day:   %s (Rs %d)\n", vehicles[pricey].model, vehicles[pricey].rate);
    printf("Average daily rate: Rs %d\n", rateSum / vehicleCount);

    if (reviews > 0) {
        printf("Average rating:     %.1f / 5 (%d review(s))\n", (double)points / reviews, reviews);
    } else {
        printf("Average rating:     No reviews yet.\n");
    }
}

/* ---------------- export customers ---------------- */
void xtExportCsv(void) {
    FILE *fp = NULL;
    int i, k;

    printf("\n--- Export Customers ---\n");
    if (customerCount == 0) {
        printf("There are no customers to export.\n");
        return;
    }

    fp = fopen(XT_CSV_FILE, "w");
    if (fp == NULL) {
        printf("Could not create %s.\n", XT_CSV_FILE);
        return;
    }

    fprintf(fp, "ID,Name,Phone,CNIC,Rentals,Spent,Tier,Status\n");
    for (i = 0; i < customerCount; i++) {
        fprintf(fp, "%d,\"", customers[i].id);
        for (k = 0; customers[i].name[k] != '\0'; k++) {
            if (customers[i].name[k] == '"') {
                fputc('"', fp);
            }
            fputc(customers[i].name[k], fp);
        }
        fprintf(fp, "\",%s,%s,%d,%d,%s,%s\n",
                customers[i].phone,
                customers[i].cnic,
                customers[i].rentals,
                customers[i].spent,
                xtTierName(customers[i].rentals),
                customers[i].blacklisted ? "Blacklisted" : "Active");
    }

    fclose(fp);
    printf("%d customer(s) exported to %s.\n", customerCount, XT_CSV_FILE);
}

/* ---------------- extras menu ---------------- */
void extrasMenu(void) {
    int choice = -1;

    while (choice != 0) {
        printf("\n---- EXTRAS ----\n");
        printf(" 1. Price quote (no booking)\n");
        printf(" 2. Monthly revenue report\n");
        printf(" 3. Search rental history\n");
        printf(" 4. Customer tiers\n");
        printf(" 5. Overdue notices\n");
        printf(" 6. Fleet statistics\n");
        printf(" 7. Export customers to CSV\n");
        printf(" 0. Back to main menu\n");
        choice = readInt("Choice (0-7): ", 0, 7);

        switch (choice) {
            case 1: xtPriceQuote(); break;
            case 2: xtMonthlyRevenue(); break;
            case 3: xtSearchHistory(); break;
            case 4: xtShowTiers(); break;
            case 5: xtOverdueNotices(); break;
            case 6: xtFleetStats(); break;
            case 7: xtExportCsv(); break;
        }
    }
}