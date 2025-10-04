#include "game.h"
#include "exploration_scene.h"

Game::Game() : m_running(true) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "JRPG Game");
    SetTargetFPS(TARGET_FPS);

    // Initialize game systems
    m_stateManager = std::make_unique<StateManager>();
    m_sceneManager = std::make_unique<SceneManager>();
    m_party = std::make_unique<Party>();

    initializeGame();
    initializeParty();
}

Game::~Game() {
    CloseWindow();
}

void Game::run() {
    while (m_running && !WindowShouldClose()) {
        update();
        draw();
    }
}

void Game::update() {
    float deltaTime = GetFrameTime();

    // Update based on current game state
    if (m_stateManager->isInExploration()) {
        m_sceneManager->update(deltaTime);
    }
    else if (m_stateManager->isInBattle()) {
        // Battle update logic will go here in Phase 3
    }
    else if (m_stateManager->isInMenu()) {
        // Menu update logic will go here in Phase 5
    }
    else if (m_stateManager->isInDialog()) {
        // Dialog update logic will go here in Phase 5
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw based on current game state
    if (m_stateManager->isInExploration()) {
        m_sceneManager->draw();
    }
    else if (m_stateManager->isInBattle()) {
        // Battle draw logic will go here in Phase 3
    }
    else if (m_stateManager->isInMenu()) {
        // Menu draw logic will go here in Phase 5
    }
    else if (m_stateManager->isInDialog()) {
        // Dialog draw logic will go here in Phase 5
    }

    // Draw FPS counter
    DrawFPS(SCREEN_WIDTH - 80, 10);

    EndDrawing();
}

void Game::initializeGame() {
    // Register scenes
    m_sceneManager->registerScene("exploration", [this]() {
        return std::make_unique<ExplorationScene>(
            SCREEN_WIDTH, SCREEN_HEIGHT, TILE_SIZE, MAP_WIDTH, MAP_HEIGHT
        );
    });

    // Start in exploration scene
    m_sceneManager->changeScene("exploration");
}

void Game::initializeParty() {
    // Create initial party member (the player character)
    auto hero = std::make_unique<PartyMember>("Hero", CharacterClass::WARRIOR, 1);
    m_party->addMember(std::move(hero));
}
