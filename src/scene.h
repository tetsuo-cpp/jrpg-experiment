#pragma once

#include <string>

// Base class for all game scenes (maps, battle arenas, etc.)
class Scene {
public:
    virtual ~Scene() = default;

    // Scene lifecycle
    virtual void onEnter() = 0;
    virtual void onExit() = 0;

    // Main loop
    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;

    // Scene identification
    virtual const std::string& getName() const = 0;

protected:
    Scene() = default;
};
