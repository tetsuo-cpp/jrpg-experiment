#pragma once

class GameCamera {
public:
    GameCamera(int screenWidth, int screenHeight, int mapWidth, int mapHeight, int tileSize);
    ~GameCamera();

    void followPlayer(int playerPixelX, int playerPixelY, int playerWidth, int playerHeight);

    int getOffsetX() const { return m_offsetX; }
    int getOffsetY() const { return m_offsetY; }

private:
    int m_screenWidth;
    int m_screenHeight;
    int m_mapPixelWidth;
    int m_mapPixelHeight;
    int m_tileSize;

    int m_offsetX;
    int m_offsetY;
};
