#include "battle_scene.h"
#include <raylib.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>

BattleScene::BattleScene(Party* party)
    : m_name("Battle")
    , m_party(party)
    , m_enemyFormation(nullptr)
    , m_battleState(BattleState::TURN_START)
    , m_currentActorIndex(0)
    , m_selectedCommand(BattleCommand::ATTACK)
    , m_selectedTarget(0)
{
    // Seed random for combat calculations
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
}

const std::string& BattleScene::getName() const {
    return m_name;
}

void BattleScene::onEnter() {
    m_battleState = BattleState::TURN_START;
    m_currentActorIndex = 0;
    initializeTurnOrder();
}

void BattleScene::onExit() {
    // Clean up battle state
    m_turnOrder.clear();
}

void BattleScene::setEnemyFormation(std::unique_ptr<EnemyFormation> formation) {
    m_enemyFormation = std::move(formation);
}

void BattleScene::setOnBattleEndCallback(std::function<void(bool)> callback) {
    m_onBattleEnd = callback;
}

void BattleScene::initializeTurnOrder() {
    m_turnOrder.clear();

    // Add all alive party members to turn order
    for (size_t i = 0; i < m_party->getActiveCount(); ++i) {
        PartyMember* member = m_party->getActiveMember(i);
        if (member && member->getStats().isAlive()) {
            // Use attack stat as speed for now (we can add speed stat later)
            int initiative = member->getStats().getAttack() + (std::rand() % 10);
            m_turnOrder.emplace_back(true, static_cast<int>(i), initiative);
        }
    }

    // Add all alive enemies to turn order
    if (m_enemyFormation) {
        for (size_t i = 0; i < m_enemyFormation->getEnemies().size(); ++i) {
            Enemy* enemy = m_enemyFormation->getEnemy(i);
            if (enemy && enemy->getStats().isAlive()) {
                int initiative = enemy->getStats().getAttack() + (std::rand() % 10);
                m_turnOrder.emplace_back(false, static_cast<int>(i), initiative);
            }
        }
    }

    // Sort by initiative (highest first)
    std::sort(m_turnOrder.begin(), m_turnOrder.end(),
        [](const BattleActor& a, const BattleActor& b) {
            return a.initiative > b.initiative;
        });

    m_currentActorIndex = 0;
}

void BattleScene::update(float deltaTime) {
    switch (m_battleState) {
        case BattleState::TURN_START:
            startNextTurn();
            break;

        case BattleState::PLAYER_SELECT:
            handlePlayerInput();
            break;

        case BattleState::ENEMY_SELECT:
            handleEnemyAI();
            break;

        case BattleState::EXECUTING_ACTION:
            executeAction();
            break;

        case BattleState::TURN_END:
            checkBattleEnd();
            break;

        case BattleState::VICTORY:
        case BattleState::DEFEAT:
        case BattleState::FLED:
            // Wait for player to exit battle
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                endBattle(m_battleState == BattleState::VICTORY || m_battleState == BattleState::FLED);
            }
            break;
    }
}

void BattleScene::startNextTurn() {
    if (m_currentActorIndex >= m_turnOrder.size()) {
        // Round complete, restart turn order
        m_currentActorIndex = 0;
        initializeTurnOrder();
    }

    BattleActor& actor = m_turnOrder[m_currentActorIndex];

    if (actor.isPartyMember) {
        m_battleState = BattleState::PLAYER_SELECT;
        m_selectedCommand = BattleCommand::ATTACK;
        m_selectedTarget = 0;
    } else {
        m_battleState = BattleState::ENEMY_SELECT;
    }
}

void BattleScene::handlePlayerInput() {
    // Command selection
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedCommand = static_cast<BattleCommand>(
            (static_cast<int>(m_selectedCommand) - 1 + 5) % 5);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedCommand = static_cast<BattleCommand>(
            (static_cast<int>(m_selectedCommand) + 1) % 5);
    }

    // Target selection (for now, just cycle through enemies)
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        m_selectedTarget = (m_selectedTarget - 1 + m_enemyFormation->getEnemies().size())
            % m_enemyFormation->getEnemies().size();
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        m_selectedTarget = (m_selectedTarget + 1) % m_enemyFormation->getEnemies().size();
    }

    // Confirm action
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        m_battleState = BattleState::EXECUTING_ACTION;
    }
}

void BattleScene::handleEnemyAI() {
    // Simple AI: just attack a random alive party member
    BattleActor& actor = m_turnOrder[m_currentActorIndex];
    Enemy* enemy = m_enemyFormation->getEnemy(actor.index);

    if (!enemy || enemy->getStats().isDead()) {
        m_currentActorIndex++;
        m_battleState = BattleState::TURN_END;
        return;
    }

    // Find random alive party member
    std::vector<int> aliveIndices;
    for (size_t i = 0; i < m_party->getActiveCount(); ++i) {
        PartyMember* member = m_party->getActiveMember(i);
        if (member && member->getStats().isAlive()) {
            aliveIndices.push_back(static_cast<int>(i));
        }
    }

    if (!aliveIndices.empty()) {
        m_selectedTarget = aliveIndices[std::rand() % aliveIndices.size()];
        m_selectedCommand = BattleCommand::ATTACK;
        m_battleState = BattleState::EXECUTING_ACTION;
    } else {
        m_currentActorIndex++;
        m_battleState = BattleState::TURN_END;
    }
}

void BattleScene::executeAction() {
    BattleActor& actor = m_turnOrder[m_currentActorIndex];

    if (actor.isPartyMember) {
        PartyMember* member = m_party->getActiveMember(actor.index);
        if (!member || member->getStats().isDead()) {
            m_currentActorIndex++;
            m_battleState = BattleState::TURN_END;
            return;
        }

        switch (m_selectedCommand) {
            case BattleCommand::ATTACK: {
                Enemy* target = m_enemyFormation->getEnemy(m_selectedTarget);
                if (target && target->getStats().isAlive()) {
                    if (checkHit()) {
                        int damage = calculateDamage(member->getStats(), target->getStats());
                        if (checkCritical()) {
                            damage *= 2;
                        }
                        target->getStats().takeDamage(damage);
                    }
                }
                break;
            }

            case BattleCommand::DEFEND:
                // TODO: Implement defense boost for next turn
                break;

            case BattleCommand::RUN: {
                // 50% chance to flee
                if (std::rand() % 100 < 50) {
                    m_battleState = BattleState::FLED;
                    return;
                }
                break;
            }

            case BattleCommand::MAGIC:
            case BattleCommand::ITEM:
                // TODO: Implement magic and item usage
                break;
        }
    } else {
        // Enemy turn
        Enemy* enemy = m_enemyFormation->getEnemy(actor.index);
        if (!enemy || enemy->getStats().isDead()) {
            m_currentActorIndex++;
            m_battleState = BattleState::TURN_END;
            return;
        }

        PartyMember* target = m_party->getActiveMember(m_selectedTarget);
        if (target && target->getStats().isAlive()) {
            if (checkHit()) {
                int damage = calculateDamage(enemy->getStats(), target->getStats());
                if (checkCritical()) {
                    damage *= 2;
                }
                target->getStats().takeDamage(damage);
            }
        }
    }

    m_currentActorIndex++;
    m_battleState = BattleState::TURN_END;
}

void BattleScene::checkBattleEnd() {
    // Check if all enemies are dead
    if (m_enemyFormation && m_enemyFormation->allEnemiesDead()) {
        m_battleState = BattleState::VICTORY;
        return;
    }

    // Check if all party members are dead
    bool anyAlive = false;
    for (size_t i = 0; i < m_party->getActiveCount(); ++i) {
        PartyMember* member = m_party->getActiveMember(i);
        if (member && member->getStats().isAlive()) {
            anyAlive = true;
            break;
        }
    }

    if (!anyAlive) {
        m_battleState = BattleState::DEFEAT;
        return;
    }

    // Continue battle
    m_battleState = BattleState::TURN_START;
}

void BattleScene::endBattle(bool victory) {
    if (victory && m_enemyFormation) {
        // Award EXP and gold
        int exp = m_enemyFormation->getTotalExpReward();
        int gold = m_enemyFormation->getTotalGoldReward();
        m_party->gainExperienceAll(exp);
        // TODO: Add gold to party inventory when implemented
    }

    if (m_onBattleEnd) {
        m_onBattleEnd(victory);
    }
}

int BattleScene::calculateDamage(const CharacterStats& attacker, const CharacterStats& defender) {
    // Simple damage formula: Attack - Defense/2, minimum 1
    int damage = attacker.getAttack() - (defender.getDefense() / 2);
    return std::max(1, damage);
}

bool BattleScene::checkHit() {
    // 90% base hit chance
    return (std::rand() % 100) < 90;
}

bool BattleScene::checkCritical() {
    // 10% critical hit chance
    return (std::rand() % 100) < 10;
}

void BattleScene::draw() {
    ClearBackground(BLACK);

    // Draw battle state text
    const char* stateText = "";
    switch (m_battleState) {
        case BattleState::TURN_START:
            stateText = "Turn Starting...";
            break;
        case BattleState::PLAYER_SELECT:
            stateText = "Select Action";
            break;
        case BattleState::ENEMY_SELECT:
            stateText = "Enemy Thinking...";
            break;
        case BattleState::EXECUTING_ACTION:
            stateText = "Executing...";
            break;
        case BattleState::TURN_END:
            stateText = "Turn Ending...";
            break;
        case BattleState::VICTORY:
            stateText = "VICTORY! (Press SPACE)";
            break;
        case BattleState::DEFEAT:
            stateText = "DEFEAT... (Press SPACE)";
            break;
        case BattleState::FLED:
            stateText = "Escaped! (Press SPACE)";
            break;
    }
    DrawText(stateText, 300, 50, 20, WHITE);

    // Draw party status
    int yPos = 100;
    DrawText("PARTY:", 50, yPos, 20, GREEN);
    yPos += 30;
    for (size_t i = 0; i < m_party->getActiveCount(); ++i) {
        PartyMember* member = m_party->getActiveMember(i);
        if (member) {
            Color color = member->getStats().isAlive() ? WHITE : RED;
            DrawText(TextFormat("%s HP: %d/%d",
                member->getName().c_str(),
                member->getStats().getHP(),
                member->getStats().getMaxHP()),
                50, yPos, 16, color);
            yPos += 25;
        }
    }

    // Draw enemies
    if (m_enemyFormation) {
        yPos = 100;
        DrawText("ENEMIES:", 500, yPos, 20, RED);
        yPos += 30;
        for (size_t i = 0; i < m_enemyFormation->getEnemies().size(); ++i) {
            Enemy* enemy = m_enemyFormation->getEnemy(i);
            if (enemy) {
                Color color = enemy->getStats().isAlive() ? WHITE : GRAY;
                if (m_battleState == BattleState::PLAYER_SELECT && i == m_selectedTarget) {
                    color = YELLOW;
                }
                DrawText(TextFormat("%s HP: %d/%d",
                    enemy->getName().c_str(),
                    enemy->getStats().getHP(),
                    enemy->getStats().getMaxHP()),
                    500, yPos, 16, color);
                yPos += 25;
            }
        }
    }

    // Draw command menu if player is selecting
    if (m_battleState == BattleState::PLAYER_SELECT) {
        DrawRectangle(50, 400, 300, 150, Fade(BLUE, 0.5f));
        const char* commands[] = {"ATTACK", "MAGIC", "ITEM", "DEFEND", "RUN"};
        for (int i = 0; i < 5; ++i) {
            Color color = (i == static_cast<int>(m_selectedCommand)) ? YELLOW : WHITE;
            DrawText(commands[i], 70, 420 + i * 25, 20, color);
        }
    }
}
