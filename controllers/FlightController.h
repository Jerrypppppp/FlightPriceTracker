#pragma once

#include "database/Database.h"
#include "services/FlightManager.h"
#include "services/AnalysisEngine.h"

#include <crow.h>
#include <string>

class FlightController {
public:
    FlightController(Database& db, FlightManager& manager);

    void registerRoutes(crow::SimpleApp& app);

private:
    Database& db_;
    FlightManager& manager_;

    std::string readFile(const std::string& path) const;
    crow::response jsonResponse(int code, const nlohmann::json& body) const;
    crow::response htmlResponse(const std::string& path) const;
};
