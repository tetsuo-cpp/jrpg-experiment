cmake_minimum_required(VERSION 3.15)
project(jrpg_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# Game executable
add_executable(jrpg_game
    src/main.cpp
    src/game.cpp
)

target_include_directories(jrpg_game PRIVATE src)
target_link_libraries(jrpg_game PRIVATE raylib)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
