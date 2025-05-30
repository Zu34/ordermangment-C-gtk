// src/orders.c
#include "orders.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>

int orders_init(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS orders ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "customer_id INTEGER NOT NULL,"
                      "order_date TEXT NOT NULL,"
                      "status TEXT NOT NULL,"
                      "FOREIGN KEY(customer_id) REFERENCES customers(id));";
    return db_execute(db, sql);
}

int orders_create(sqlite3 *db, int customer_id) {
    const char *sql_template = "INSERT INTO orders (customer_id, order_date, status) "
                               "VALUES (%d, datetime('now'), 'pending');";
    char sql[256];
    snprintf(sql, sizeof(sql), sql_template, customer_id);

    return db_execute(db, sql);
}

void orders_print_all(sqlite3 *db) {
    const char *sql = "SELECT id, customer_id, order_date, status FROM orders;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch orders: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Orders:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int cust_id = sqlite3_column_int(stmt, 1);
        const unsigned char *date = sqlite3_column_text(stmt, 2);
        const unsigned char *status = sqlite3_column_text(stmt, 3);
        printf("ID: %d | Customer ID: %d | Date: %s | Status: %s\n", id, cust_id, date, status);
    }
    sqlite3_finalize(stmt);
}
