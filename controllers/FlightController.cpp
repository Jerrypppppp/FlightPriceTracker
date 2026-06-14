#include "controllers/FlightController.h"

#include <fstream>
#include <sstream>
#include <iostream>

FlightController::FlightController(Database& db, FlightManager& manager)
    : db_(db), manager_(manager) {}

std::string FlightController::readFile(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

crow::response FlightController::jsonResponse(int code, const nlohmann::json& body) const {
    crow::response res(code, body.dump());
    res.set_header("Content-Type", "application/json");
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

crow::response FlightController::htmlResponse(const std::string& path) const {
    std::string content = readFile(path);
    if (content.empty()) {
        return crow::response(404, "Page not found");
    }
    crow::response res(200, content);
    res.set_header("Content-Type", "text/html; charset=utf-8");
    return res;
}

void FlightController::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/")([this]() {
        return htmlResponse("templates/dashboard.html");
    });

    CROW_ROUTE(app, "/flights")([this]() {
        return htmlResponse("templates/flights.html");
    });

    CROW_ROUTE(app, "/analysis")([this]() {
        return htmlResponse("templates/analysis.html");
    });

    CROW_ROUTE(app, "/statistics")([this]() {
        return htmlResponse("templates/statistics.html");
    });

    CROW_ROUTE(app, "/api/flights").methods(crow::HTTPMethod::Get)([this](const crow::request& req) {
        const char* searchParam = req.url_params.get("search");
        const char* sortParam = req.url_params.get("sort");
        std::string search = searchParam ? searchParam : "";
        std::string sortBy = sortParam ? sortParam : "";

        std::vector<Flight> flights;
        if (!search.empty()) {
            flights = manager_.searchFlight(search);
        } else {
            flights = manager_.getAllFlights();
        }

        if (!sortBy.empty()) {
            flights = manager_.sortFlights(flights, sortBy);
        }

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& f : flights) {
            arr.push_back(f.toJson());
        }
        return jsonResponse(200, arr);
    });

    CROW_ROUTE(app, "/api/flights").methods(crow::HTTPMethod::Post)([this](const crow::request& req) {
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string flightNumber = body.at("flight_number").get<std::string>();
            std::string origin = body.at("origin").get<std::string>();
            std::string destination = body.at("destination").get<std::string>();
            std::string departureDate = body.at("departure_date").get<std::string>();
            double currentPrice = body.at("current_price").get<double>();

            if (manager_.addFlight(flightNumber, origin, destination, departureDate, currentPrice)) {
                return jsonResponse(201, {{"message", "Flight added successfully"}});
            }
            return jsonResponse(500, {{"error", "Failed to add flight"}});
        } catch (const std::exception& e) {
            return jsonResponse(400, {{"error", e.what()}});
        }
    });

    CROW_ROUTE(app, "/api/flights/<int>").methods(crow::HTTPMethod::Put)([this](const crow::request& req, int id) {
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string departureDate = body.at("departure_date").get<std::string>();
            double currentPrice = body.at("current_price").get<double>();

            if (manager_.updateFlight(id, departureDate, currentPrice)) {
                return jsonResponse(200, {{"message", "Flight updated successfully"}});
            }
            return jsonResponse(404, {{"error", "Flight not found"}});
        } catch (const std::exception& e) {
            return jsonResponse(400, {{"error", e.what()}});
        }
    });

    CROW_ROUTE(app, "/api/flights/<int>").methods(crow::HTTPMethod::Delete)([this](int id) {
        if (manager_.deleteFlight(id)) {
            return jsonResponse(200, {{"message", "Flight deleted successfully"}});
        }
        return jsonResponse(404, {{"error", "Flight not found"}});
    });

    CROW_ROUTE(app, "/api/flights/<int>/history")([this](int id) {
        auto flight = manager_.getFlightById(id);
        if (!flight) {
            return jsonResponse(404, {{"error", "Flight not found"}});
        }

        auto history = manager_.getPriceHistory(id);
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& h : history) {
            arr.push_back(h.toJson());
        }
        return jsonResponse(200, arr);
    });

    CROW_ROUTE(app, "/api/flights/<int>/analysis")([this](int id) {
        auto flight = manager_.getFlightById(id);
        if (!flight) {
            return jsonResponse(404, {{"error", "Flight not found"}});
        }

        auto history = manager_.getPriceHistory(id);
        return jsonResponse(200, AnalysisEngine::analyzeFlight(*flight, history));
    });

    CROW_ROUTE(app, "/api/stats")([this]() {
        auto flights = manager_.getAllFlights();
        std::vector<std::vector<PriceHistory>> histories;
        histories.reserve(flights.size());
        for (const auto& f : flights) {
            histories.push_back(manager_.getPriceHistory(f.id));
        }
        return jsonResponse(200, AnalysisEngine::buildStats(flights, histories));
    });
}
