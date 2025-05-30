#include "inventory.h"
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

bool inventory_init(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS inventory ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "quantity INTEGER NOT NULL,"
                      "price REAL NOT NULL"
                      ");";
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creating inventory table: %s\n", errmsg);
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

bool inventory_add(sqlite3 *db, const char *name, int quantity, double price) {
    const char *sql = "INSERT INTO inventory (name, quantity, price) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare insert statement\n");
        return false;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, quantity);
    sqlite3_bind_double(stmt, 3, price);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute insert statement\n");
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

void inventory_list(sqlite3 *db) {
    const char *sql = "SELECT id, name, quantity, price FROM inventory;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare select statement\n");
        return;
    }

    printf("Inventory Items:\n");
    printf("%-5s %-20s %-10s %-10s\n", "ID", "Name", "Quantity", "Price");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);

        printf("%-5d %-20s %-10d $%-9.2f\n", id, name, quantity, price);
    }

    sqlite3_finalize(stmt);
}
