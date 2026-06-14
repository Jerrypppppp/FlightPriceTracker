#include "models/Flight.h"

void Flight::updatePrice(double newPrice) {
    currentPrice = newPrice;
}

nlohmann::json Flight::toJson() const {
    return {
        {"id", id},
        {"flight_number", flightNumber},
        {"origin", origin},
        {"destination", destination},
        {"departure_date", departureDate},
        {"current_price", currentPrice},
        {"created_at", createdAt}
    };
}

Flight Flight::fromRow(sqlite3_stmt* stmt) {
    Flight f;
    f.id = sqlite3_column_int(stmt, 0);
    f.flightNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    f.origin = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    f.destination = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    f.departureDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    f.currentPrice = sqlite3_column_double(stmt, 5);
    if (sqlite3_column_text(stmt, 6)) {
        f.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    }
    return f;
}
