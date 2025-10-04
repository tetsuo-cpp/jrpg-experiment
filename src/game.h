#pragma once

#include <raylib.h>

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void update();
    void draw();

    // Game state
    bool m_running;

    // Window configuration
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
    static constexpr int TARGET_FPS = 60;
};
