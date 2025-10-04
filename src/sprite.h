#pragma once

#include <raylib.h>
#include <vector>
#include <string>

enum class Direction {
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3
};

class Sprite {
public:
    // Constructor for color-based placeholder sprites
    Sprite(int width, int height, Color color);

    // Constructor for texture-based sprites
    Sprite(const std::string& texturePath, int frameWidth, int frameHeight);

    ~Sprite();

    void update(float deltaTime);
    void render(int x, int y) const;

    void setDirection(Direction direction);
    void setAnimating(bool animating);

    int getWidth() const { return m_frameWidth; }
    int getHeight() const { return m_frameHeight; }

private:
    int m_frameWidth;
    int m_frameHeight;
    Color m_color;

    // Texture support
    Texture2D m_texture;
    bool m_hasTexture;

    Direction m_currentDirection;
    bool m_isAnimating;

    int m_currentFrame;
    float m_frameTimer;
    static constexpr float FRAME_TIME = 0.15f;
    static constexpr int FRAMES_PER_ANIMATION = 2;
};
