#pragma once

#include "database/Database.h"
#include "models/Flight.h"
#include "models/PriceHistory.h"

#include <string>
#include <vector>
#include <optional>

class FlightManager {
public:
    explicit FlightManager(Database& db);

    bool addFlight(const std::string& flightNumber, const std::string& origin,
                   const std::string& destination, const std::string& departureDate,
                   double currentPrice);

    bool deleteFlight(int id);
    bool updateFlight(int id, const std::string& departureDate, double currentPrice);

    std::vector<Flight> getAllFlights();
    std::optional<Flight> getFlightById(int id);
    std::vector<Flight> searchFlight(const std::string& keyword);

    std::vector<Flight> sortFlights(std::vector<Flight> flights,
                                    const std::string& sortBy);

    std::vector<PriceHistory> getPriceHistory(int flightId);

private:
    Database& db_;

    void addPriceHistory(int flightId, double price);
};
