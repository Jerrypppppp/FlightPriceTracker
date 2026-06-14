#pragma once

#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

class PriceHistory {
public:
    int id = 0;
    int flightId = 0;
    double price = 0.0;
    std::string recordTime;

    PriceHistory() = default;

    PriceHistory(int id, int flightId, double price, const std::string& recordTime)
        : id(id), flightId(flightId), price(price), recordTime(recordTime) {}

    nlohmann::json toJson() const;
    static PriceHistory fromRow(sqlite3_stmt* stmt);
};
