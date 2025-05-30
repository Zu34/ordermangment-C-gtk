// include/database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

sqlite3* db_open(const char *filename);
void db_close(sqlite3 *db);
int db_execute(sqlite3 *db, const char *sql);

#endif // DATABASE_H
