#include <sqlite3.h>

#include "repo.h"

#ifndef DATABASE_H
#define DATABASE_H

sqlite3* create_database(char* location);
void list_all_packages(sqlite3 *db);
int get_all_packages(sqlite3 *db, struct repo_package** out_pkgs);

void add_package(sqlite3 *db, char* name, char* version, char* architecture, char* repository, char* description, long installdate, int size);

int get_package(sqlite3 *db, char* name, struct repo_package* out_pkg);

void update_package(sqlite3 *db, struct repo_package pkg);

#endif