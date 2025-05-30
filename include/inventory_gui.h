#ifndef INVENTORY_GUI_H
#define INVENTORY_GUI_H

#include <gtk/gtk.h>
#include <sqlite3.h>

void inventory_gui_show(GtkWidget *parent, sqlite3 *db);

#endif // INVENTORY_GUI_H
