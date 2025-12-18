#pragma once
#include <string>

class GameConfig {
public:
    // Grid and rendering settings
    int cellSize = 10;
    int initialSnakeLength = 3;
    
    // Game mechanics settings
    int bulletLifetime = 50;
    int bulletSpeed = 1;
    float baseUpdateInterval = 0.1f;
    
    // Enemy spawn settings
    int initialEnemySpawnInterval = 150;  // 15 seconds at 10 FPS
    int initialMinEnemies = 4;
    int maxSpawnAttempts = 50;
    int minEnemyDistance = 5;  // Minimum distance from snake head
    
    // Difficulty scaling settings
    int traitIncreaseInterval = 300;  // Every 30 seconds
    int enemyIncreaseInterval = 600;  // Every 60 seconds
    int minSpawnIntervalLimit = 50;   // Minimum spawn interval
    
    // Combat settings
    int baseShootCooldown = 20;
    float attackSpeedBonusPerFood = 0.4f;  // per 5 foods
    int foodsPerAttackBonus = 5;
    
    // Enemy trait settings
    static constexpr int MAX_TRAITS = 5;
    
    // AI settings
    static constexpr int LOG_BUFFER_SIZE = 100;
    std::string aiLogFile = "ai_log.txt";
    
    // Snake life settings
    int invulnerableTime = 10;  // frames
    
    // Visual settings
    int borderWidth = 2;
    
    // Enemy split settings
    static constexpr int SPLIT_OFFSETS_COUNT = 8;
    const int SPLIT_OFFSETS[SPLIT_OFFSETS_COUNT][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    
    // Direction constants
    static const int DIRECTIONS[4][2];
    
    // Constructor with default values
    GameConfig() = default;
    
    // Allow custom configuration
    GameConfig(int cellSize, int initialSnakeLength);
    
    // Validation methods
    bool isValid() const;
    
    // Utility methods
    int getDynamicTraitCount(int gameTimer) const;
    int getDynamicMinEnemies(int gameTimer) const;
    int getDynamicSpawnInterval(int gameTimer, int baseInterval) const;
    float getDynamicAttackSpeed(int foodCount) const;
};