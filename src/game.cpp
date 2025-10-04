#include "game.h"
#include "exploration_scene.h"
#include "battle_scene.h"
#include "enemy.h"
#include "enemy_formation.h"
#include "item.h"
#include "equipment.h"
#include "skill.h"

Game::Game() : m_running(true) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "JRPG Game");
    SetTargetFPS(TARGET_FPS);

    // Initialize game systems
    m_sceneManager = std::make_unique<SceneManager>();
    m_party = std::make_unique<Party>();
    m_inventory = std::make_unique<Inventory>();

    initializeGame();
    initializeParty();
    initializeInventory();
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
        std::make_unique<BattleScene>(m_party.get(), m_inventory.get())
    );

    // Start in exploration
    m_sceneManager->changeState(GameState::EXPLORATION);
}

void Game::initializeParty() {
    // Create initial party member (the player character)
    auto hero = std::make_unique<PartyMember>("Hero", CharacterClass::WARRIOR, 10);

    // Give the hero some starting skills based on class
    hero->learnSkill(Skill("Power Strike", "A powerful melee attack",
        SkillType::OFFENSIVE_MAGIC, TargetType::SINGLE_ENEMY, 5, 30));

    m_party->addMember(std::move(hero));

    // Add a mage for testing magic
    auto mage = std::make_unique<PartyMember>("Mage", CharacterClass::MAGE, 10);
    mage->learnSkill(Skill("Fire", "Cast a fireball at an enemy",
        SkillType::OFFENSIVE_MAGIC, TargetType::SINGLE_ENEMY, 8, 40));
    mage->learnSkill(Skill("Ice", "Freeze an enemy with ice",
        SkillType::OFFENSIVE_MAGIC, TargetType::SINGLE_ENEMY, 8, 40));
    m_party->addMember(std::move(mage));

    // Add a cleric for testing healing
    auto cleric = std::make_unique<PartyMember>("Cleric", CharacterClass::CLERIC, 10);
    cleric->learnSkill(Skill("Heal", "Restore HP to an ally",
        SkillType::HEALING_MAGIC, TargetType::SINGLE_ALLY, 6, 50));
    cleric->learnSkill(Skill("Cure All", "Restore HP to all allies",
        SkillType::HEALING_MAGIC, TargetType::ALL_ALLIES, 15, 30));
    m_party->addMember(std::move(cleric));
}

void Game::initializeInventory() {
    // Add some starting items for testing
    m_inventory->addItem(Item("Potion", "Restores 50 HP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_HP, 50, 50, 25), 5);
    m_inventory->addItem(Item("Hi-Potion", "Restores 100 HP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_HP, 100, 100, 50), 3);
    m_inventory->addItem(Item("Ether", "Restores 30 MP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_MP, 30, 80, 40), 3);
    m_inventory->addItem(Item("Elixir", "Fully restores HP and MP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_BOTH, 9999, 500, 250), 1);
}
