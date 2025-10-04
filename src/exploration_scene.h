#pragma once

#include "scene.h"
#include "tilemap.h"
#include "player.h"
#include "camera.h"
#include "scene_manager.h"
#include "party.h"
#include <memory>

class ExplorationScene : public Scene {
public:
    ExplorationScene(int screenWidth, int screenHeight, int tileSize, int mapWidth, int mapHeight,
                     SceneManager* sceneManager, Party* party);
    ~ExplorationScene() override = default;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void draw() override;
    const std::string& getName() const override { return m_name; }

    // Accessors for external systems that may need to interact with exploration
    Player* getPlayer() { return m_player.get(); }
    Tilemap* getTilemap() { return m_tilemap.get(); }

private:
    void initializeMap();
    void startBattle();

    std::string m_name;
    std::unique_ptr<Tilemap> m_tilemap;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<GameCamera> m_camera;

    int m_screenWidth;
    int m_screenHeight;
    int m_tileSize;
    int m_mapWidth;
    int m_mapHeight;

    // Non-owning pointers to game systems
    SceneManager* m_sceneManager;
    Party* m_party;
};
