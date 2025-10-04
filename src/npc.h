#pragma once

#include <string>

enum class NPCType {
    DIALOG,     // Triggers dialog
    SHOP        // Triggers shop
};

class NPC {
public:
    NPC(const std::string& name, int tileX, int tileY, int dialogId, int tileSize, NPCType type = NPCType::DIALOG);

    // Getters
    const std::string& getName() const { return m_name; }
    int getTileX() const { return m_tileX; }
    int getTileY() const { return m_tileY; }
    int getDialogId() const { return m_dialogId; }
    NPCType getType() const { return m_type; }

    // Check if player is adjacent (within 1 tile in any cardinal direction)
    bool isPlayerAdjacent(int playerTileX, int playerTileY) const;

    // Rendering
    void render(int cameraOffsetX, int cameraOffsetY) const;

private:
    std::string m_name;
    int m_tileX;
    int m_tileY;
    int m_dialogId;
    NPCType m_type;
    int m_tileSize;
    int m_pixelX;
    int m_pixelY;
};
