#pragma once

#include <raylib.h>
#include <vector>

enum class Direction {
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3
};

class Sprite {
public:
    Sprite(int width, int height, Color color);
    ~Sprite();

    void update(float deltaTime);
    void render(int x, int y);

    void setDirection(Direction direction);
    void setAnimating(bool animating);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width;
    int m_height;
    Color m_color;

    Direction m_currentDirection;
    bool m_isAnimating;

    int m_currentFrame;
    float m_frameTimer;
    static constexpr float FRAME_TIME = 0.15f;
    static constexpr int FRAMES_PER_ANIMATION = 4;
};
