#pragma once

#include <string>
#include <vector>
#include <functional>
#include <sqlite3.h>

class Database {
public:
    explicit Database(const std::string& dbPath);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void initSchema(const std::string& schemaPath);
    void seedMockData();

    bool execute(const std::string& sql);
    bool executeWithCallback(
        const std::string& sql,
        const std::function<bool(sqlite3_stmt*)>& callback,
        const std::vector<std::string>& params = {});

    int64_t lastInsertId() const;
    sqlite3* handle() { return db_; }

private:
    sqlite3* db_ = nullptr;
    std::string dbPath_;

    bool bindParams(sqlite3_stmt* stmt, const std::vector<std::string>& params);
};
