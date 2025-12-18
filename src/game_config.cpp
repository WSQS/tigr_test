#include "game_config.hpp"

// Static member definition
const int GameConfig::DIRECTIONS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

GameConfig::GameConfig(int cellSize, int initialSnakeLength) 
    : cellSize(cellSize), initialSnakeLength(initialSnakeLength) {
}

bool GameConfig::isValid() const {
    return cellSize > 0 && 
           initialSnakeLength > 0 && 
           bulletLifetime > 0 && 
           bulletSpeed > 0 &&
           baseUpdateInterval > 0.0f &&
           initialEnemySpawnInterval > 0 &&
           initialMinEnemies >= 0 &&
           maxSpawnAttempts > 0 &&
           minEnemyDistance >= 0 &&
           traitIncreaseInterval > 0 &&
           enemyIncreaseInterval > 0 &&
           minSpawnIntervalLimit > 0 &&
           baseShootCooldown >= 0 &&
           attackSpeedBonusPerFood >= 0.0f &&
           foodsPerAttackBonus > 0 &&
           invulnerableTime >= 0 &&
           borderWidth >= 0;
}

int GameConfig::getDynamicTraitCount(int gameTimer) const {
    return gameTimer / traitIncreaseInterval;
}

int GameConfig::getDynamicMinEnemies(int gameTimer) const {
    return initialMinEnemies + (gameTimer / enemyIncreaseInterval);
}

int GameConfig::getDynamicSpawnInterval(int gameTimer, int baseInterval) const {
    int currentInterval = baseInterval - (gameTimer / traitIncreaseInterval);
    return currentInterval < minSpawnIntervalLimit ? minSpawnIntervalLimit : currentInterval;
}

float GameConfig::getDynamicAttackSpeed(int foodCount) const {
    float attackSpeedBonus = (foodCount / foodsPerAttackBonus) * attackSpeedBonusPerFood;
    return baseShootCooldown - attackSpeedBonus;
}