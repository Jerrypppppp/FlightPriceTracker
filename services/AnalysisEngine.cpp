#include "services/AnalysisEngine.h"

#include <algorithm>
#include <numeric>

double AnalysisEngine::getMinPrice(const std::vector<Flight>& flights) {
    if (flights.empty()) return 0.0;
    return std::min_element(flights.begin(), flights.end(),
                            [](const Flight& a, const Flight& b) {
                                return a.currentPrice < b.currentPrice;
                            })->currentPrice;
}

double AnalysisEngine::getMaxPrice(const std::vector<Flight>& flights) {
    if (flights.empty()) return 0.0;
    return std::max_element(flights.begin(), flights.end(),
                            [](const Flight& a, const Flight& b) {
                                return a.currentPrice < b.currentPrice;
                            })->currentPrice;
}

double AnalysisEngine::getAveragePrice(const std::vector<Flight>& flights) {
    if (flights.empty()) return 0.0;
    double sum = std::accumulate(flights.begin(), flights.end(), 0.0,
                                 [](double acc, const Flight& f) { return acc + f.currentPrice; });
    return sum / static_cast<double>(flights.size());
}

double AnalysisEngine::getPriceDropPercent(double maxPrice, double currentPrice) {
    if (maxPrice <= 0) return 0.0;
    return (maxPrice - currentPrice) / maxPrice * 100.0;
}

double AnalysisEngine::getDiscountPercent(double averagePrice, double currentPrice) {
    if (averagePrice <= 0) return 0.0;
    return (averagePrice - currentPrice) / averagePrice * 100.0;
}

bool AnalysisEngine::isSpecialOffer(double averagePrice, double currentPrice) {
    return currentPrice < averagePrice * 0.8;
}

bool AnalysisEngine::isHistoricalLow(double currentPrice, const std::vector<PriceHistory>& history) {
    if (history.empty()) {
        return true;
    }
    for (const auto& h : history) {
        if (h.price < currentPrice) {
            return false;
        }
    }
    return true;
}

bool AnalysisEngine::hasConsecutiveDrops(const std::vector<PriceHistory>& history, int minDrops) {
    if (history.size() < static_cast<size_t>(minDrops + 1)) {
        return false;
    }

    int consecutive = 0;
    for (size_t i = 1; i < history.size(); ++i) {
        if (history[i].price < history[i - 1].price) {
            ++consecutive;
            if (consecutive >= minDrops) {
                return true;
            }
        } else {
            consecutive = 0;
        }
    }
    return false;
}

std::vector<Flight> AnalysisEngine::detectDiscount(const std::vector<Flight>& flights) {
    double avg = getAveragePrice(flights);
    std::vector<Flight> discounted;
    for (const auto& f : flights) {
        if (isSpecialOffer(avg, f.currentPrice)) {
            discounted.push_back(f);
        }
    }
    std::sort(discounted.begin(), discounted.end(),
              [](const Flight& a, const Flight& b) { return a.currentPrice < b.currentPrice; });
    return discounted;
}

nlohmann::json AnalysisEngine::buildChartData(const std::vector<PriceHistory>& history) {
    nlohmann::json labels = nlohmann::json::array();
    nlohmann::json prices = nlohmann::json::array();

    for (const auto& h : history) {
        std::string label = h.recordTime;
        if (label.size() >= 10) {
            label = label.substr(0, 10);
        }
        labels.push_back(label);
        prices.push_back(h.price);
    }

    return {{"labels", labels}, {"prices", prices}};
}

nlohmann::json AnalysisEngine::buildAlertTags(const Flight& flight,
                                              const std::vector<PriceHistory>& history,
                                              double marketAverage) {
    nlohmann::json tags = nlohmann::json::array();

    if (isSpecialOffer(marketAverage, flight.currentPrice)) {
        tags.push_back({
            {"type", "special_offer"},
            {"label", "★★★★ 優惠警報 ★★★★"},
            {"detail", "目前價格低於市場平均 20% 以上"}
        });
    }

    if (isHistoricalLow(flight.currentPrice, history)) {
        tags.push_back({
            {"type", "historical_low"},
            {"label", "📉 歷史新低"},
            {"detail", "目前價格為追蹤以來最低紀錄"}
        });
    }

    if (hasConsecutiveDrops(history, 2)) {
        tags.push_back({
            {"type", "consecutive_drop"},
            {"label", "🔻 連續下跌"},
            {"detail", "近期價格連續 2 次以上調降"}
        });
    }

    return tags;
}

nlohmann::json AnalysisEngine::analyzeFlight(const Flight& flight,
                                             const std::vector<PriceHistory>& history) {
    double minPrice = flight.currentPrice;
    double maxPrice = flight.currentPrice;

    for (const auto& h : history) {
        minPrice = std::min(minPrice, h.price);
        maxPrice = std::max(maxPrice, h.price);
    }

    double avgPrice = 0.0;
    if (!history.empty()) {
        double sum = std::accumulate(history.begin(), history.end(), 0.0,
                                     [](double acc, const PriceHistory& h) { return acc + h.price; });
        avgPrice = sum / static_cast<double>(history.size());
    } else {
        avgPrice = flight.currentPrice;
    }

    double dropPercent = getPriceDropPercent(maxPrice, flight.currentPrice);
    bool special = isSpecialOffer(avgPrice, flight.currentPrice);
    bool historicalLow = isHistoricalLow(flight.currentPrice, history);
    bool consecutiveDrop = hasConsecutiveDrops(history, 2);

    nlohmann::json historyJson = nlohmann::json::array();
    for (const auto& h : history) {
        historyJson.push_back(h.toJson());
    }

    return {
        {"flight", flight.toJson()},
        {"min_price", minPrice},
        {"max_price", maxPrice},
        {"average_price", avgPrice},
        {"price_drop_percent", dropPercent},
        {"discount_percent", getDiscountPercent(avgPrice, flight.currentPrice)},
        {"is_special", special},
        {"is_historical_low", historicalLow},
        {"has_consecutive_drops", consecutiveDrop},
        {"alert_tags", buildAlertTags(flight, history, avgPrice)},
        {"chart", buildChartData(history)},
        {"history", historyJson}
    };
}

nlohmann::json AnalysisEngine::buildStats(const std::vector<Flight>& flights,
                                          const std::vector<std::vector<PriceHistory>>& histories) {
    if (flights.empty()) {
        return {
            {"total_flights", 0},
            {"average_price", 0},
            {"min_price", 0},
            {"max_price", 0},
            {"discount_count", 0},
            {"historical_low_count", 0},
            {"consecutive_drop_count", 0},
            {"cheapest_flight", nullptr},
            {"discounts", nlohmann::json::array()},
            {"advanced_alerts", nlohmann::json::array()},
            {"ranking", nlohmann::json::array()}
        };
    }

    double avg = getAveragePrice(flights);
    auto discounted = detectDiscount(flights);

    auto cheapestIt = std::min_element(flights.begin(), flights.end(),
                                       [](const Flight& a, const Flight& b) {
                                           return a.currentPrice < b.currentPrice;
                                       });

    int historicalLowCount = 0;
    int consecutiveDropCount = 0;
    nlohmann::json discountsJson = nlohmann::json::array();
    nlohmann::json advancedAlertsJson = nlohmann::json::array();

    for (size_t i = 0; i < flights.size(); ++i) {
        const auto& f = flights[i];
        const auto& history = (i < histories.size()) ? histories[i] : std::vector<PriceHistory>{};
        auto tags = buildAlertTags(f, history, avg);

        if (isHistoricalLow(f.currentPrice, history)) {
            ++historicalLowCount;
        }
        if (hasConsecutiveDrops(history, 2)) {
            ++consecutiveDropCount;
        }

        if (!tags.empty()) {
            advancedAlertsJson.push_back({
                {"flight_number", f.flightNumber},
                {"origin", f.origin},
                {"destination", f.destination},
                {"departure_date", f.departureDate},
                {"current_price", f.currentPrice},
                {"average_price", avg},
                {"tags", tags}
            });
        }

        if (isSpecialOffer(avg, f.currentPrice)) {
            discountsJson.push_back({
                {"flight_number", f.flightNumber},
                {"origin", f.origin},
                {"destination", f.destination},
                {"departure_date", f.departureDate},
                {"current_price", f.currentPrice},
                {"average_price", avg},
                {"discount_percent", getDiscountPercent(avg, f.currentPrice)},
                {"is_special", true},
                {"alert", "★★★★ 優惠警報 ★★★★"}
            });
        }
    }

    std::vector<Flight> ranking = flights;
    std::sort(ranking.begin(), ranking.end(),
              [](const Flight& a, const Flight& b) { return a.currentPrice < b.currentPrice; });

    nlohmann::json rankingJson = nlohmann::json::array();
    for (const auto& f : ranking) {
        rankingJson.push_back({
            {"flight_number", f.flightNumber},
            {"origin", f.origin},
            {"destination", f.destination},
            {"departure_date", f.departureDate},
            {"current_price", f.currentPrice}
        });
    }

    return {
        {"total_flights", flights.size()},
        {"average_price", avg},
        {"min_price", getMinPrice(flights)},
        {"max_price", getMaxPrice(flights)},
        {"discount_count", discounted.size()},
        {"historical_low_count", historicalLowCount},
        {"consecutive_drop_count", consecutiveDropCount},
        {"cheapest_flight", cheapestIt->toJson()},
        {"discounts", discountsJson},
        {"advanced_alerts", advancedAlertsJson},
        {"ranking", rankingJson}
    };
}
