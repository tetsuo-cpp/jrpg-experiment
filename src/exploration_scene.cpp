#include "exploration_scene.h"
#include "battle_scene.h"
#include "dialog_scene.h"
#include "enemy.h"
#include "enemy_formation.h"
#include <raylib.h>

ExplorationScene::ExplorationScene(int screenWidth, int screenHeight, int tileSize, int mapWidth, int mapHeight,
                                   SceneManager* sceneManager, Party* party)
    : m_name("exploration")
    , m_screenWidth(screenWidth)
    , m_screenHeight(screenHeight)
    , m_tileSize(tileSize)
    , m_mapWidth(mapWidth)
    , m_mapHeight(mapHeight)
    , m_sceneManager(sceneManager)
    , m_party(party)
{
    m_tilemap = std::make_unique<Tilemap>(mapWidth, mapHeight, tileSize);
    m_player = std::make_unique<Player>(mapWidth / 2, mapHeight / 2, tileSize);
    m_camera = std::make_unique<GameCamera>(screenWidth, screenHeight, mapWidth, mapHeight, tileSize);

    initializeMap();
    initializeNPCs();
}

void ExplorationScene::onEnter() {
    // Called when entering exploration mode
    // Could be used to resume music, reset states, etc.
}

void ExplorationScene::onExit() {
    // Called when leaving exploration mode
    // Could be used to pause music, save state, etc.
}

void ExplorationScene::update(float deltaTime) {
    // Handle input and update player
    m_player->handleInput(*m_tilemap);
    m_player->update(deltaTime);

    // Update camera to follow player
    m_camera->followPlayer(
        m_player->getPixelX(),
        m_player->getPixelY(),
        m_tileSize,
        m_tileSize
    );

    // Check for NPC interaction
    checkNPCInteraction();

    // Press B to trigger a battle (for testing)
    if (IsKeyPressed(KEY_B)) {
        startBattle();
    }

    // Press T to trigger dialog (for testing)
    if (IsKeyPressed(KEY_T)) {
        startDialog();
    }

    // Press ESC or M to open menu
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_M)) {
        m_sceneManager->changeState(GameState::MENU);
    }
}

void ExplorationScene::draw() {
    // Draw game elements with camera offset
    int camX = m_camera->getOffsetX();
    int camY = m_camera->getOffsetY();

    m_tilemap->render(camX, camY);

    // Draw NPCs
    for (const auto& npc : m_npcs) {
        npc->render(camX, camY);
    }

    m_player->render(camX, camY);

    // Draw exploration UI
    DrawText("Exploration Mode", 10, 10, 20, WHITE);
    DrawText("WASD/Arrows to move", 10, 35, 16, LIGHTGRAY);
    DrawText("Press SPACE near NPCs to talk", 10, 55, 16, LIGHTGRAY);
    DrawText("Press B for battle (test)", 10, 75, 16, LIGHTGRAY);
    DrawText("Press ESC/M for menu", 10, 95, 16, LIGHTGRAY);
}

void ExplorationScene::initializeMap() {
    // Create a simple test map with walls around the border
    for (int y = 0; y < m_mapHeight; y++) {
        for (int x = 0; x < m_mapWidth; x++) {
            // Border walls
            if (x == 0 || x == m_mapWidth - 1 || y == 0 || y == m_mapHeight - 1) {
                m_tilemap->setTile(x, y, 1); // Wall
            }
            // Some interior walls to test collision
            else if ((x == 5 && y >= 3 && y <= 10) ||
                     (x == 15 && y >= 8 && y <= 15) ||
                     (y == 10 && x >= 8 && x <= 12)) {
                m_tilemap->setTile(x, y, 1); // Wall
            }
            // Water patches
            else if ((x >= 20 && x <= 22 && y >= 5 && y <= 8) ||
                     (x >= 10 && x <= 11 && y >= 15 && y <= 16)) {
                m_tilemap->setTile(x, y, 2); // Water
            }
            // Path tiles
            else if (y == m_mapHeight / 2 && x >= 3 && x < m_mapWidth - 3) {
                m_tilemap->setTile(x, y, 3); // Path
            }
            // Default grass
            else {
                m_tilemap->setTile(x, y, 0); // Grass
            }
        }
    }
}

void ExplorationScene::startBattle() {
    // Create a test enemy formation
    auto formation = std::make_unique<EnemyFormation>();
    formation->addEnemy(std::make_unique<Enemy>("Slime", 1, AIBehavior::AGGRESSIVE));
    formation->addEnemy(std::make_unique<Enemy>("Goblin", 2, AIBehavior::BALANCED));

    // Get battle scene (it's already created and alive)
    BattleScene* battleScene = static_cast<BattleScene*>(
        m_sceneManager->getScene(GameState::BATTLE));

    if (battleScene) {
        battleScene->setEnemyFormation(std::move(formation));

        // Set callback - capture scene manager pointer (owned by Game, always valid)
        SceneManager* sceneManager = m_sceneManager;
        battleScene->setOnBattleEndCallback([sceneManager](bool won) {
            sceneManager->changeState(GameState::EXPLORATION);
        });
    }

    // Transition to battle
    m_sceneManager->changeState(GameState::BATTLE);
}

void ExplorationScene::startDialog() {
    // Get dialog scene (it's already created and alive)
    DialogScene* dialogScene = static_cast<DialogScene*>(
        m_sceneManager->getScene(GameState::DIALOG));

    if (dialogScene) {
        // Start with test dialog ID 1
        dialogScene->startDialog(1);
    }

    // Transition to dialog
    m_sceneManager->changeState(GameState::DIALOG);
}

void ExplorationScene::initializeNPCs() {
    // Create test NPCs at various locations on the map
    // NPC 1: Friendly villager near the center
    m_npcs.push_back(std::make_unique<NPC>("Villager", 10, 8, 1, m_tileSize, NPCType::DIALOG));

    // NPC 2: Guard near a wall
    m_npcs.push_back(std::make_unique<NPC>("Guard", 18, 12, 2, m_tileSize, NPCType::DIALOG));

    // NPC 3: Merchant in another area (triggers shop)
    m_npcs.push_back(std::make_unique<NPC>("Merchant", 7, 14, 3, m_tileSize, NPCType::SHOP));
}

void ExplorationScene::checkNPCInteraction() {
    // Check if player presses SPACE or ENTER to interact
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        int playerTileX = m_player->getTileX();
        int playerTileY = m_player->getTileY();

        // Check all NPCs to see if player is adjacent
        for (const auto& npc : m_npcs) {
            if (npc->isPlayerAdjacent(playerTileX, playerTileY)) {
                if (npc->getType() == NPCType::SHOP) {
                    // Transition to shop
                    m_sceneManager->changeState(GameState::SHOP);
                } else {
                    // Get dialog scene
                    DialogScene* dialogScene = static_cast<DialogScene*>(
                        m_sceneManager->getScene(GameState::DIALOG));

                    if (dialogScene) {
                        // Start dialog with this NPC's dialog ID
                        dialogScene->startDialog(npc->getDialogId());

                        // Transition to dialog
                        m_sceneManager->changeState(GameState::DIALOG);
                    }
                }

                // Only interact with one NPC at a time
                return;
            }
        }
    }
}
