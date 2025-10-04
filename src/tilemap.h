#pragma once

#include <raylib.h>
#include <vector>
#include <string>

class Tilemap {
public:
    Tilemap(int width, int height, int tileSize);
    ~Tilemap();

    void setTile(int x, int y, int tileId);
    int getTile(int x, int y) const;
    bool isWalkable(int x, int y) const;

    // Load tileset texture
    void loadTileset(const std::string& tilesetPath, int tilesPerRow);

    void render(int cameraOffsetX, int cameraOffsetY);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getTileSize() const { return m_tileSize; }

private:
    int m_width;
    int m_height;
    int m_tileSize;
    std::vector<std::vector<int>> m_tiles;

    // Tileset support
    Texture2D m_tileset;
    bool m_hasTileset;
    int m_tilesPerRow;

    Color getTileColor(int tileId) const;
};
