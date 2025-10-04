#pragma once

#include <vector>
#include <memory>
#include "enemy.h"

class EnemyFormation {
public:
    EnemyFormation();

    // Formation management
    void addEnemy(std::unique_ptr<Enemy> enemy);
    void removeDeadEnemies();
    bool allEnemiesDead() const;
    int getAliveCount() const;

    // Accessors
    std::vector<std::unique_ptr<Enemy>>& getEnemies() { return m_enemies; }
    const std::vector<std::unique_ptr<Enemy>>& getEnemies() const { return m_enemies; }
    Enemy* getEnemy(size_t index);
    const Enemy* getEnemy(size_t index) const;

    // Rewards
    int getTotalGoldReward() const;
    int getTotalExpReward() const;

private:
    std::vector<std::unique_ptr<Enemy>> m_enemies;
};
