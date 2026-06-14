#include "models/PriceHistory.h"

nlohmann::json PriceHistory::toJson() const {
    return {
        {"id", id},
        {"flight_id", flightId},
        {"price", price},
        {"record_time", recordTime}
    };
}

PriceHistory PriceHistory::fromRow(sqlite3_stmt* stmt) {
    PriceHistory h;
    h.id = sqlite3_column_int(stmt, 0);
    h.flightId = sqlite3_column_int(stmt, 1);
    h.price = sqlite3_column_double(stmt, 2);
    if (sqlite3_column_text(stmt, 3)) {
        h.recordTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    }
    return h;
}
