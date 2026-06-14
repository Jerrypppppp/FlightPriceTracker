cmake_minimum_required(VERSION 3.16)
project(FlightPriceTracker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "" FORCE)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(Crow json)

find_library(SQLITE3_LIB sqlite3 REQUIRED)

set(SOURCES
    main.cpp
    models/Flight.cpp
    models/PriceHistory.cpp
    database/Database.cpp
    services/FlightManager.cpp
    services/AnalysisEngine.cpp
    controllers/FlightController.cpp
)

add_executable(FlightPriceTracker ${SOURCES})

target_include_directories(FlightPriceTracker PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${ASIO_INCLUDE_DIR}
)

target_link_libraries(FlightPriceTracker PRIVATE
    Crow::Crow
    nlohmann_json::nlohmann_json
    ${SQLITE3_LIB}
    pthread
)

file(COPY ${CMAKE_SOURCE_DIR}/templates DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/static DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/database/init.sql DESTINATION ${CMAKE_BINARY_DIR}/database)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/SQLite)
