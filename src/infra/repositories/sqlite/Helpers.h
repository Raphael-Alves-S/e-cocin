#ifndef ECOCIN_INFRA_REPOSITORIES_SQLITE_HELPERS_H
#define ECOCIN_INFRA_REPOSITORIES_SQLITE_HELPERS_H

#include <sqlite3.h>
#include <stdexcept>
#include <string>

inline void sqlite_check(int rc, sqlite3* db, const char* where) {
    if (rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE) {
        throw std::runtime_error(std::string("SQLite error @ ") + where + ": " + sqlite3_errmsg(db));
    }
}

#endif // ECOCIN_INFRA_REPOSITORIES_SQLITE_HELPERS_H
