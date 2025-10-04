#include "game.h"

Game::Game() : m_running(true) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "JRPG Game");
    SetTargetFPS(TARGET_FPS);
}

Game::~Game() {
    CloseWindow();
}

void Game::run() {
    while (m_running && !WindowShouldClose()) {
        update();
        draw();
    }
}

void Game::update() {
    // Game logic updates go here
    // - Handle input
    // - Update player position
    // - Update NPCs
    // - Check collisions
    // - Update game state
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw game elements here
    // - Draw map/tilemap
    // - Draw characters
    // - Draw UI
    // - Draw menus

    DrawText("JRPG Game - Ready for Development!", 190, 280, 20, DARKGRAY);
    DrawFPS(10, 10);

    EndDrawing();
}
