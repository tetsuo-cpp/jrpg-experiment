#pragma once

#include <raylib.h>
#include <memory>
#include "state_manager.h"
#include "scene_manager.h"
#include "party.h"

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void update();
    void draw();
    void initializeGame();
    void initializeParty();

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
    std::unique_ptr<StateManager> m_stateManager;
    std::unique_ptr<SceneManager> m_sceneManager;
    std::unique_ptr<Party> m_party;
};
