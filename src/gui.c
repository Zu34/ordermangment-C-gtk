#include "gui.h"
#include "database.h"   // for db_execute, etc.
#include <stdlib.h>
#include <string.h>

static sqlite3 *global_db = NULL;

static void show_message_dialog(GtkWindow *parent, const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Helper: callback to gather query results in a string
typedef struct {
    GString *result;
} QueryData;

static int callback_collect_rows(void *data, int argc, char **argv, char **col_name) {
    QueryData *qdata = (QueryData *)data;
    for (int i = 0; i < argc; i++) {
        g_string_append_printf(qdata->result, "%s: %s\n", col_name[i], argv[i] ? argv[i] : "NULL");
    }
    g_string_append(qdata->result, "\n");
    return 0;
}

// Button callback: Show Customers
static void on_show_customers_clicked(GtkWidget *widget, gpointer user_data) {
    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_toplevel(widget));
    QueryData qdata;
    qdata.result = g_string_new("");

    const char *sql = "SELECT id, name, email, phone FROM customers;";
    char *errmsg = NULL;
    int rc = sqlite3_exec(global_db, sql, callback_collect_rows, &qdata, &errmsg);
    if (rc != SQLITE_OK) {
        show_message_dialog(parent, "Database Error", errmsg);
        sqlite3_free(errmsg);
        g_string_free(qdata.result, TRUE);
        return;
    }

    if (qdata.result->len == 0) {
        show_message_dialog(parent, "Customers", "No customers found.");
    } else {
        show_message_dialog(parent, "Customers", qdata.result->str);
    }

    g_string_free(qdata.result, TRUE);
}

// Button callback: Show Orders
static void on_show_orders_clicked(GtkWidget *widget, gpointer user_data) {
    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_toplevel(widget));
    QueryData qdata;
    qdata.result = g_string_new("");

    const char *sql = "SELECT id, customer_id, order_date FROM orders;";
    char *errmsg = NULL;
    int rc = sqlite3_exec(global_db, sql, callback_collect_rows, &qdata, &errmsg);
    if (rc != SQLITE_OK) {
        show_message_dialog(parent, "Database Error", errmsg);
        sqlite3_free(errmsg);
        g_string_free(qdata.result, TRUE);
        return;
    }

    if (qdata.result->len == 0) {
        show_message_dialog(parent, "Orders", "No orders found.");
    } else {
        show_message_dialog(parent, "Orders", qdata.result->str);
    }

    g_string_free(qdata.result, TRUE);
}

void gui_init(sqlite3 *db) {
    global_db = db;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Order Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *btn_customers = gtk_button_new_with_label("Show Customers");
    g_signal_connect(btn_customers, "clicked", G_CALLBACK(on_show_customers_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_customers, TRUE, TRUE, 0);

    GtkWidget *btn_orders = gtk_button_new_with_label("Show Orders");
    g_signal_connect(btn_orders, "clicked", G_CALLBACK(on_show_orders_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_orders, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
}
