#include "sprite.h"

Sprite::Sprite(int width, int height, Color color)
    : m_width(width), m_height(height), m_color(color),
      m_currentDirection(Direction::DOWN), m_isAnimating(false),
      m_currentFrame(0), m_frameTimer(0.0f) {}

Sprite::~Sprite() {}

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

void Sprite::render(int x, int y) {
    // For now, render as a simple colored rectangle
    // Later this will use sprite sheets
    DrawRectangle(x, y, m_width, m_height, m_color);

    // Add a simple border to make it more visible
    DrawRectangleLines(x, y, m_width, m_height, BLACK);

    // Draw direction indicator (small triangle)
    Vector2 center = {x + m_width / 2.0f, y + m_height / 2.0f};
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

void Sprite::setDirection(Direction direction) {
    m_currentDirection = direction;
}

void Sprite::setAnimating(bool animating) {
    m_isAnimating = animating;
}
