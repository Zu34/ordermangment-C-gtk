#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>
#include <sqlite3.h>

typedef struct {
    int id;
    char name[256];
    int quantity;
    double price;
} InventoryItem;


bool inventory_init(sqlite3 *db);

bool inventory_add(sqlite3 *db, const char *name, int quantity, double price);

void inventory_list(sqlite3 *db);


#endif // INVENTORY_H
