#ifndef STRUCT_H
#define STRUCT_H

#define MAX_RENTAL_VEHICLES 10

typedef struct
{
    int user_id;
    char username[50];
    char email[100];
    char password[50];
    bool isAdmin;
} User;

typedef struct
{
    int vehicle_id;
    char brand[50];
    char model[50];
    int year;
    char registration_number[30];
    char category[30];
    char fuel_type[20];
    char transmission[20];
    float daily_rate;
    bool isAvailable;
} Vehicle;

typedef struct
{
    int rental_id;
    int user_id;
    char starting_date[20];
    int vehicleCount;
    int vehicle_ids[MAX_RENTAL_VEHICLES];
    int totalDays[MAX_RENTAL_VEHICLES];
    char returnDates[MAX_RENTAL_VEHICLES][20];
    float totalAmounts[MAX_RENTAL_VEHICLES];
    float grandTotal;
    bool isActive;
} Rental;

#endif