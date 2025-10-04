#include "tilemap.h"

Tilemap::Tilemap(int width, int height, int tileSize)
    : m_width(width), m_height(height), m_tileSize(tileSize) {
    m_tiles.resize(height, std::vector<int>(width, 0));
}

Tilemap::~Tilemap() {}

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
            Color color = getTileColor(tileId);

            Rectangle rect = {
                static_cast<float>(x * m_tileSize - cameraOffsetX),
                static_cast<float>(y * m_tileSize - cameraOffsetY),
                static_cast<float>(m_tileSize),
                static_cast<float>(m_tileSize)
            };

            DrawRectangleRec(rect, color);
            DrawRectangleLinesEx(rect, 1, ColorAlpha(DARKGRAY, 0.3f));
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
