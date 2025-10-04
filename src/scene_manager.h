#pragma once

#include "scene.h"
#include <memory>
#include <array>

enum class GameState {
    EXPLORATION,
    BATTLE,
    MENU,
    DIALOG,
    SHOP
};

class SceneManager {
public:
    SceneManager();

    // Scene registration - called once during initialization
    void registerScene(GameState state, std::unique_ptr<Scene> scene);

    // State transitions
    void changeState(GameState newState);
    GameState getCurrentState() const { return m_currentState; }
    GameState getPreviousState() const { return m_previousState; }

    // Scene access
    Scene* getScene(GameState state);
    Scene* getCurrentScene() { return m_currentScene; }
    const Scene* getCurrentScene() const { return m_currentScene; }

    // Main loop
    void update(float deltaTime);
    void draw();

private:
    std::array<std::unique_ptr<Scene>, 5> m_scenes;
    Scene* m_currentScene;
    GameState m_currentState;
    GameState m_previousState;
};
