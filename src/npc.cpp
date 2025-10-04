#include "npc.h"
#include <raylib.h>
#include <cmath>

NPC::NPC(const std::string& name, int tileX, int tileY, int dialogId, int tileSize)
    : m_name(name)
    , m_tileX(tileX)
    , m_tileY(tileY)
    , m_dialogId(dialogId)
    , m_tileSize(tileSize)
{
    // Convert tile coordinates to pixel coordinates
    m_pixelX = tileX * tileSize;
    m_pixelY = tileY * tileSize;
}

bool NPC::isPlayerAdjacent(int playerTileX, int playerTileY) const {
    // Check if player is exactly 1 tile away in a cardinal direction (no diagonals)
    int dx = std::abs(playerTileX - m_tileX);
    int dy = std::abs(playerTileY - m_tileY);

    // Adjacent means either (dx=1, dy=0) or (dx=0, dy=1)
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

void NPC::render(int cameraOffsetX, int cameraOffsetY) const {
    // Calculate screen position (same pattern as Player)
    int screenX = m_pixelX - cameraOffsetX + 2;
    int screenY = m_pixelY - cameraOffsetY + 2;

    // Draw NPC as a blue rectangle (same size as player: tileSize - 4)
    int npcSize = m_tileSize - 4;
    DrawRectangle(screenX, screenY, npcSize, npcSize, BLUE);

    // Draw a simple face-like pattern to distinguish from walls
    int eyeSize = npcSize / 8;
    int eyeOffsetX = npcSize / 4;
    int eyeOffsetY = npcSize / 3;
    DrawCircle(screenX + eyeOffsetX, screenY + eyeOffsetY, eyeSize, WHITE);
    DrawCircle(screenX + npcSize - eyeOffsetX, screenY + eyeOffsetY, eyeSize, WHITE);

    // Draw name label above NPC
    int textWidth = MeasureText(m_name.c_str(), 10);
    DrawText(m_name.c_str(), screenX + (npcSize - textWidth) / 2, screenY - 15, 10, WHITE);
}
