#include <stdio.h>
#include <string.h>

#define TOTAL_CARS 5

typedef struct {
    int carID;
    int rentrateperday; // Changed to int for numerical rates
    int isrented;
} Car;

Car fleet[TOTAL_CARS];

void displayMenu() { 
    printf("====================\n"); 
    printf("1. Display Available Cars\n"); 
    printf("2. Rent a Car\n"); 
    printf("3. Return a Car\n"); 
    printf("4. Exit\n"); 
    printf("====================\n"); 
    printf("Enter your choice: "); 
} 

void initializeFleet() { 
    // CAR 1 
    fleet[0].carID = 101; 
    fleet[0].rentrateperday = 50; 
    fleet[0].isrented = 0; 
    
    // CAR 2 
    fleet[1].carID = 102; 
    fleet[1].rentrateperday = 60; 
    fleet[1].isrented = 0; 
    
    // CAR 3 
    fleet[2].carID = 103; 
    fleet[2].rentrateperday = 70; 
    fleet[2].isrented = 0; 
    
    // CAR 4 
    fleet[3].carID = 104; 
    fleet[3].rentrateperday = 80; 
    fleet[3].isrented = 0; 
    
    // CAR 5 
    fleet[4].carID = 105; 
    fleet[4].rentrateperday = 90; 
    fleet[4].isrented = 0; 
} 

int main() { 
    initializeFleet(); 
    printf("Car rental system initialized with %d cars.\n", TOTAL_CARS); 
    printf("Car rental system is ready for operations.\n"); 

    int choice = 0; 
    while (choice != 4) { 
        displayMenu(); 
        if (scanf("%d", &choice) != 1) {
        
            while (getchar() != '\n');
            choice = 0;
        }
        
        switch (choice) { 
            case 1: 
                printf("Displaying available cars...\n"); 
                break; 
            case 2: 
                printf("Renting a car...\n"); 
                break; 
            case 3: 
                printf("Returning a car...\n"); 
                break; 
            case 4: 
                printf("Thank you for using the car rental system. Goodbye!\n"); 
                break; 
            default: 
                printf("Invalid choice. Please try again.\n"); 
        } 
    } 

    return 0; 
}





