#include "inventory_gui.h"
#include <gtk/gtk.h>

#include "inventory.h"
#include <stdio.h>

// Forward declarations
static void on_add_item_clicked(GtkButton *button, gpointer user_data);
static void refresh_inventory_list(GtkListStore *store, sqlite3 *db);

// Structure to hold references needed in callbacks
typedef struct {
    GtkWidget *window;
    GtkListStore *store;
    sqlite3 *db;
} InventoryGUIData;

void inventory_gui_show(GtkWidget *parent, sqlite3 *db) {
    InventoryGUIData *data = g_malloc(sizeof(InventoryGUIData));
    data->db = db;

    data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(data->window), "Inventory Management");
    gtk_window_set_default_size(GTK_WINDOW(data->window), 600, 400);
    gtk_window_set_transient_for(GTK_WINDOW(data->window), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(data->window), TRUE);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(data->window), vbox);

    // List store columns: ID, Item Name, Quantity, Price
    data->store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_DOUBLE);

    GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->store));

    // Add columns to treeview
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Item Name
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Item Name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Quantity
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Quantity", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Price
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Price", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    gtk_box_pack_start(GTK_BOX(vbox), treeview, TRUE, TRUE, 0);

    // Add item button
    GtkWidget *btn_add = gtk_button_new_with_label("Add Item");
    gtk_box_pack_start(GTK_BOX(vbox), btn_add, FALSE, FALSE, 0);
    g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_item_clicked), data);

    // Populate initial data
    refresh_inventory_list(data->store, db);

    gtk_widget_show_all(data->window);
}

// Refresh the list store from the database
static void refresh_inventory_list(GtkListStore *store, sqlite3 *db) {
    gtk_list_store_clear(store);

    const char *sql = "SELECT id, item_name, quantity, price FROM inventory;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare inventory query: %s\n", sqlite3_errmsg(db));
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, name,
                           2, quantity,
                           3, price,
                           -1);
    }
    sqlite3_finalize(stmt);
}

// Callback to show dialog and add item
static void on_add_item_clicked(GtkButton *button, gpointer user_data) {
    InventoryGUIData *data = (InventoryGUIData *)user_data;

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Inventory Item",
                                                    GTK_WINDOW(data->window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Add", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Item Name entry
    GtkWidget *label_name = gtk_label_new("Item Name:");
    GtkWidget *entry_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_name, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_name, 1, 0, 1, 1);

    // Quantity entry
    GtkWidget *label_qty = gtk_label_new("Quantity:");
    GtkWidget *entry_qty = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_qty, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_qty, 1, 1, 1, 1);

    // Price entry
    GtkWidget *label_price = gtk_label_new("Price:");
    GtkWidget *entry_price = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_price, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_price, 1, 2, 1, 1);

    gtk_widget_show_all(dialog);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
        const char *qty_text = gtk_entry_get_text(GTK_ENTRY(entry_qty));
        const char *price_text = gtk_entry_get_text(GTK_ENTRY(entry_price));

        int qty = atoi(qty_text);
        double price = atof(price_text);

        if (name && qty > 0 && price >= 0.0) {
            if (!inventory_add(data->db, name, qty, price)) {
                fprintf(stderr, "Failed to add inventory item.\n");
            } else {
                refresh_inventory_list(data->store, data->db);
            }
        }
    }

    gtk_widget_destroy(dialog);
}
