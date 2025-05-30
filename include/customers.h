// include/customers.h
#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <sqlite3.h>

int customers_init(sqlite3 *db);
int customers_add(sqlite3 *db, const char *name, const char *email, const char *phone);
void customers_list(sqlite3 *db);

#endif // CUSTOMERS_H
