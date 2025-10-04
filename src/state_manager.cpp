#include "state_manager.h"

StateManager::StateManager()
    : m_currentState(GameState::EXPLORATION)
    , m_previousState(GameState::EXPLORATION)
{
}

void StateManager::changeState(GameState newState) {
    if (m_currentState == newState) {
        return; // Already in this state
    }

    exitCurrentState();
    m_previousState = m_currentState;
    m_currentState = newState;
    enterNewState(newState);
}

void StateManager::exitCurrentState() {
    switch (m_currentState) {
        case GameState::EXPLORATION:
            if (m_explorationExit) m_explorationExit();
            break;
        case GameState::BATTLE:
            if (m_battleExit) m_battleExit();
            break;
        case GameState::MENU:
            if (m_menuExit) m_menuExit();
            break;
        case GameState::DIALOG:
            if (m_dialogExit) m_dialogExit();
            break;
    }
}

void StateManager::enterNewState(GameState newState) {
    switch (newState) {
        case GameState::EXPLORATION:
            if (m_explorationEnter) m_explorationEnter();
            break;
        case GameState::BATTLE:
            if (m_battleEnter) m_battleEnter();
            break;
        case GameState::MENU:
            if (m_menuEnter) m_menuEnter();
            break;
        case GameState::DIALOG:
            if (m_dialogEnter) m_dialogEnter();
            break;
    }
}

void StateManager::setExplorationCallbacks(
    std::function<void()> onEnter,
    std::function<void()> onExit
) {
    m_explorationEnter = onEnter;
    m_explorationExit = onExit;
}

void StateManager::setBattleCallbacks(
    std::function<void()> onEnter,
    std::function<void()> onExit
) {
    m_battleEnter = onEnter;
    m_battleExit = onExit;
}

void StateManager::setMenuCallbacks(
    std::function<void()> onEnter,
    std::function<void()> onExit
) {
    m_menuEnter = onEnter;
    m_menuExit = onExit;
}

void StateManager::setDialogCallbacks(
    std::function<void()> onEnter,
    std::function<void()> onExit
) {
    m_dialogEnter = onEnter;
    m_dialogExit = onExit;
}
