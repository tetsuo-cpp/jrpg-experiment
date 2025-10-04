#include "tilemap.h"
#include <iostream>

Tilemap::Tilemap(int width, int height, int tileSize)
    : m_width(width), m_height(height), m_tileSize(tileSize),
      m_hasTileset(false), m_tilesPerRow(0) {
    m_tiles.resize(height, std::vector<int>(width, 0));
    m_tileset = {0}; // Initialize empty texture
}

Tilemap::~Tilemap() {
    if (m_hasTileset && m_tileset.id > 0) {
        UnloadTexture(m_tileset);
    }
}

void Tilemap::loadTileset(const std::string& tilesetPath, int tilesPerRow) {
    m_tileset = LoadTexture(tilesetPath.c_str());

    if (m_tileset.id > 0) {
        m_hasTileset = true;
        m_tilesPerRow = tilesPerRow;
    } else {
        std::cerr << "Failed to load tileset: " << tilesetPath << std::endl;
        m_hasTileset = false;
    }
}

void Tilemap::setTile(int x, int y, int tileId) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_tiles[y][x] = tileId;
    }
}

int Tilemap::getTile(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return m_tiles[y][x];
    }
    return -1;
}

bool Tilemap::isWalkable(int x, int y) const {
    int tile = getTile(x, y);
    // Tile ID 1 = wall (not walkable)
    // Tile ID 2 = water (not walkable)
    // All other tiles are walkable
    return tile != 1 && tile != 2 && tile != -1;
}

void Tilemap::render(int cameraOffsetX, int cameraOffsetY) {
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            int tileId = m_tiles[y][x];

            Rectangle destRect = {
                static_cast<float>(x * m_tileSize - cameraOffsetX),
                static_cast<float>(y * m_tileSize - cameraOffsetY),
                static_cast<float>(m_tileSize),
                static_cast<float>(m_tileSize)
            };

            if (m_hasTileset) {
                // Render from tileset
                int tileCol = tileId % m_tilesPerRow;
                int tileRow = tileId / m_tilesPerRow;

                Rectangle sourceRect = {
                    static_cast<float>(tileCol * m_tileSize),
                    static_cast<float>(tileRow * m_tileSize),
                    static_cast<float>(m_tileSize),
                    static_cast<float>(m_tileSize)
                };

                DrawTexturePro(m_tileset, sourceRect, destRect, {0, 0}, 0.0f, WHITE);
            } else {
                // Fallback: render as colored rectangles
                Color color = getTileColor(tileId);
                DrawRectangleRec(destRect, color);
                DrawRectangleLinesEx(destRect, 1, ColorAlpha(DARKGRAY, 0.3f));
            }
        }
    }
}

Color Tilemap::getTileColor(int tileId) const {
    switch (tileId) {
        case 0: return GREEN;        // Grass
        case 1: return DARKGRAY;     // Wall
        case 2: return BLUE;         // Water
        case 3: return BROWN;        // Dirt/Path
        default: return MAGENTA;     // Unknown tile
    }
}
