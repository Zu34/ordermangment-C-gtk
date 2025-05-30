#include "inventory.h"
#include <stdio.h>
#include <stdlib.h>

bool inventory_init(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS inventory ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "item_name TEXT NOT NULL,"
                      "quantity INTEGER NOT NULL,"
                      "price REAL NOT NULL);";
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create inventory table: %s\n", errmsg);
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

bool inventory_add(sqlite3 *db, const char *item_name, int quantity, double price) {
    const char *sql = "INSERT INTO inventory (item_name, quantity, price) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, item_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, quantity);
    sqlite3_bind_double(stmt, 3, price);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

void inventory_list(sqlite3 *db) {
    const char *sql = "SELECT id, item_name, quantity, price FROM inventory;";
    sqlite3_stmt *stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare select statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("Inventory List:\n");
    printf("ID | Item Name | Quantity | Price\n");
    printf("---------------------------------\n");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *item_name = sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);

        printf("%d | %s | %d | %.2f\n", id, item_name, quantity, price);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error reading rows: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}
