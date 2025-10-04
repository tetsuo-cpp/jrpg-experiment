#include "game.h"
#include "exploration_scene.h"
#include "battle_scene.h"
#include "enemy.h"
#include "enemy_formation.h"

Game::Game() : m_running(true) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "JRPG Game");
    SetTargetFPS(TARGET_FPS);

    // Initialize game systems
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
    m_sceneManager->update(deltaTime);
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    m_sceneManager->draw();

    // Draw FPS counter
    DrawFPS(SCREEN_WIDTH - 80, 10);

    EndDrawing();
}

void Game::initializeGame() {
    // Create scenes once (they stay alive for the entire game)
    m_sceneManager->registerScene(GameState::EXPLORATION,
        std::make_unique<ExplorationScene>(
            SCREEN_WIDTH, SCREEN_HEIGHT, TILE_SIZE, MAP_WIDTH, MAP_HEIGHT,
            m_sceneManager.get(), m_party.get()
        )
    );

    m_sceneManager->registerScene(GameState::BATTLE,
        std::make_unique<BattleScene>(m_party.get())
    );

    // Start in exploration
    m_sceneManager->changeState(GameState::EXPLORATION);
}

void Game::initializeParty() {
    // Create initial party member (the player character)
    auto hero = std::make_unique<PartyMember>("Hero", CharacterClass::WARRIOR, 1);
    m_party->addMember(std::move(hero));
}
