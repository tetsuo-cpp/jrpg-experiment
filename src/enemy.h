#pragma once

#include <string>
#include "character_stats.h"

enum class AIBehavior {
    AGGRESSIVE,  // Always attacks
    BALANCED,    // Mix of attacks and skills
    DEFENSIVE,   // Prefers defending, attacks when safe
    SUPPORT      // Heals/buffs allies if implemented
};

class Enemy {
public:
    Enemy(const std::string& name, int level, AIBehavior behavior);

    // Accessors
    const std::string& getName() const { return m_name; }
    CharacterStats& getStats() { return m_stats; }
    const CharacterStats& getStats() const { return m_stats; }
    AIBehavior getBehavior() const { return m_behavior; }
    int getGoldReward() const { return m_goldReward; }
    int getExpReward() const { return m_expReward; }

    // Visual data (for future sprite implementation)
    const std::string& getSpritePath() const { return m_spritePath; }
    void setSpritePath(const std::string& path) { m_spritePath = path; }

private:
    void calculateRewards();

    std::string m_name;
    CharacterStats m_stats;
    AIBehavior m_behavior;
    int m_goldReward;
    int m_expReward;
    std::string m_spritePath;
};
