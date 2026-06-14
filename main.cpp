#include "crow.h"
#include "database/Database.h"
#include "services/FlightManager.h"
#include "controllers/FlightController.h"

#include <iostream>
#include <filesystem>

int main() {
    try {
        std::filesystem::create_directories("SQLite");

        Database db("SQLite/flights.db");
        db.initSchema("database/init.sql");
        db.seedMockData();

        FlightManager manager(db);
        FlightController controller(db, manager);

        crow::SimpleApp app;
        controller.registerRoutes(app);

        std::cout << "Flight Price Tracker running at http://localhost:8080" << std::endl;
        app.port(8080).multithreaded().run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
