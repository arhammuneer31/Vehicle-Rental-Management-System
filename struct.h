#ifndef STRUCT_H
#define STRUCT_H

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

#endif