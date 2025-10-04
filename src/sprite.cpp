#include "sprite.h"
#include <iostream>

Sprite::Sprite(int width, int height, Color color)
    : m_frameWidth(width), m_frameHeight(height), m_color(color),
      m_hasTexture(false), m_currentDirection(Direction::DOWN),
      m_isAnimating(false), m_currentFrame(0), m_frameTimer(0.0f) {
    m_texture = {0}; // Initialize empty texture
}

Sprite::Sprite(const std::string& texturePath, int frameWidth, int frameHeight)
    : m_frameWidth(frameWidth), m_frameHeight(frameHeight), m_color(WHITE),
      m_hasTexture(false), m_currentDirection(Direction::DOWN),
      m_isAnimating(false), m_currentFrame(0), m_frameTimer(0.0f) {

    // Try to load texture
    m_texture = LoadTexture(texturePath.c_str());

    if (m_texture.id > 0) {
        m_hasTexture = true;
    } else {
        std::cerr << "Failed to load sprite texture: " << texturePath << std::endl;
        m_texture = {0};
    }
}

Sprite::~Sprite() {
    if (m_hasTexture && m_texture.id > 0) {
        UnloadTexture(m_texture);
    }
}

void Sprite::update(float deltaTime) {
    if (m_isAnimating) {
        m_frameTimer += deltaTime;

        if (m_frameTimer >= FRAME_TIME) {
            m_frameTimer = 0.0f;
            m_currentFrame = (m_currentFrame + 1) % FRAMES_PER_ANIMATION;
        }
    } else {
        m_currentFrame = 0;
        m_frameTimer = 0.0f;
    }
}

void Sprite::render(int x, int y) const {
    if (m_hasTexture) {
        // Render from sprite sheet
        // Sprite sheet layout: 2 columns (frames) x 4 rows (directions)
        int row = static_cast<int>(m_currentDirection);
        int col = m_currentFrame;

        Rectangle sourceRect = {
            static_cast<float>(col * m_frameWidth),
            static_cast<float>(row * m_frameHeight),
            static_cast<float>(m_frameWidth),
            static_cast<float>(m_frameHeight)
        };

        Rectangle destRect = {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(m_frameWidth),
            static_cast<float>(m_frameHeight)
        };

        DrawTexturePro(m_texture, sourceRect, destRect, {0, 0}, 0.0f, WHITE);
    } else {
        // Fallback: render as a simple colored rectangle
        DrawRectangle(x, y, m_frameWidth, m_frameHeight, m_color);

        // Add a simple border to make it more visible
        DrawRectangleLines(x, y, m_frameWidth, m_frameHeight, BLACK);

        // Draw direction indicator (small triangle)
        Vector2 center = {x + m_frameWidth / 2.0f, y + m_frameHeight / 2.0f};
        int indicatorSize = 4;

        switch (m_currentDirection) {
            case Direction::DOWN:
                DrawTriangle(
                    {center.x, center.y + indicatorSize},
                    {center.x + indicatorSize, center.y - indicatorSize},
                    {center.x - indicatorSize, center.y - indicatorSize},
                    BLACK
                );
                break;
            case Direction::UP:
                DrawTriangle(
                    {center.x, center.y - indicatorSize},
                    {center.x - indicatorSize, center.y + indicatorSize},
                    {center.x + indicatorSize, center.y + indicatorSize},
                    BLACK
                );
                break;
            case Direction::LEFT:
                DrawTriangle(
                    {center.x - indicatorSize, center.y},
                    {center.x + indicatorSize, center.y + indicatorSize},
                    {center.x + indicatorSize, center.y - indicatorSize},
                    BLACK
                );
                break;
            case Direction::RIGHT:
                DrawTriangle(
                    {center.x + indicatorSize, center.y},
                    {center.x - indicatorSize, center.y - indicatorSize},
                    {center.x - indicatorSize, center.y + indicatorSize},
                    BLACK
                );
                break;
        }
    }
}

void Sprite::setDirection(Direction direction) {
    m_currentDirection = direction;
}

void Sprite::setAnimating(bool animating) {
    m_isAnimating = animating;
}
