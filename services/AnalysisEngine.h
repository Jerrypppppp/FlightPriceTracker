#pragma once

#include "models/Flight.h"
#include "models/PriceHistory.h"

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class AnalysisEngine {
public:
    static double getMinPrice(const std::vector<Flight>& flights);
    static double getMaxPrice(const std::vector<Flight>& flights);
    static double getAveragePrice(const std::vector<Flight>& flights);

    static double getPriceDropPercent(double maxPrice, double currentPrice);
    static double getDiscountPercent(double averagePrice, double currentPrice);

    static bool isSpecialOffer(double averagePrice, double currentPrice);
    static bool isHistoricalLow(double currentPrice, const std::vector<PriceHistory>& history);
    static bool hasConsecutiveDrops(const std::vector<PriceHistory>& history, int minDrops = 2);

    static std::vector<Flight> detectDiscount(const std::vector<Flight>& flights);

    static nlohmann::json buildChartData(const std::vector<PriceHistory>& history);
    static nlohmann::json buildAlertTags(const Flight& flight,
                                         const std::vector<PriceHistory>& history,
                                         double marketAverage);

    static nlohmann::json analyzeFlight(const Flight& flight,
                                        const std::vector<PriceHistory>& history);

    static nlohmann::json buildStats(const std::vector<Flight>& flights,
                                     const std::vector<std::vector<PriceHistory>>& histories);
};
