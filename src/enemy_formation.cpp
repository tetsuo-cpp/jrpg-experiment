#include "enemy_formation.h"
#include <algorithm>

EnemyFormation::EnemyFormation() {
}

void EnemyFormation::addEnemy(std::unique_ptr<Enemy> enemy) {
    m_enemies.push_back(std::move(enemy));
}

void EnemyFormation::removeDeadEnemies() {
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const std::unique_ptr<Enemy>& enemy) {
                return enemy->getStats().isDead();
            }),
        m_enemies.end()
    );
}

bool EnemyFormation::allEnemiesDead() const {
    return std::all_of(m_enemies.begin(), m_enemies.end(),
        [](const std::unique_ptr<Enemy>& enemy) {
            return enemy->getStats().isDead();
        });
}

int EnemyFormation::getAliveCount() const {
    return std::count_if(m_enemies.begin(), m_enemies.end(),
        [](const std::unique_ptr<Enemy>& enemy) {
            return enemy->getStats().isAlive();
        });
}

Enemy* EnemyFormation::getEnemy(size_t index) {
    if (index >= m_enemies.size()) {
        return nullptr;
    }
    return m_enemies[index].get();
}

const Enemy* EnemyFormation::getEnemy(size_t index) const {
    if (index >= m_enemies.size()) {
        return nullptr;
    }
    return m_enemies[index].get();
}

int EnemyFormation::getTotalGoldReward() const {
    int total = 0;
    for (const auto& enemy : m_enemies) {
        total += enemy->getGoldReward();
    }
    return total;
}

int EnemyFormation::getTotalExpReward() const {
    int total = 0;
    for (const auto& enemy : m_enemies) {
        total += enemy->getExpReward();
    }
    return total;
}
