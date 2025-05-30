#include <gtk/gtk.h>
#include "gui.h"
#include <stdio.h>
#include "database.h"
#include "customers.h"
#include "orders.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    sqlite3 *db = db_open("data/ordersys.db");
    if (!db) {
        return 1;
    }

    if (!customers_init(db)) {
        fprintf(stderr, "Failed to initialize customers table\n");
        db_close(db);
        return 1;
    }

    if (!orders_init(db)) {
        fprintf(stderr, "Failed to initialize orders table\n");
        db_close(db);
        return 1;
    }

    gui_init();

    gtk_main();

    db_close(db);
    return 0;
}
