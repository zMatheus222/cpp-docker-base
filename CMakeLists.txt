cmake_minimum_required(VERSION 3.28)
project(GameBase VERSION 1.0.0 LANGUAGES CXX)

# ── C++26 ────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── FetchContent ──────────────────────────────────────────────────────────────
include(FetchContent)

# SFML 3
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SFML)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ── System: SQLite3 ───────────────────────────────────────────────────────────
find_package(SQLite3 REQUIRED)

# ── Sources ───────────────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)

# ── Copy assets to build dir ──────────────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
    COMMENT "Copying assets..."
)
