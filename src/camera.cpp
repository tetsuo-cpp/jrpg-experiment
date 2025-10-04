#include "camera.h"
#include <algorithm>

GameCamera::GameCamera(int screenWidth, int screenHeight, int mapWidth, int mapHeight, int tileSize)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight),
      m_mapPixelWidth(mapWidth * tileSize), m_mapPixelHeight(mapHeight * tileSize),
      m_tileSize(tileSize), m_offsetX(0), m_offsetY(0) {}

GameCamera::~GameCamera() {}

void GameCamera::followPlayer(int playerPixelX, int playerPixelY, int playerWidth, int playerHeight) {
    // Center camera on player
    int targetOffsetX = playerPixelX + playerWidth / 2 - m_screenWidth / 2;
    int targetOffsetY = playerPixelY + playerHeight / 2 - m_screenHeight / 2;

    // Clamp camera to map boundaries
    m_offsetX = std::max(0, std::min(targetOffsetX, m_mapPixelWidth - m_screenWidth));
    m_offsetY = std::max(0, std::min(targetOffsetY, m_mapPixelHeight - m_screenHeight));

    // If map is smaller than screen, center the map
    if (m_mapPixelWidth < m_screenWidth) {
        m_offsetX = -(m_screenWidth - m_mapPixelWidth) / 2;
    }
    if (m_mapPixelHeight < m_screenHeight) {
        m_offsetY = -(m_screenHeight - m_mapPixelHeight) / 2;
    }
}
