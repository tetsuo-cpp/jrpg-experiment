#include "battle_scene.h"
#include <raylib.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>

BattleScene::BattleScene(Party* party, Inventory* inventory)
    : m_name("Battle")
    , m_party(party)
    , m_inventory(inventory)
    , m_enemyFormation(nullptr)
    , m_battleState(BattleState::TURN_START)
    , m_currentActorIndex(0)
    , m_selectedCommand(BattleCommand::ATTACK)
    , m_selectedTarget(0)
    , m_selectedSkillIndex(0)
    , m_selectedItemIndex(0)
    , m_selectedSkill(nullptr)
    , m_selectedItem(nullptr)
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

        case BattleState::SKILL_SELECT:
            handleSkillSelect();
            break;

        case BattleState::ITEM_SELECT:
            handleItemSelect();
            break;

        case BattleState::TARGET_SELECT:
            handleTargetSelect();
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

    // Confirm action
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        switch (m_selectedCommand) {
            case BattleCommand::ATTACK:
                m_selectedTarget = 0;
                m_battleState = BattleState::TARGET_SELECT;
                break;
            case BattleCommand::MAGIC:
                m_selectedSkillIndex = 0;
                m_battleState = BattleState::SKILL_SELECT;
                break;
            case BattleCommand::ITEM:
                m_selectedItemIndex = 0;
                m_battleState = BattleState::ITEM_SELECT;
                break;
            case BattleCommand::DEFEND:
            case BattleCommand::RUN:
                m_battleState = BattleState::EXECUTING_ACTION;
                break;
        }
    }
}

void BattleScene::handleSkillSelect() {
    BattleActor& actor = m_turnOrder[m_currentActorIndex];
    PartyMember* member = m_party->getActiveMember(actor.index);
    if (!member) return;

    const auto& skills = member->getSkills();
    if (skills.empty()) {
        // No skills, go back
        m_battleState = BattleState::PLAYER_SELECT;
        return;
    }

    // Navigate skills
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedSkillIndex = (m_selectedSkillIndex - 1 + skills.size()) % skills.size();
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedSkillIndex = (m_selectedSkillIndex + 1) % skills.size();
    }

    // Back
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_battleState = BattleState::PLAYER_SELECT;
        return;
    }

    // Confirm skill
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        m_selectedSkill = &skills[m_selectedSkillIndex];
        // Check if enough MP
        if (!member->getStats().hasEnoughMP(m_selectedSkill->getMPCost())) {
            m_selectedSkill = nullptr;
            return;  // Not enough MP, stay in skill select
        }
        m_selectedTarget = 0;
        m_battleState = BattleState::TARGET_SELECT;
    }
}

void BattleScene::handleItemSelect() {
    auto items = m_inventory->getAllItems();
    // Filter to only usable battle items
    std::vector<std::pair<Item, int>> usableItems;
    for (const auto& pair : items) {
        if (pair.first.isUsableInBattle()) {
            usableItems.push_back(pair);
        }
    }

    if (usableItems.empty()) {
        // No items, go back
        m_battleState = BattleState::PLAYER_SELECT;
        return;
    }

    // Navigate items
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedItemIndex = (m_selectedItemIndex - 1 + usableItems.size()) % usableItems.size();
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedItemIndex = (m_selectedItemIndex + 1) % usableItems.size();
    }

    // Back
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_battleState = BattleState::PLAYER_SELECT;
        return;
    }

    // Confirm item
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        m_selectedItem = &usableItems[m_selectedItemIndex].first;
        m_selectedTarget = 0;
        m_battleState = BattleState::TARGET_SELECT;
    }
}

void BattleScene::handleTargetSelect() {
    // Determine if targeting enemies or allies
    bool targetingEnemies = true;
    int maxTargets = m_enemyFormation->getEnemies().size();

    if (m_selectedSkill) {
        targetingEnemies = m_selectedSkill->targetsEnemy();
        if (!targetingEnemies) {
            maxTargets = m_party->getActiveCount();
        }
    } else if (m_selectedItem) {
        // Items target allies (healing items)
        targetingEnemies = false;
        maxTargets = m_party->getActiveCount();
    }

    // Navigate targets
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        m_selectedTarget = (m_selectedTarget - 1 + maxTargets) % maxTargets;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        m_selectedTarget = (m_selectedTarget + 1) % maxTargets;
    }

    // Back
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (m_selectedSkill) {
            m_selectedSkill = nullptr;
            m_battleState = BattleState::SKILL_SELECT;
        } else if (m_selectedItem) {
            m_selectedItem = nullptr;
            m_battleState = BattleState::ITEM_SELECT;
        } else {
            m_battleState = BattleState::PLAYER_SELECT;
        }
        return;
    }

    // Confirm target
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

            case BattleCommand::MAGIC: {
                if (m_selectedSkill) {
                    // Use MP
                    member->getStats().useMP(m_selectedSkill->getMPCost());

                    if (m_selectedSkill->isOffensive()) {
                        // Offensive magic targets enemies
                        Enemy* target = m_enemyFormation->getEnemy(m_selectedTarget);
                        if (target && target->getStats().isAlive()) {
                            int damage = calculateSkillDamage(*m_selectedSkill, member->getStats());
                            target->getStats().takeDamage(damage);
                        }
                    } else if (m_selectedSkill->isHealing()) {
                        // Healing magic targets allies
                        if (m_selectedSkill->isMultiTarget()) {
                            // Heal all party members
                            for (size_t i = 0; i < m_party->getActiveCount(); ++i) {
                                PartyMember* ally = m_party->getActiveMember(i);
                                if (ally) {
                                    ally->getStats().heal(m_selectedSkill->getPower());
                                }
                            }
                        } else {
                            // Heal single target
                            PartyMember* target = m_party->getActiveMember(m_selectedTarget);
                            if (target) {
                                target->getStats().heal(m_selectedSkill->getPower());
                            }
                        }
                    }
                    m_selectedSkill = nullptr;
                }
                break;
            }

            case BattleCommand::ITEM: {
                if (m_selectedItem) {
                    PartyMember* target = m_party->getActiveMember(m_selectedTarget);
                    if (target) {
                        // Apply item effect
                        switch (m_selectedItem->getEffect()) {
                            case ItemEffect::RESTORE_HP:
                                target->getStats().heal(m_selectedItem->getEffectPower());
                                break;
                            case ItemEffect::RESTORE_MP:
                                target->getStats().restoreMP(m_selectedItem->getEffectPower());
                                break;
                            case ItemEffect::RESTORE_BOTH:
                                target->getStats().heal(m_selectedItem->getEffectPower());
                                target->getStats().restoreMP(m_selectedItem->getEffectPower());
                                break;
                            default:
                                break;
                        }
                        // Remove item from inventory
                        m_inventory->removeItem(m_selectedItem->getName(), 1);
                    }
                    m_selectedItem = nullptr;
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

int BattleScene::calculateSkillDamage(const Skill& skill, const CharacterStats& attacker) {
    // Skill damage uses the skill's power directly
    // Could add attacker's magic stat later for scaling
    return skill.getPower();
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
        case BattleState::SKILL_SELECT:
            stateText = "Select Skill (ESC to cancel)";
            break;
        case BattleState::ITEM_SELECT:
            stateText = "Select Item (ESC to cancel)";
            break;
        case BattleState::TARGET_SELECT:
            stateText = "Select Target (ESC to cancel)";
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
            if (m_battleState == BattleState::TARGET_SELECT && m_selectedTarget == static_cast<int>(i) &&
                (m_selectedSkill && m_selectedSkill->targetsAlly() || m_selectedItem)) {
                color = YELLOW;
            }
            DrawText(TextFormat("%s HP: %d/%d MP: %d/%d",
                member->getName().c_str(),
                member->getStats().getHP(),
                member->getStats().getMaxHP(),
                member->getStats().getMP(),
                member->getStats().getMaxMP()),
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
                if (m_battleState == BattleState::TARGET_SELECT && m_selectedTarget == static_cast<int>(i) &&
                    (!m_selectedSkill || m_selectedSkill->targetsEnemy())) {
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

    // Draw skill selection menu
    if (m_battleState == BattleState::SKILL_SELECT) {
        BattleActor& actor = m_turnOrder[m_currentActorIndex];
        PartyMember* member = m_party->getActiveMember(actor.index);
        if (member) {
            const auto& skills = member->getSkills();
            DrawRectangle(50, 300, 400, 250, Fade(PURPLE, 0.5f));
            DrawText("SKILLS:", 60, 310, 20, WHITE);
            for (size_t i = 0; i < skills.size(); ++i) {
                Color color = (i == m_selectedSkillIndex) ? YELLOW : WHITE;
                bool hasMP = member->getStats().hasEnoughMP(skills[i].getMPCost());
                if (!hasMP) color = GRAY;
                DrawText(TextFormat("%s (MP: %d)", skills[i].getName().c_str(), skills[i].getMPCost()),
                    70, 340 + i * 25, 16, color);
            }
        }
    }

    // Draw item selection menu
    if (m_battleState == BattleState::ITEM_SELECT) {
        auto items = m_inventory->getAllItems();
        std::vector<std::pair<Item, int>> usableItems;
        for (const auto& pair : items) {
            if (pair.first.isUsableInBattle()) {
                usableItems.push_back(pair);
            }
        }

        DrawRectangle(50, 300, 400, 250, Fade(GREEN, 0.5f));
        DrawText("ITEMS:", 60, 310, 20, WHITE);
        for (size_t i = 0; i < usableItems.size(); ++i) {
            Color color = (i == m_selectedItemIndex) ? YELLOW : WHITE;
            DrawText(TextFormat("%s x%d", usableItems[i].first.getName().c_str(), usableItems[i].second),
                70, 340 + i * 25, 16, color);
        }
    }

    // Highlight target in TARGET_SELECT state
    if (m_battleState == BattleState::TARGET_SELECT) {
        DrawText("< Use LEFT/RIGHT to select target >", 250, 560, 16, YELLOW);
    }
}
