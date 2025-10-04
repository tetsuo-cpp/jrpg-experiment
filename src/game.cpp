#include "game.h"
#include "exploration_scene.h"
#include "battle_scene.h"
#include "menu_scene.h"
#include "dialog_scene.h"
#include "enemy.h"
#include "enemy_formation.h"
#include "item.h"
#include "equipment.h"
#include "skill.h"

Game::Game() : m_running(true) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "JRPG Game");
    SetTargetFPS(TARGET_FPS);

    // Disable ESC key to close window (we use ESC for menus)
    SetExitKey(KEY_NULL);

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

    // Create menu scene
    auto menuScene = std::make_unique<MenuScene>(m_party.get(), m_inventory.get());
    menuScene->setReturnCallback([this]() {
        m_sceneManager->changeState(GameState::EXPLORATION);
    });
    m_sceneManager->registerScene(GameState::MENU, std::move(menuScene));

    // Create dialog scene
    auto dialogScene = std::make_unique<DialogScene>();
    dialogScene->setReturnCallback([this]() {
        m_sceneManager->changeState(GameState::EXPLORATION);
    });

    // Register NPC dialogs
    // Dialog 1: Villager - Simple friendly greeting
    auto villagerDialog = std::make_unique<Dialog>(1);
    villagerDialog->addLine("Hello there, traveler!", "Villager");
    villagerDialog->addLine("Beautiful day, isn't it? The weather has been lovely lately.", "Villager");
    villagerDialog->addLine("Safe travels on your journey!", "Villager");
    dialogScene->registerDialog(std::move(villagerDialog));

    // Dialog 2: Guard - Quest-related with choices
    auto guardDialog = std::make_unique<Dialog>(2);
    guardDialog->addLine("Halt! State your business.", "Guard");
    guardDialog->addLine("We've been on high alert due to monster attacks.", "Guard");
    guardDialog->addLine("Are you here to help defend the town?", "Guard");
    guardDialog->addChoice("Yes, I'll help fight the monsters!", 4);
    guardDialog->addChoice("No, just passing through.", 5);
    dialogScene->registerDialog(std::move(guardDialog));

    // Dialog 3: Merchant - Shop preview
    auto merchantDialog = std::make_unique<Dialog>(3);
    merchantDialog->addLine("Welcome to my shop!", "Merchant");
    merchantDialog->addLine("I've got potions, weapons, armor... all the finest goods!", "Merchant");
    merchantDialog->addLine("Come back when the shop system is ready and I'll sell you anything you need!", "Merchant");
    dialogScene->registerDialog(std::move(merchantDialog));

    // Dialog 4: Guard - Positive response
    auto guardPositive = std::make_unique<Dialog>(4);
    guardPositive->addLine("Excellent! We need brave warriors like you.", "Guard");
    guardPositive->addLine("The monsters have been spotted near the northern border.", "Guard");
    guardPositive->addLine("Good luck out there, hero!", "Guard");
    dialogScene->registerDialog(std::move(guardPositive));

    // Dialog 5: Guard - Negative response
    auto guardNegative = std::make_unique<Dialog>(5);
    guardNegative->addLine("I see... well, be careful on the roads.", "Guard");
    guardNegative->addLine("These are dangerous times.", "Guard");
    dialogScene->registerDialog(std::move(guardNegative));

    m_sceneManager->registerScene(GameState::DIALOG, std::move(dialogScene));

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
    // Add some starting consumable items for testing
    m_inventory->addItem(Item("Potion", "Restores 50 HP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_HP, 50, 50, 25), 5);
    m_inventory->addItem(Item("Hi-Potion", "Restores 100 HP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_HP, 100, 100, 50), 3);
    m_inventory->addItem(Item("Ether", "Restores 30 MP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_MP, 30, 80, 40), 3);
    m_inventory->addItem(Item("Elixir", "Fully restores HP and MP",
        ItemType::CONSUMABLE, ItemEffect::RESTORE_BOTH, 9999, 500, 250), 1);

    // Add some equipment for testing
    m_inventory->addItem(Equipment("Iron Sword", "A basic iron sword",
        EquipmentType::WEAPON, 15, 0, 0, 0, 100, 50), 1);
    m_inventory->addItem(Equipment("Steel Sword", "A stronger steel sword",
        EquipmentType::WEAPON, 25, 0, 0, 0, 250, 125), 1);
    m_inventory->addItem(Equipment("Magic Staff", "A staff imbued with magic",
        EquipmentType::WEAPON, 10, 0, 0, 20, 200, 100), 1);

    m_inventory->addItem(Equipment("Leather Armor", "Basic leather protection",
        EquipmentType::ARMOR, 0, 10, 0, 0, 80, 40), 1);
    m_inventory->addItem(Equipment("Chain Mail", "Heavy chain armor",
        EquipmentType::ARMOR, 0, 20, 0, 0, 200, 100), 1);
    m_inventory->addItem(Equipment("Mage Robe", "Robes for magic users",
        EquipmentType::ARMOR, 0, 8, 0, 15, 150, 75), 1);

    m_inventory->addItem(Equipment("Power Ring", "Increases attack power",
        EquipmentType::ACCESSORY, 5, 5, 0, 0, 150, 75), 1);
    m_inventory->addItem(Equipment("HP Ring", "Increases maximum HP",
        EquipmentType::ACCESSORY, 0, 0, 50, 0, 120, 60), 1);
}
