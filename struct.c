#include<stdbool.h>

#define MAX_RENTAL_VEHICLES 10

typedef struct
{
    int userId;
    char username[50];
    char email[100];
    char password[50];
    bool isAdmin;
} User;

typedef struct
{
    int vehicleId;
    char brand[50];
    char model[50];
    int year;
    char registrationNumber[30];
    char category[30];
    char fuelType[20];
    char transmission[20];
    float dailyRate;
    float dailyLateFee;
    bool isAvailable;
    bool onMaintainance;
} Vehicle;

typedef struct
{
    int rentalId;
    int userId;
    char startingDate[20];
    int vehicleCount;
    int vehicleIds[MAX_RENTAL_VEHICLES];
    int totalDays[MAX_RENTAL_VEHICLES];
    char returnDates[MAX_RENTAL_VEHICLES][20];
    float totalAmounts[MAX_RENTAL_VEHICLES];
    float lateFees[MAX_RENTAL_VEHICLES];
    float grandTotal;
    bool status;
} Rental;