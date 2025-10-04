#include "game.h"

Game::Game() : m_running(true) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "JRPG Game");
    SetTargetFPS(TARGET_FPS);

    // Initialize game systems
    m_tilemap = std::make_unique<Tilemap>(MAP_WIDTH, MAP_HEIGHT, TILE_SIZE);
    m_player = std::make_unique<Player>(MAP_WIDTH / 2, MAP_HEIGHT / 2, TILE_SIZE);
    m_camera = std::make_unique<GameCamera>(SCREEN_WIDTH, SCREEN_HEIGHT, MAP_WIDTH, MAP_HEIGHT, TILE_SIZE);

    initializeMap();
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
    float deltaTime = GetFrameTime();

    // Handle input and update player
    m_player->handleInput(*m_tilemap);
    m_player->update(deltaTime);

    // Update camera to follow player
    m_camera->followPlayer(
        m_player->getPixelX(),
        m_player->getPixelY(),
        TILE_SIZE,
        TILE_SIZE
    );
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw game elements with camera offset
    int camX = m_camera->getOffsetX();
    int camY = m_camera->getOffsetY();

    m_tilemap->render(camX, camY);
    m_player->render(camX, camY);

    // Draw UI
    DrawText("JRPG Game - Phase 1", 10, 10, 20, WHITE);
    DrawText("WASD/Arrows to move", 10, 35, 16, LIGHTGRAY);
    DrawFPS(SCREEN_WIDTH - 80, 10);

    EndDrawing();
}

void Game::initializeMap() {
    // Create a simple test map with walls around the border
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Border walls
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                m_tilemap->setTile(x, y, 1); // Wall
            }
            // Some interior walls to test collision
            else if ((x == 5 && y >= 3 && y <= 10) ||
                     (x == 15 && y >= 8 && y <= 15) ||
                     (y == 10 && x >= 8 && x <= 12)) {
                m_tilemap->setTile(x, y, 1); // Wall
            }
            // Water patches
            else if ((x >= 20 && x <= 22 && y >= 5 && y <= 8) ||
                     (x >= 10 && x <= 11 && y >= 15 && y <= 16)) {
                m_tilemap->setTile(x, y, 2); // Water
            }
            // Path tiles
            else if (y == MAP_HEIGHT / 2 && x >= 3 && x < MAP_WIDTH - 3) {
                m_tilemap->setTile(x, y, 3); // Path
            }
            // Default grass
            else {
                m_tilemap->setTile(x, y, 0); // Grass
            }
        }
    }
}
