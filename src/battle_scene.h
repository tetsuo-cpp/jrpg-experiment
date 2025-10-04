#pragma once

#include "scene.h"
#include "party.h"
#include "enemy_formation.h"
#include "inventory.h"
#include "skill.h"
#include <memory>
#include <vector>
#include <functional>

enum class BattleState {
    TURN_START,       // Beginning of a turn, determine next actor
    PLAYER_SELECT,    // Player selecting action
    SKILL_SELECT,     // Player selecting which skill to use
    ITEM_SELECT,      // Player selecting which item to use
    TARGET_SELECT,    // Player selecting target for action
    ENEMY_SELECT,     // Enemy AI choosing action
    EXECUTING_ACTION, // Action animation/execution
    TURN_END,         // End of turn, check victory/defeat
    VICTORY,          // Battle won
    DEFEAT,           // Party wiped
    FLED              // Successfully ran away
};

enum class BattleCommand {
    ATTACK,
    MAGIC,
    ITEM,
    DEFEND,
    RUN
};

// Represents a combatant in battle (can be party member or enemy)
struct BattleActor {
    bool isPartyMember;
    int index;  // Index in party or enemy formation
    int initiative;  // Speed-based turn order value

    BattleActor(bool isPM, int idx, int init)
        : isPartyMember(isPM), index(idx), initiative(init) {}
};

class BattleScene : public Scene {
public:
    BattleScene(Party* party, Inventory* inventory);

    // Scene lifecycle
    void onEnter() override;
    void onExit() override;

    // Main loop
    void update(float deltaTime) override;
    void draw() override;

    // Scene identification
    const std::string& getName() const override;

    // Battle setup
    void setEnemyFormation(std::unique_ptr<EnemyFormation> formation);

    // Battle exit callback
    void setOnBattleEndCallback(std::function<void(bool won)> callback);

private:
    void initializeTurnOrder();
    void startNextTurn();
    void handlePlayerInput();
    void handleSkillSelect();
    void handleItemSelect();
    void handleTargetSelect();
    void handleEnemyAI();
    void executeAction();
    void checkBattleEnd();
    void endBattle(bool victory);

    // Combat helpers
    int calculateDamage(const CharacterStats& attacker, const CharacterStats& defender);
    int calculateSkillDamage(const Skill& skill, const CharacterStats& attacker);
    bool checkHit();
    bool checkCritical();

    std::string m_name;
    Party* m_party;  // Non-owning pointer to game's party
    Inventory* m_inventory;  // Non-owning pointer to game's inventory
    std::unique_ptr<EnemyFormation> m_enemyFormation;

    BattleState m_battleState;
    std::vector<BattleActor> m_turnOrder;
    size_t m_currentActorIndex;

    // Player action state
    BattleCommand m_selectedCommand;
    int m_selectedTarget;
    int m_selectedSkillIndex;
    int m_selectedItemIndex;
    const Skill* m_selectedSkill;
    Item* m_selectedItem;

    std::function<void(bool)> m_onBattleEnd;
};
