#pragma once

#include <raylib.h>
#include <memory>
#include "tilemap.h"
#include "player.h"
#include "camera.h"

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void update();
    void draw();
    void initializeMap();

    // Game state
    bool m_running;

    // Window configuration
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
    static constexpr int TARGET_FPS = 60;
    static constexpr int TILE_SIZE = 32;
    static constexpr int MAP_WIDTH = 30;
    static constexpr int MAP_HEIGHT = 20;

    // Game systems
    std::unique_ptr<Tilemap> m_tilemap;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<GameCamera> m_camera;
};
