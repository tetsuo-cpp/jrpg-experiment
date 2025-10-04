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
    src/tilemap.cpp
    src/sprite.cpp
    src/player.cpp
    src/camera.cpp
    src/character_stats.cpp
    src/party_member.cpp
    src/party.cpp
    src/scene_manager.cpp
    src/exploration_scene.cpp
    src/enemy.cpp
    src/enemy_formation.cpp
    src/battle_scene.cpp
    src/menu_scene.cpp
    src/dialog_scene.cpp
    src/item.cpp
    src/inventory.cpp
    src/equipment.cpp
    src/skill.cpp
    src/npc.cpp
)

target_include_directories(jrpg_game PRIVATE src include)
target_link_libraries(jrpg_game PRIVATE raylib)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
