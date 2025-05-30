// src/customers.c
#include "customers.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>

int customers_init(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS customers ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "email TEXT,"
                      "phone TEXT);";
    return db_execute(db, sql);
}

int customers_add(sqlite3 *db, const char *name, const char *email, const char *phone) {
    const char *sql_template = "INSERT INTO customers (name, email, phone) VALUES ('%q', '%q', '%q');";
    char sql[512];
    snprintf(sql, sizeof(sql), sql_template, name, email, phone);
    return db_execute(db, sql);
}

void customers_list(sqlite3 *db) {
    const char *sql = "SELECT id, name, email, phone FROM customers;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare customers list statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("Customers:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *email = sqlite3_column_text(stmt, 2);
        const unsigned char *phone = sqlite3_column_text(stmt, 3);
        printf("ID: %d | Name: %s | Email: %s | Phone: %s\n", id, name, email, phone);
    }
    sqlite3_finalize(stmt);
}
