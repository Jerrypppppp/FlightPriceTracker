#include "database/Database.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Database::Database(const std::string& dbPath) : dbPath_(dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + dbPath);
    }
    sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

void Database::initSchema(const std::string& schemaPath) {
    std::ifstream file(schemaPath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open schema file: " + schemaPath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    execute(buffer.str());
}

bool Database::bindParams(sqlite3_stmt* stmt, const std::vector<std::string>& params) {
    for (size_t i = 0; i < params.size(); ++i) {
        if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            return false;
        }
    }
    return true;
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::executeWithCallback(
    const std::string& sql,
    const std::function<bool(sqlite3_stmt*)>& callback,
    const std::vector<std::string>& params) {

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    if (!bindParams(stmt, params)) {
        sqlite3_finalize(stmt);
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (!callback(stmt)) {
            break;
        }
    }

    sqlite3_finalize(stmt);
    return true;
}

int64_t Database::lastInsertId() const {
    return sqlite3_last_insert_rowid(db_);
}

void Database::seedMockData() {
    const char* countSql = "SELECT COUNT(*) FROM flights;";
    int count = 0;
    executeWithCallback(countSql, [&count](sqlite3_stmt* stmt) {
        count = sqlite3_column_int(stmt, 0);
        return false;
    });

    if (count > 0) {
        return;
    }

    struct SeedFlight {
        const char* number;
        const char* origin;
        const char* dest;
        const char* date;
        double price;
        const char* history[3][2];
        int historyCount;
    };

    SeedFlight seeds[] = {
        {"CI100", "TPE", "NRT", "2026-06-14", 6900.0,
         {{"2026-06-10 10:00:00", "8500"}, {"2026-06-12 14:30:00", "8200"}, {"2026-06-14 09:00:00", "6900"}},
         3},
        {"BR198", "TPE", "LAX", "2026-07-01", 28000.0,
         {{"2026-06-01 08:00:00", "32000"}, {"2026-06-10 12:00:00", "28000"}},
         2},
        {"CI101", "TPE", "HKG", "2026-06-20", 4500.0,
         {{"2026-06-05 11:00:00", "5200"}, {"2026-06-15 16:00:00", "4500"}},
         2},
        {"JL97", "TPE", "NRT", "2026-06-18", 9200.0,
         {{"2026-06-08 09:00:00", "9800"}, {"2026-06-14 10:00:00", "9200"}},
         2},
        {"CX450", "TPE", "HKG", "2026-06-25", 3800.0,
         {{"2026-06-10 08:00:00", "4200"}, {"2026-06-14 15:00:00", "3800"}},
         2},
        {"NH852", "TPE", "NRT", "2026-07-05", 7500.0,
         {{"2026-06-12 10:00:00", "8500"}, {"2026-06-14 11:00:00", "7500"}},
         2},
    };

    for (const auto& seed : seeds) {
        std::string insertFlight =
            "INSERT INTO flights (flight_number, origin, destination, departure_date, current_price) "
            "VALUES ('" + std::string(seed.number) + "', '" + seed.origin + "', '" +
            seed.dest + "', '" + seed.date + "', " + std::to_string(seed.price) + ");";
        execute(insertFlight);

        int64_t flightId = lastInsertId();
        for (int i = 0; i < seed.historyCount; ++i) {
            std::string insertHistory =
                "INSERT INTO price_history (flight_id, price, record_time) VALUES (" +
                std::to_string(flightId) + ", " + std::string(seed.history[i][1]) + ", '" +
                seed.history[i][0] + "');";
            execute(insertHistory);
        }
    }

    std::cout << "Mock data seeded successfully." << std::endl;
}
