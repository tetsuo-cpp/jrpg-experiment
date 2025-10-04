#include "scene_manager.h"

SceneManager::SceneManager()
    : m_currentScene(nullptr)
    , m_currentSceneName("")
{
}

void SceneManager::registerScene(const std::string& name, std::function<std::unique_ptr<Scene>()> factory) {
    m_sceneFactories[name] = factory;
}

bool SceneManager::changeScene(const std::string& name) {
    // Check if scene exists
    auto it = m_sceneFactories.find(name);
    if (it == m_sceneFactories.end()) {
        return false;
    }

    // Exit current scene
    if (m_currentScene) {
        m_currentScene->onExit();
    }

    // Create and enter new scene
    m_currentScene = it->second();
    m_currentSceneName = name;

    if (m_currentScene) {
        m_currentScene->onEnter();
        return true;
    }

    return false;
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
