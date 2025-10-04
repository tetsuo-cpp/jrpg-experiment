#include "scene_manager.h"

SceneManager::SceneManager()
    : m_currentScene(nullptr)
    , m_currentState(GameState::EXPLORATION)
    , m_previousState(GameState::EXPLORATION)
{
}

void SceneManager::registerScene(GameState state, std::unique_ptr<Scene> scene) {
    m_scenes[static_cast<size_t>(state)] = std::move(scene);
}

void SceneManager::changeState(GameState newState) {
    // Exit current scene
    if (m_currentScene) {
        m_currentScene->onExit();
    }

    // Update state
    m_previousState = m_currentState;
    m_currentState = newState;

    // Get new scene
    m_currentScene = m_scenes[static_cast<size_t>(newState)].get();

    // Enter new scene
    if (m_currentScene) {
        m_currentScene->onEnter();
    }
}

Scene* SceneManager::getScene(GameState state) {
    return m_scenes[static_cast<size_t>(state)].get();
}

void SceneManager::update(float deltaTime) {
    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void SceneManager::draw() {
    if (m_currentScene) {
        m_currentScene->draw();
    }
}
