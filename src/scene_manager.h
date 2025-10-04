#pragma once

#include "scene.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

class SceneManager {
public:
    SceneManager();

    // Scene registration
    void registerScene(const std::string& name, std::function<std::unique_ptr<Scene>()> factory);

    // Scene transitions
    bool changeScene(const std::string& name);
    void update(float deltaTime);
    void draw();

    // Current scene info
    Scene* getCurrentScene() { return m_currentScene.get(); }
    const Scene* getCurrentScene() const { return m_currentScene.get(); }
    const std::string& getCurrentSceneName() const { return m_currentSceneName; }

private:
    std::unique_ptr<Scene> m_currentScene;
    std::string m_currentSceneName;
    std::unordered_map<std::string, std::function<std::unique_ptr<Scene>()>> m_sceneFactories;
};
