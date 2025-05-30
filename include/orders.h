// include/orders.h
#ifndef ORDERS_H
#define ORDERS_H

#include <sqlite3.h>

int orders_init(sqlite3 *db);
int orders_create(sqlite3 *db, int customer_id);
void orders_print_all(sqlite3 *db);

#endif // ORDERS_H
