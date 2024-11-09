#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "repo.h"
#include "utils.h"

sqlite3* database_connect(char* location) {
    sqlite3* db;

    int rc = sqlite3_open(location, &db);

    if (rc) {
        fprintf(stderr, "Failed to open pkgdb\n");
        exit(1);
    }

    return db;
}

sqlite3* create_database(char* location) {
    if (access(location, F_OK) == 0) {
        return database_connect(location);
    }

    system(catstring("mkdir -p ", VAR_PREFIX, "/eggpm", NULL));

    sqlite3 *db = database_connect(location);

    char* err_msg;
    int rc;

    const char* sql = "CREATE TABLE IF NOT EXISTS `Packages` (`PackageID` INTEGER PRIMARY KEY AUTOINCREMENT , `Name` VARCHAR(50) NOT NULL , `Version` VARCHAR(20) NOT NULL , `Architecture` VARCHAR(10) NOT NULL , `Repository` VARCHAR(100) NOT NULL , `Description` VARCHAR(100) NOT NULL , `InstallDate` VARCHAR(30) NOT NULL , `Size` INT NOT NULL);";

    rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error while creating pkgdb: %s\n", err_msg);
        exit(1);
    }

    return db;
}

void list_all_packages(sqlite3 *db) {
    sqlite3_stmt *res;
    int rc;

    const char *sql = "SELECT PackageID, Name, Version FROM Packages;";

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    printf("Packages:\n");

    while ((rc = sqlite3_step(res)) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        const unsigned char *name = sqlite3_column_text(res, 1);
        const unsigned char *version = sqlite3_column_text(res, 2);

        printf("PackageID: %d | Name: %s | Version: %s\n", id, name, version);
    }

    sqlite3_finalize(res);

}

void add_package(sqlite3 *db, char* name, char* version, char* architecture, char* repository, char* description, char* installdate, int size) {
    sqlite3_stmt *stmt;
    int rc;

    const char *sql = "INSERT INTO Packages (Name, Version, Architecture, Repository, Description, InstallDate, Size) VALUES (?, ?, ?, ?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, version, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, architecture, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, repository, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, installdate, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, size);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("DB Error\n");
        exit(1);
    }

    sqlite3_finalize(stmt);
}

int get_package(sqlite3 *db, char* name, struct repo_package* out_pkg) {
    sqlite3_stmt *stmt;
    int rc;
    struct repo_package pkg;

    const char *sql = "SELECT PackageID, Version, Architecture, Repository, Description, Size FROM Packages WHERE Name = ?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        pkg.version = (char*) sqlite3_column_text(stmt, 1);
        pkg.architecture = (char*) sqlite3_column_text(stmt, 2);
        pkg.repository = (char*) sqlite3_column_text(stmt, 3);
        pkg.description = (char*) sqlite3_column_text(stmt, 4);
        pkg.size = sqlite3_column_int(stmt, 5);

        sqlite3_finalize(stmt);
        if (out_pkg != NULL) {
            *out_pkg = pkg;
        }
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

void update_package(sqlite3 *db, struct repo_package pkg) {
    sqlite3_stmt *stmt;
    int rc;

    const char *sql = "UPDATE Packages SET `Version`=?,`Architecture`=?,`Repository`=?,`Description`=?,`InstallDate`=?,`Size`=? WHERE `Name` = ?";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, pkg.version, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pkg.architecture, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pkg.repository, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, pkg.description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, pkg.installdate, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, pkg.size);
    sqlite3_bind_text(stmt, 7, pkg.name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("DB Error\n");
        exit(1);
    }

    sqlite3_finalize(stmt);
}