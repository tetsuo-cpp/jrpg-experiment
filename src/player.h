#pragma once

#include "sprite.h"
#include "tilemap.h"
#include <string>

class Player {
public:
    Player(int tileX, int tileY, int tileSize, const std::string& spritePath = "");
    ~Player();

    void update(float deltaTime);
    void render(int cameraOffsetX, int cameraOffsetY);

    void handleInput(const Tilemap& tilemap);

    int getTileX() const { return m_tileX; }
    int getTileY() const { return m_tileY; }
    int getPixelX() const { return m_pixelX; }
    int getPixelY() const { return m_pixelY; }

private:
    void move(int dx, int dy, const Tilemap& tilemap);

    int m_tileX;
    int m_tileY;
    int m_pixelX;
    int m_pixelY;
    int m_tileSize;

    Sprite m_sprite;

    // Movement state
    bool m_isMoving;
    int m_targetX;
    int m_targetY;
    float m_moveProgress;
    static constexpr float MOVE_SPEED = 4.0f; // tiles per second
};
