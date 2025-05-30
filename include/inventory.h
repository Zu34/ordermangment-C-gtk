//include/inventory.h
#ifndef INVENTORY_H
#define INVENTORY_H

#include <sqlite3.h>
#include <stdbool.h>

bool inventory_init(sqlite3 *db);
bool inventory_add(sqlite3 *db, const char *item_name, int quantity, double price);
void inventory_list(sqlite3 *db);

#endif // INVENTORY_H
