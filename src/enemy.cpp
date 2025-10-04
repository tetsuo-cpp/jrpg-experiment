#include "enemy.h"

Enemy::Enemy(const std::string& name, int level, AIBehavior behavior)
    : m_name(name)
    , m_stats(level)
    , m_behavior(behavior)
    , m_goldReward(0)
    , m_expReward(0)
    , m_spritePath("")
{
    calculateRewards();
}

void Enemy::calculateRewards() {
    // Base rewards scale with level
    int level = m_stats.getLevel();
    m_expReward = 10 * level + (level * level);
    m_goldReward = 5 * level + (level / 2);
}
