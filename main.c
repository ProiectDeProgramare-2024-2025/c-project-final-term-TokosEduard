#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 10
#define MAX_RESERVED 10

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"

typedef struct {
    char name[50];
    int amount;
    float price;
} Item;

Item inventory[MAX_ITEMS];
int itemCount = 0;
Item reservedItems[MAX_RESERVED];
int reservedCount = 0;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void readFromFile() {
    FILE *file = fopen("products.csv", "r");
    if (!file) {
        printf("Nu s-a putut deschide fisierul pentru citire.\n");
        return;
    }

    itemCount = 0;
    while (fscanf(file, " %49[^,],%d,%f\n", inventory[itemCount].name,
                  &inventory[itemCount].amount, &inventory[itemCount].price) == 3) {
        itemCount++;
        if (itemCount >= MAX_ITEMS) break;
    }

    fclose(file);
}

void writeToFile() {
    FILE *file = fopen("products.csv", "w");
    if (!file) {
        printf("Nu s-a putut deschide fisierul pentru scriere.\n");
        return;
    }

    for (int i = 0; i < itemCount; i++) {
        fprintf(file, "%s,%d,%.2f\n", inventory[i].name,
                inventory[i].amount, inventory[i].price);
    }

    fclose(file);
}

void viewItems() {
    clearScreen();
    printf("========================================================\n");
    printf("--- Produse disponibile ---\n");
    printf("========================================================\n");
    if (itemCount == 0) {
        printf("Nu exista produse in inventar.\n");
    } else {
        for (int i = 0; i < itemCount; i++) {
            printf("%d) %s%s%s - %s%d buc%s - %s%.2f RON%s\n", i + 1,
                   COLOR_CYAN, inventory[i].name, COLOR_RESET,
                   COLOR_YELLOW, inventory[i].amount, COLOR_RESET,
                   COLOR_GREEN, inventory[i].price, COLOR_RESET);
        }
    }
}

void reserveItem() {
    clearScreen();
    int choice;
    viewItems();
    if (itemCount == 0) return;

    printf("\n-> Numarul produsului de rezervat: ");
    fflush(stdout);

    if (scanf("%d", &choice) != 1) {
        printf("Numar invalid!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (choice < 1 || choice > itemCount) {
        printf("Numar invalid!\n");
        return;
    }

    if (reservedCount < MAX_RESERVED) {
        reservedItems[reservedCount++] = inventory[choice - 1];
        printf("Produsul '%s' a fost rezervat cu succes!\n", inventory[choice - 1].name);
    } else {
        printf("Lista de rezervari este plina.\n");
    }
}

void addItem() {
    if (itemCount >= MAX_ITEMS) {
        printf("Inventarul este plin! Nu mai poti adauga produse.\n");
        return;
    }

    printf("-> Nume produs: ");
    fgets(inventory[itemCount].name, sizeof(inventory[itemCount].name), stdin);
    size_t len = strlen(inventory[itemCount].name);
    if (len > 0 && inventory[itemCount].name[len - 1] == '\n') {
        inventory[itemCount].name[len - 1] = '\0';
    }

    printf("-> Cantitate disponibila (ex: 10): ");
    scanf("%d", &inventory[itemCount].amount);
    while (getchar() != '\n');

    printf("-> Pret per bucata (ex: 5.99): ");
    scanf("%f", &inventory[itemCount].price);
    while (getchar() != '\n');

    itemCount++;
    writeToFile();
    printf("Produs adaugat si salvat cu succes!\n");
}

void removeItem() {
    clearScreen();
    int choice;
    viewItems();
    if (itemCount == 0) return;

    printf("\n-> Numarul produsului de sters: ");
    fflush(stdout);

    if (scanf("%d", &choice) != 1) {
        printf("Numar invalid!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (choice < 1 || choice > itemCount) {
        printf("Numar invalid!\n");
        return;
    }

    for (int i = choice - 1; i < itemCount - 1; i++) {
        inventory[i] = inventory[i + 1];
    }
    itemCount--;
    writeToFile();
    printf("Produsul a fost sters si fisierul actualizat!\n");
}

void viewReservedItems() {
    clearScreen();
    printf("========================================================\n");
    printf("--- Produse rezervate ---\n");
    printf("========================================================\n");
    if (reservedCount == 0) {
        printf("Nu ai rezervat niciun produs.\n");
    } else {
        for (int i = 0; i < reservedCount; i++) {
            printf("%d) %s%s%s\n", i + 1, COLOR_CYAN, reservedItems[i].name, COLOR_RESET);
        }
    }
}

void searchItem() {
    clearScreen();
    char searchName[50];
    int found = 0;

    printf("-> Introdu numele produsului de cautat: ");
    fgets(searchName, sizeof(searchName), stdin);
    size_t len = strlen(searchName);
    if (len > 0 && searchName[len - 1] == '\n') {
        searchName[len - 1] = '\0';
    }

    printf("\n--- Rezultatele cautarii ---\n");
    for (int i = 0; i < itemCount; i++) {
        if (strstr(inventory[i].name, searchName) != NULL) {
            printf("%d) %s%s%s - %s%d buc%s - %s%.2f RON%s\n", i + 1,
                   COLOR_CYAN, inventory[i].name, COLOR_RESET,
                   COLOR_YELLOW, inventory[i].amount, COLOR_RESET,
                   COLOR_GREEN, inventory[i].price, COLOR_RESET);
            found = 1;
        }
    }

    if (!found) {
        printf("Niciun produs gasit cu numele '%s'.\n", searchName);
    }
}

void cancelReservation() {
    clearScreen();
    printf("========================================================\n");
    printf("--- Anulare rezervare produs ---\n");
    printf("========================================================\n");

    if (reservedCount == 0) {
        printf("Nu exista produse rezervate.\n");
        return;
    }

    for (int i = 0; i < reservedCount; i++) {
        printf("%d) %s%s%s\n", i + 1, COLOR_CYAN, reservedItems[i].name, COLOR_RESET);
    }

    int choice;
    printf("\n-> Numarul produsului de anulat: ");
    fflush(stdout);

    if (scanf("%d", &choice) != 1) {
        printf("Numar invalid!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (choice < 1 || choice > reservedCount) {
        printf("Numar invalid!\n");
        return;
    }

    for (int i = choice - 1; i < reservedCount - 1; i++) {
        reservedItems[i] = reservedItems[i + 1];
    }
    reservedCount--;
    printf("Rezervarea a fost anulata cu succes.\n");
}

int main() {
    readFromFile();
    int choice = 0;

    while (1) {
        clearScreen();
        printf("========================================================\n");
        printf("=== Sistem pentru rezervarea produselor din farmacie ===\n");
        printf("========================================================\n");
        printf("1) Vizualizare produse\n");
        printf("2) Rezervare produse\n");
        printf("3) Adaugare produse\n");
        printf("4) Stergere produse\n");
        printf("5) Vizualizare produse rezervate\n");
        printf("6) Cautare produs\n");
        printf("7) Anulare rezervare\n");
        printf("8) Iesire\n");
        printf("Ce doresti sa faci: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) {
            printf("Eroare: introdu un numar valid!\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: viewItems(); break;
            case 2: reserveItem(); break;
            case 3: addItem(); break;
            case 4: removeItem(); break;
            case 5: viewReservedItems(); break;
            case 6: searchItem(); break;
            case 7: cancelReservation(); break;
            case 8: printf("Program inchis.\n"); return 0;
            default: printf("Optiune invalida!\n");
        }

        printf("\nApasa ENTER pentru a continua...");
        getchar(); 
    }
}
