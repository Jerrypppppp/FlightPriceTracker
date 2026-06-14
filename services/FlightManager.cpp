#include "services/FlightManager.h"

#include <algorithm>
#include <sstream>

FlightManager::FlightManager(Database& db) : db_(db) {}

void FlightManager::addPriceHistory(int flightId, double price) {
    std::string sql = "INSERT INTO price_history (flight_id, price) VALUES (?, ?);";
    std::ostringstream priceStr;
    priceStr << price;
    db_.executeWithCallback(sql, [](sqlite3_stmt*) { return false; },
                            {std::to_string(flightId), priceStr.str()});
}

bool FlightManager::addFlight(const std::string& flightNumber, const std::string& origin,
                              const std::string& destination, const std::string& departureDate,
                              double currentPrice) {
    std::string sql =
        "INSERT INTO flights (flight_number, origin, destination, departure_date, current_price) "
        "VALUES (?, ?, ?, ?, ?);";

    std::ostringstream priceStr;
    priceStr << currentPrice;

    if (!db_.executeWithCallback(sql, [](sqlite3_stmt*) { return false; },
                                 {flightNumber, origin, destination, departureDate, priceStr.str()})) {
        return false;
    }

    addPriceHistory(static_cast<int>(db_.lastInsertId()), currentPrice);
    return true;
}

bool FlightManager::deleteFlight(int id) {
    std::string sql = "DELETE FROM flights WHERE id = ?;";
    return db_.executeWithCallback(sql, [](sqlite3_stmt*) { return false; },
                                   {std::to_string(id)});
}

bool FlightManager::updateFlight(int id, const std::string& departureDate, double currentPrice) {
    auto existing = getFlightById(id);
    if (!existing) {
        return false;
    }

    std::ostringstream priceStr;
    priceStr << currentPrice;

    std::string sql =
        "UPDATE flights SET departure_date = ?, current_price = ? WHERE id = ?;";

    if (!db_.executeWithCallback(sql, [](sqlite3_stmt*) { return false; },
                                 {departureDate, priceStr.str(), std::to_string(id)})) {
        return false;
    }

    if (existing->currentPrice != currentPrice) {
        addPriceHistory(id, currentPrice);
    }

    return true;
}

std::vector<Flight> FlightManager::getAllFlights() {
    std::vector<Flight> flights;
    std::string sql =
        "SELECT id, flight_number, origin, destination, departure_date, current_price, created_at "
        "FROM flights ORDER BY id;";

    db_.executeWithCallback(sql, [&flights](sqlite3_stmt* stmt) {
        flights.push_back(Flight::fromRow(stmt));
        return true;
    });

    return flights;
}

std::optional<Flight> FlightManager::getFlightById(int id) {
    std::optional<Flight> result;
    std::string sql =
        "SELECT id, flight_number, origin, destination, departure_date, current_price, created_at "
        "FROM flights WHERE id = ?;";

    db_.executeWithCallback(sql, [&result](sqlite3_stmt* stmt) {
        result = Flight::fromRow(stmt);
        return false;
    }, {std::to_string(id)});

    return result;
}

std::vector<Flight> FlightManager::searchFlight(const std::string& keyword) {
    std::vector<Flight> flights;
    std::string pattern = "%" + keyword + "%";
    std::string sql =
        "SELECT id, flight_number, origin, destination, departure_date, current_price, created_at "
        "FROM flights WHERE flight_number LIKE ? OR origin LIKE ? OR destination LIKE ? "
        "ORDER BY id;";

    db_.executeWithCallback(sql, [&flights](sqlite3_stmt* stmt) {
        flights.push_back(Flight::fromRow(stmt));
        return true;
    }, {pattern, pattern, pattern});

    return flights;
}

std::vector<Flight> FlightManager::sortFlights(std::vector<Flight> flights,
                                               const std::string& sortBy) {
    if (sortBy == "price_asc") {
        std::sort(flights.begin(), flights.end(),
                  [](const Flight& a, const Flight& b) { return a.currentPrice < b.currentPrice; });
    } else if (sortBy == "price_desc") {
        std::sort(flights.begin(), flights.end(),
                  [](const Flight& a, const Flight& b) { return a.currentPrice > b.currentPrice; });
    } else if (sortBy == "date_asc") {
        std::sort(flights.begin(), flights.end(),
                  [](const Flight& a, const Flight& b) { return a.departureDate < b.departureDate; });
    } else if (sortBy == "date_desc") {
        std::sort(flights.begin(), flights.end(),
                  [](const Flight& a, const Flight& b) { return a.departureDate > b.departureDate; });
    }
    return flights;
}

std::vector<PriceHistory> FlightManager::getPriceHistory(int flightId) {
    std::vector<PriceHistory> history;
    std::string sql =
        "SELECT id, flight_id, price, record_time FROM price_history "
        "WHERE flight_id = ? ORDER BY record_time ASC;";

    db_.executeWithCallback(sql, [&history](sqlite3_stmt* stmt) {
        history.push_back(PriceHistory::fromRow(stmt));
        return true;
    }, {std::to_string(flightId)});

    return history;
}
