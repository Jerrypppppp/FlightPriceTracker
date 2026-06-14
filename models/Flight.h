#pragma once

#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

class Flight {
public:
    int id = 0;
    std::string flightNumber;
    std::string origin;
    std::string destination;
    std::string departureDate;
    double currentPrice = 0.0;
    std::string createdAt;

    Flight() = default;

    Flight(int id, const std::string& flightNumber, const std::string& origin,
           const std::string& destination, const std::string& departureDate,
           double currentPrice, const std::string& createdAt = "")
        : id(id), flightNumber(flightNumber), origin(origin),
          destination(destination), departureDate(departureDate),
          currentPrice(currentPrice), createdAt(createdAt) {}

    void updatePrice(double newPrice);
    nlohmann::json toJson() const;
    static Flight fromRow(sqlite3_stmt* stmt);
};
