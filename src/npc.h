#pragma once

#include <string>

class NPC {
public:
    NPC(const std::string& name, int tileX, int tileY, int dialogId, int tileSize);

    // Getters
    const std::string& getName() const { return m_name; }
    int getTileX() const { return m_tileX; }
    int getTileY() const { return m_tileY; }
    int getDialogId() const { return m_dialogId; }

    // Check if player is adjacent (within 1 tile in any cardinal direction)
    bool isPlayerAdjacent(int playerTileX, int playerTileY) const;

    // Rendering
    void render(int cameraOffsetX, int cameraOffsetY) const;

private:
    std::string m_name;
    int m_tileX;
    int m_tileY;
    int m_dialogId;
    int m_tileSize;
    int m_pixelX;
    int m_pixelY;
};
