#pragma once

#include <functional>

enum class GameState {
    EXPLORATION,
    BATTLE,
    MENU,
    DIALOG
};

class StateManager {
public:
    StateManager();

    // State management
    void changeState(GameState newState);
    GameState getCurrentState() const { return m_currentState; }
    GameState getPreviousState() const { return m_previousState; }

    // State callbacks
    void setExplorationCallbacks(
        std::function<void()> onEnter,
        std::function<void()> onExit
    );

    void setBattleCallbacks(
        std::function<void()> onEnter,
        std::function<void()> onExit
    );

    void setMenuCallbacks(
        std::function<void()> onEnter,
        std::function<void()> onExit
    );

    void setDialogCallbacks(
        std::function<void()> onEnter,
        std::function<void()> onExit
    );

    // State checks
    bool isInExploration() const { return m_currentState == GameState::EXPLORATION; }
    bool isInBattle() const { return m_currentState == GameState::BATTLE; }
    bool isInMenu() const { return m_currentState == GameState::MENU; }
    bool isInDialog() const { return m_currentState == GameState::DIALOG; }

private:
    void exitCurrentState();
    void enterNewState(GameState newState);

    GameState m_currentState;
    GameState m_previousState;

    // Callbacks for each state
    std::function<void()> m_explorationEnter;
    std::function<void()> m_explorationExit;
    std::function<void()> m_battleEnter;
    std::function<void()> m_battleExit;
    std::function<void()> m_menuEnter;
    std::function<void()> m_menuExit;
    std::function<void()> m_dialogEnter;
    std::function<void()> m_dialogExit;
};
