#pragma once

#include <raylib.h>
#include <memory>
#include "scene_manager.h"
#include "party.h"
#include "inventory.h"

class Game {
public:
    Game();
    ~Game();

    void run();

    // Global systems accessors
    Inventory* getInventory() { return m_inventory.get(); }
    Party* getParty() { return m_party.get(); }

private:
    void update();
    void draw();
    void initializeGame();
    void initializeParty();
    void initializeInventory();

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
    std::unique_ptr<SceneManager> m_sceneManager;
    std::unique_ptr<Party> m_party;
    std::unique_ptr<Inventory> m_inventory;
};
