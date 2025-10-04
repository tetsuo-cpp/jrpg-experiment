#pragma once

#include <raylib.h>
#include <vector>

class Tilemap {
public:
    Tilemap(int width, int height, int tileSize);
    ~Tilemap();

    void setTile(int x, int y, int tileId);
    int getTile(int x, int y) const;
    bool isWalkable(int x, int y) const;

    void render(int cameraOffsetX, int cameraOffsetY);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getTileSize() const { return m_tileSize; }

private:
    int m_width;
    int m_height;
    int m_tileSize;
    std::vector<std::vector<int>> m_tiles;

    Color getTileColor(int tileId) const;
};
