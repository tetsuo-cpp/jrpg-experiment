#include "exploration_scene.h"
#include <raylib.h>

ExplorationScene::ExplorationScene(int screenWidth, int screenHeight, int tileSize, int mapWidth, int mapHeight)
    : m_name("exploration")
    , m_screenWidth(screenWidth)
    , m_screenHeight(screenHeight)
    , m_tileSize(tileSize)
    , m_mapWidth(mapWidth)
    , m_mapHeight(mapHeight)
{
    m_tilemap = std::make_unique<Tilemap>(mapWidth, mapHeight, tileSize);
    m_player = std::make_unique<Player>(mapWidth / 2, mapHeight / 2, tileSize);
    m_camera = std::make_unique<GameCamera>(screenWidth, screenHeight, mapWidth, mapHeight, tileSize);

    initializeMap();
}

void ExplorationScene::onEnter() {
    // Called when entering exploration mode
    // Could be used to resume music, reset states, etc.
}

void ExplorationScene::onExit() {
    // Called when leaving exploration mode
    // Could be used to pause music, save state, etc.
}

void ExplorationScene::update(float deltaTime) {
    // Handle input and update player
    m_player->handleInput(*m_tilemap);
    m_player->update(deltaTime);

    // Update camera to follow player
    m_camera->followPlayer(
        m_player->getPixelX(),
        m_player->getPixelY(),
        m_tileSize,
        m_tileSize
    );
}

void ExplorationScene::draw() {
    // Draw game elements with camera offset
    int camX = m_camera->getOffsetX();
    int camY = m_camera->getOffsetY();

    m_tilemap->render(camX, camY);
    m_player->render(camX, camY);

    // Draw exploration UI
    DrawText("Exploration Mode", 10, 10, 20, WHITE);
    DrawText("WASD/Arrows to move", 10, 35, 16, LIGHTGRAY);
}

void ExplorationScene::initializeMap() {
    // Create a simple test map with walls around the border
    for (int y = 0; y < m_mapHeight; y++) {
        for (int x = 0; x < m_mapWidth; x++) {
            // Border walls
            if (x == 0 || x == m_mapWidth - 1 || y == 0 || y == m_mapHeight - 1) {
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
            else if (y == m_mapHeight / 2 && x >= 3 && x < m_mapWidth - 3) {
                m_tilemap->setTile(x, y, 3); // Path
            }
            // Default grass
            else {
                m_tilemap->setTile(x, y, 0); // Grass
            }
        }
    }
}
