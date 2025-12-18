#include "tigr.h"
#include "game_types.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cfloat>
#include <queue>
#include <algorithm>

class SnakeGame
{
private:
    std::vector<Point> snake;
    std::vector<Point> foods;
    Direction direction;
    int cellSize;
    int gridWidth, gridHeight;
    bool gameOver;
    int score;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    int shootCooldown;
    
    int enemySpawnTimer;
    int enemySpawnInterval;
    int minEnemies;
    int gameTimer;
    
    int invulnerableTimer;
    bool isHit;
    
    bool aiMode;
    
    int foodCount;
    
    std::vector<std::string> logBuffer;
    static const int LOG_BUFFER_SIZE = 100;
    int logCounter;
    
    static const int DIRECTIONS[4][2];
    
    Enemy* findNearestEnemy()
    {
        if (enemies.empty()) return nullptr;
        
        Enemy* nearest = nullptr;
        float minDistance = FLT_MAX;
        
        for (auto& enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            float distance = GameUtils::euclideanDistance(
                static_cast<float>(snake[0].x), 
                static_cast<float>(snake[0].y),
                static_cast<float>(enemy.position.x), 
                static_cast<float>(enemy.position.y)
            );
            
            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = &enemy;
            }
        }
        
        return nearest;
    }
    
    void shoot()
    {
        if (shootCooldown > 0) return;
        
        Enemy* target = findNearestEnemy();
        if (target)
        {
            bullets.push_back(Bullet(snake[0], target->position, 0.8f));
            
            float attackSpeedBonus = (foodCount / 5) * 2.0f;
            int dynamicCooldown = 20 - static_cast<int>(attackSpeedBonus);
            shootCooldown = dynamicCooldown;
        }
    }
    
    bool isPositionOccupied(int x, int y)
    {
        for (const auto& segment : snake)
        {
            if (segment.x == x && segment.y == y) return true;
        }
        
        for (const auto& enemy : enemies)
        {
            if (enemy.isAlive() && enemy.position.x == x && enemy.position.y == y) return true;
        }
        
        for (const auto &food : foods)
        {
            if (food.x == x && food.y == y) return true;
        }
        
        return false;
    }
    
    void spawnEnemy()
    {
        const int maxAttempts = 50;
        bool spawned = false;
        
        for (int attempt = 0; attempt < maxAttempts && !spawned; attempt++)
        {
            int x, y;
            int side = std::rand() % 4;
            
            switch (side)
            {
            case 0:  // Top
                x = std::rand() % gridWidth;
                y = 0;
                break;
            case 1:  // Right
                x = gridWidth - 1;
                y = std::rand() % gridHeight;
                break;
            case 2:  // Bottom
                x = std::rand() % gridWidth;
                y = gridHeight - 1;
                break;
            case 3:  // Left
                x = 0;
                y = std::rand() % gridHeight;
                break;
            default:
                continue;
            }
            
            if (!isPositionOccupied(x, y))
            {
                float dx = x - snake[0].x;
                float dy = y - snake[0].y;
                float distance = sqrt(dx * dx + dy * dy);
                
                if (distance > 5.0f)
                {
                    int maxTraitCount = gameTimer / 300;
                    std::vector<EnemyTrait> selectedTraits;
                    
                    int traitCount = std::rand() % (maxTraitCount + 1);
                    for (int i = 0; i < traitCount; i++)
                    {
                        EnemyTrait trait = static_cast<EnemyTrait>(std::rand() % TRAIT_COUNT);
                        selectedTraits.push_back(trait);
                    }
                    
                    enemies.push_back(Enemy(x, y, selectedTraits));
                    spawned = true;
                }
            }
        }
    }
    
    void updateEnemySpawn()
    {
        gameTimer++;
        enemySpawnTimer++;
        
        int aliveEnemies = 0;
        for (const auto& enemy : enemies)
        {
            if (enemy.isAlive()) aliveEnemies++;
        }
        
        int dynamicMinEnemies = minEnemies + (gameTimer / 600);
        
        if (aliveEnemies < dynamicMinEnemies)
        {
            spawnEnemy();
            enemySpawnTimer = 0;
            return;
        }
        
        int currentInterval = enemySpawnInterval - (gameTimer / 300);
        if (currentInterval < 50) currentInterval = 50;
        
        if (enemySpawnTimer >= currentInterval)
        {
            spawnEnemy();
            enemySpawnTimer = 0;
        }
    }
    
    void takeDamage()
    {
        if (invulnerableTimer > 0) return;
        
        if (snake.size() > 1)
        {
            snake.pop_back();
            invulnerableTimer = 10;
            isHit = true;
        }
        else
        {
            gameOver = true;
        }
    }
    
    void updateInvulnerability()
    {
        if (invulnerableTimer > 0)
        {
            invulnerableTimer--;
            if (invulnerableTimer == 0)
            {
                isHit = false;
            }
        }
    }

public:
    SnakeGame(int width, int height)
    {
        cellSize = 10;
        gridWidth = width / cellSize;
        gridHeight = height / cellSize;

        snake.push_back({gridWidth / 2, gridHeight / 2});
        snake.push_back({gridWidth / 2 - 1, gridHeight / 2});
        snake.push_back({gridWidth / 2 - 2, gridHeight / 2});

        direction = RIGHT;
        gameOver = false;
        score = 0;

        std::srand(std::time(0));
        generateFood();
        
        enemies.clear();
        enemies.push_back(Enemy(2, 2, {}));
        enemies.push_back(Enemy(gridWidth - 3, 2, {}));
        enemies.push_back(Enemy(2, gridHeight - 3, {}));
        enemies.push_back(Enemy(gridWidth - 3, gridHeight - 3, {}));
        
        bullets.clear();
        shootCooldown = 0;
        
        enemySpawnTimer = 0;
        enemySpawnInterval = 150;
        minEnemies = 4;
        gameTimer = 0;
        
        invulnerableTimer = 0;
        isHit = false;
        
        aiMode = true;
        foodCount = 0;
        
        logCounter = 0;
        logBuffer.reserve(LOG_BUFFER_SIZE);
    }
    
    void toggleAIMode() { aiMode = !aiMode; }
    bool isAIMode() const { return aiMode; }
    
    int manhattanDistance(const Point& a, const Point& b) const
    {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }
    
    bool isSafePosition(const Point& pos, bool allowFood = true) const
    {
        if (pos.x < 0 || pos.x >= gridWidth || pos.y < 0 || pos.y >= gridHeight)
            return false;
        
        for (const auto& segment : snake)
        {
            if (pos.x == segment.x && pos.y == segment.y)
                return false;
        }
        
        for (const auto& enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            if (pos.x == enemy.position.x && pos.y == enemy.position.y)
                return false;
            
            int enemySpeed = static_cast<int>(enemy.speed);
            if (enemy.moveCounter >= enemySpeed - 1)
            {
                Point predictedEnemyPos = enemy.position;
                
                Point nearestSegment = snake[0];
                float minDist = FLT_MAX;
                for (const auto& segment : snake)
                {
                    float dist = manhattanDistance(enemy.position, segment);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        nearestSegment = segment;
                    }
                }
                
                if (nearestSegment.x < predictedEnemyPos.x) predictedEnemyPos.x--;
                else if (nearestSegment.x > predictedEnemyPos.x) predictedEnemyPos.x++;
                
                if (nearestSegment.y < predictedEnemyPos.y) predictedEnemyPos.y--;
                else if (nearestSegment.y > predictedEnemyPos.y) predictedEnemyPos.y++;
                
                if (pos.x == predictedEnemyPos.x && pos.y == predictedEnemyPos.y)
                    return false;
            }
        }
        
        return true;
    }
    
    std::vector<Point> findPath(const Point& start, const Point& target) const
    {
        if (start.x == target.x && start.y == target.y) {
            return {};
        }
        
        std::vector<std::vector<bool>> visited(gridWidth, std::vector<bool>(gridHeight, false));
        std::vector<std::vector<Point>> parent(gridWidth, std::vector<Point>(gridHeight, {-1, -1}));
        std::queue<Point> queue;
        
        queue.push(start);
        visited[start.x][start.y] = true;
        
        const int (*directions)[2] = DIRECTIONS;
        
        while (!queue.empty())
        {
            Point current = queue.front();
            queue.pop();
            
            if (current.x == target.x && current.y == target.y)
            {
                std::vector<Point> path;
                Point node = target;
                while (!(node.x == start.x && node.y == start.y))
                {
                    path.insert(path.begin(), node);
                    node = parent[node.x][node.y];
                }
                return path;
            }
            
            for (int i = 0; i < 4; i++)
            {
                Point next = {current.x + directions[i][0], current.y + directions[i][1]};
                
                if (next.x < 0 || next.x >= gridWidth || next.y < 0 || next.y >= gridHeight)
                    continue;
                
                if (visited[next.x][next.y])
                    continue;
                
                bool isTarget = (next.x == target.x && next.y == target.y);
                bool safe = false;
                
                if (isTarget) {
                    safe = true;
                    for (const auto& segment : snake) {
                        if (next.x == segment.x && next.y == segment.y) {
                            safe = false;
                            break;
                        }
                    }
                } else {
                    safe = isSafePosition(next, false);
                }
                
                if (safe)
                {
                    visited[next.x][next.y] = true;
                    parent[next.x][next.y] = current;
                    queue.push(next);
                }
            }
        }
        
        return {};
    }
    
    int evaluateAccessibleArea(const Point& start) const
    {
        std::vector<std::vector<bool>> visited(gridWidth, std::vector<bool>(gridHeight, false));
        std::queue<Point> queue;
        int areaSize = 0;
        
        queue.push(start);
        visited[start.x][start.y] = true;
        
        const int (*directions)[2] = DIRECTIONS;
        
        while (!queue.empty())
        {
            Point current = queue.front();
            queue.pop();
            areaSize++;
            
            for (int i = 0; i < 4; i++)
            {
                Point next = {current.x + directions[i][0], current.y + directions[i][1]};
                
                if (next.x >= 0 && next.x < gridWidth && next.y >= 0 && next.y < gridHeight &&
                    !visited[next.x][next.y] && isSafePosition(next))
                {
                    visited[next.x][next.y] = true;
                    queue.push(next);
                }
            }
        }
        
        return areaSize;
    }
    
    Point predictEnemyPosition(const Enemy& enemy, int steps = 2) const
    {
        Point predictedPos = enemy.position;
        
        for (int step = 0; step < steps; step++)
        {
            Point nearestSegment = snake[0];
            float minDist = FLT_MAX;
            
            for (const auto& segment : snake)
            {
                float dist = manhattanDistance(predictedPos, segment);
                if (dist < minDist)
                {
                    minDist = dist;
                    nearestSegment = segment;
                }
            }
            
            if (nearestSegment.x < predictedPos.x) predictedPos.x--;
            else if (nearestSegment.x > predictedPos.x) predictedPos.x++;
            else if (nearestSegment.y < predictedPos.y) predictedPos.y--;
            else if (nearestSegment.y > predictedPos.y) predictedPos.y++;
        }
        
        return predictedPos;
    }
    
    void logAIDecision(const std::string& message)
    {
        logBuffer.push_back(message);
        logCounter++;
        
        if (logBuffer.size() >= LOG_BUFFER_SIZE || logCounter % 10 == 0)
        {
            flushLogBuffer();
        }
    }
    
    void flushLogBuffer()
    {
        if (logBuffer.empty()) return;
        
        FILE* logFile = fopen("ai_log.txt", "a");
        if (logFile)
        {
            for (const auto& msg : logBuffer)
            {
                fprintf(logFile, "%s\n", msg.c_str());
            }
            fclose(logFile);
            logBuffer.clear();
        }
    }
    
    static bool firstRun;
    
    Direction makeAIDecision()
    {
        Point head = snake[0];
        
        if (firstRun)
        {
            FILE* logFile = fopen("ai_log.txt", "w");
            if (logFile)
            {
                fprintf(logFile, "=== Snake Game AI Log Start ===\n");
                fprintf(logFile, "Game Time: %I64d\n", (long long)time(nullptr));
                fclose(logFile);
            }
            firstRun = false;
        }
        
        char statusMsg[256];
        sprintf(statusMsg, "\n=== AI Decision Start [%I64d] === Head:(%d,%d) Foods:%zu Length:%zu Dir:%s", 
                (long long)time(nullptr), head.x, head.y, foods.size(), snake.size(),
                direction == UP ? "UP" : direction == DOWN ? "DOWN" : direction == LEFT ? "LEFT" : "RIGHT");
        logAIDecision(statusMsg);
        
        Point targetFood = {-1, -1};
        if (!foods.empty())
        {
            targetFood = foods[0];
            float minDistance = manhattanDistance(head, targetFood);
            for (const auto& food : foods)
            {
                float dist = manhattanDistance(head, food);
                if (dist < minDistance)
                {
                    minDistance = dist;
                    targetFood = food;
                }
            }
        }
        
        for (size_t i = 0; i < enemies.size(); i++)
        {
            if (enemies[i].isAlive())
            {
                char enemyMsg[128];
                sprintf(enemyMsg, "Enemy%zu: Pos(%d,%d) HP:%d Speed:%.1f", 
                        i, enemies[i].position.x, enemies[i].position.y, 
                        enemies[i].currentHealth, enemies[i].speed);
                logAIDecision(enemyMsg);
            }
        }
        
        std::vector<std::pair<Direction, Point>> possibleMoves;
        
        if (direction != DOWN) possibleMoves.push_back({UP, {head.x, head.y - 1}});
        if (direction != UP) possibleMoves.push_back({DOWN, {head.x, head.y + 1}});
        if (direction != RIGHT) possibleMoves.push_back({LEFT, {head.x - 1, head.y}});
        if (direction != LEFT) possibleMoves.push_back({RIGHT, {head.x + 1, head.y}});
        
        Direction bestMove = direction;
        float bestScore = -FLT_MAX;
        
        for (const auto& move : possibleMoves)
        {
            float score = 0.0f;
            Point nextPos = move.second;
            
            const char* dirName = move.first == UP ? "UP" : move.first == DOWN ? "DOWN" : 
                                 move.first == LEFT ? "LEFT" : "RIGHT";
            
            char moveMsg[128];
            sprintf(moveMsg, "--- Evaluating Direction: %s to Pos(%d,%d) ---", dirName, nextPos.x, nextPos.y);
            logAIDecision(moveMsg);
            
            if (!isSafePosition(nextPos))
            {
                score -= 1000.0f;
                logAIDecision("  X Unsafe position! -1000 score");
            }
            else
            {
                logAIDecision("  + Position is safe");
                
                bool canEatFood = false;
                int pathLength = 0;
                
                bool directFood = false;
                for (const auto& food : foods) {
                    if (nextPos.x == food.x && nextPos.y == food.y) {
                        canEatFood = true;
                        pathLength = 0;
                        char directMsg[128];
                        sprintf(directMsg, "  >> Can eat food directly! Pos(%d,%d)", nextPos.x, nextPos.y);
                        logAIDecision(directMsg);
                        directFood = true;
                        break;
                    }
                }
                
                if (!directFood) {
                    Point targetFood = {-1, -1};
                    if (!foods.empty()) {
                        targetFood = foods[0];
                        float minDist = FLT_MAX;
                        for (const auto& food : foods) {
                            float dist = manhattanDistance(nextPos, food);
                            if (dist < minDist) {
                                minDist = dist;
                                targetFood = food;
                            }
                        }
                    }
                    
                    if (targetFood.x >= 0) {
                        std::vector<Point> pathToFood = findPath(nextPos, targetFood);
                        if (!pathToFood.empty()) {
                            canEatFood = true;
                            pathLength = pathToFood.size();
                            char pathMsg[128];
                            sprintf(pathMsg, "  * Found path to food! Length:%d", pathLength);
                            logAIDecision(pathMsg);
                        }
                    }
                }
                
                if (canEatFood)
                {
                    score += 1000.0f;
                    
                    if (pathLength == 0) {
                        score += 1000.0f;
                        char bonusMsg[128];
                        sprintf(bonusMsg, "  *** Direct food eat! Super bonus:1000.0");
                        logAIDecision(bonusMsg);
                    } else {
                        score += (200.0f / pathLength);
                        char pathScoreMsg[128];
                        sprintf(pathScoreMsg, "  * Path length bonus:%.1f", 200.0f / pathLength);
                        logAIDecision(pathScoreMsg);
                        
                        if (pathLength == 1) {
                            score += 500.0f;
                            char bonusMsg[128];
                            sprintf(bonusMsg, "  >> Next step eats food! Extra bonus:500.0");
                            logAIDecision(bonusMsg);
                        }
                    }
                }
                else
                {
                    bool directToFood = isSafePosition(targetFood, true);
                    char debugMsg[256];
                    sprintf(debugMsg, "  ! Cannot reach food - Check food pos(%d,%d) safety:%s", 
                            targetFood.x, targetFood.y, directToFood ? "safe" : "unsafe");
                    logAIDecision(debugMsg);
                    
                    bool foodOnSnake = false;
                    for (const auto& segment : snake) {
                        if (segment.x == targetFood.x && segment.y == targetFood.y) {
                            foodOnSnake = true;
                            break;
                        }
                    }
                    if (foodOnSnake) {
                        logAIDecision("    Reason: Food overlaps with snake body");
                    }
                }
                
                int accessibleArea = evaluateAccessibleArea(nextPos);
                score += accessibleArea * 0.5f;
                
                char areaMsg[128];
                sprintf(areaMsg, "  # Accessible area:%d score:%.1f", accessibleArea, accessibleArea * 0.5f);
                logAIDecision(areaMsg);
                
                float totalThreat = 0.0f;
                for (const auto& enemy : enemies)
                {
                    if (!enemy.isAlive()) continue;
                    
                    float currentDist = manhattanDistance(nextPos, enemy.position);
                    Point predictedEnemy = predictEnemyPosition(enemy, 2);
                    float futureDist = manhattanDistance(nextPos, predictedEnemy);
                    
                    float enemyThreat = 0.0f;
                    if (currentDist < 5.0f)
                    {
                        enemyThreat += (5.0f - currentDist) * 100.0f;
                    }
                    if (futureDist < 5.0f)
                    {
                        enemyThreat += (5.0f - futureDist) * 150.0f;
                    }
                    
                    int dx = snake[0].x - enemy.position.x;
                    int dy = snake[0].y - enemy.position.y;
                    if (abs(dx) <= 2 && abs(dy) <= 2)
                    {
                        enemyThreat += 50.0f;
                    }
                    
                    totalThreat += enemyThreat;
                    
                    char threatMsg[256];
                    sprintf(threatMsg, "    @ Enemy(%d,%d) CurrDist:%.1f PredDist:%.1f Threat:%.1f", 
                            enemy.position.x, enemy.position.y, currentDist, futureDist, enemyThreat);
                    logAIDecision(threatMsg);
                }
                
                char totalThreatMsg[128];
                sprintf(totalThreatMsg, "  ! Total threat:%.1f penalty:%.1f", totalThreat, -totalThreat);
                logAIDecision(totalThreatMsg);
                score -= totalThreat;
                
                int foodDist = manhattanDistance(nextPos, targetFood);
                
                if (snake.size() <= 5)
                {
                    score += (500.0f - foodDist * 15.0f);
                }
                else if (snake.size() <= 10)
                {
                    score += (400.0f - foodDist * 12.0f);
                }
                else
                {
                    score += (300.0f - foodDist * 10.0f);
                }
                
                int openSpaces = 0;
                for (int dx = -2; dx <= 2; dx++)
                {
                    for (int dy = -2; dy <= 2; dy++)
                    {
                        Point checkPos = {nextPos.x + dx, nextPos.y + dy};
                        if (isSafePosition(checkPos))
                            openSpaces++;
                    }
                }
                score += openSpaces * 5.0f;
                
                float wallDist = std::min({nextPos.x, nextPos.y, 
                                         gridWidth - 1 - nextPos.x, 
                                         gridHeight - 1 - nextPos.y});
                score += wallDist * 15.0f;
                
                std::vector<Point> futureSnake = snake;
                futureSnake.insert(futureSnake.begin(), nextPos);
                if (!futureSnake.empty())
                {
                    futureSnake.pop_back();
                }
                
                int futureSafeMoves = 0;
                for (int dx = -1; dx <= 1; dx++)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        if (dx == 0 && dy == 0) continue;
                        Point futurePos = {nextPos.x + dx, nextPos.y + dy};
                        if (isSafePosition(futurePos))
                            futureSafeMoves++;
                    }
                }
                score += futureSafeMoves * 20.0f;
            }
            
            char finalScoreMsg[128];
            sprintf(finalScoreMsg, "  = Final score:%.1f", score);
            logAIDecision(finalScoreMsg);
            
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move.first;
                
                char bestMsg[128];
                sprintf(bestMsg, "  *** New best choice! %s score:%.1f", dirName, score);
                logAIDecision(bestMsg);
            }
        }
        
        const char* finalDirName = bestMove == UP ? "UP" : bestMove == DOWN ? "DOWN" : 
                                   bestMove == LEFT ? "LEFT" : "RIGHT";
        char finalDecision[256];
        sprintf(finalDecision, "=== AI Decision Complete === Choice:%s Score:%.1f ===", finalDirName, bestScore);
        logAIDecision(finalDecision);
        
        Point chosenPos = head;
        if (bestMove == UP) chosenPos.y--;
        else if (bestMove == DOWN) chosenPos.y++;
        else if (bestMove == LEFT) chosenPos.x--;
        else if (bestMove == RIGHT) chosenPos.x++;
        
        for (const auto& food : foods) {
            if (chosenPos.x == food.x && chosenPos.y == food.y) {
                char eatMsg[128];
                sprintf(eatMsg, ">> AI chose to move directly to food position!(%d,%d)", chosenPos.x, chosenPos.y);
                logAIDecision(std::string(eatMsg));
                break;
            }
        }
        
        return bestMove;
    }

    void generateFood()
    {
        bool validPosition = false;
        int attempts = 0;
        const int maxAttempts = 100;
        
        while (!validPosition && attempts < maxAttempts)
        {
            Point newFood;
            newFood.x = std::rand() % gridWidth;
            newFood.y = std::rand() % gridHeight;

            validPosition = true;
            for (const auto &segment : snake)
            {
                if (segment.x == newFood.x && segment.y == newFood.y)
                {
                    validPosition = false;
                    break;
                }
            }
            
            for (const auto &food : foods)
            {
                if (food.x == newFood.x && food.y == newFood.y)
                {
                    validPosition = false;
                    break;
                }
            }
            
            if (validPosition) {
                foods.push_back(newFood);
            }
            
            attempts++;
        }
    }
    
    void generateFoodAt(int x, int y)
    {
        foods.push_back({x, y});
    }

    void update()
    {
        if (gameOver)
            return;

        if (shootCooldown > 0) shootCooldown--;
        shoot();
        updateEnemySpawn();
        updateInvulnerability();
        
        if (foods.empty())
        {
            generateFood();
        }
        
        if (aiMode)
        {
            direction = makeAIDecision();
        }

        for (auto bullet = bullets.begin(); bullet != bullets.end();)
        {
            bullet->update();
            Point bulletPos = bullet->getPosition();
            
            if (!bullet->isAlive() || bulletPos.x < 0 || bulletPos.x >= gridWidth || 
                bulletPos.y < 0 || bulletPos.y >= gridHeight)
            {
                bullet = bullets.erase(bullet);
            }
            else
            {
                bool hit = false;
                for (auto &enemy : enemies)
                {
                    if (!enemy.isAlive()) continue;
                    
                    float distance = GameUtils::euclideanDistance(
                        bullet->positionX, 
                        bullet->positionY,
                        static_cast<float>(enemy.position.x), 
                        static_cast<float>(enemy.position.y)
                    );
                    
                    if (distance < 1.0f)
                    {
                        enemy.takeDamage();
                        enemy.knockback(bulletPos, gridWidth, gridHeight);
                        hit = true;
                        
                        if (!enemy.isAlive())
                        {
                            score += 5;
                            generateFoodAt(enemy.position.x, enemy.position.y);
                            
                            int splitCount = std::count(enemy.traits.begin(), enemy.traits.end(), TRAIT_SPLIT);
                            if (splitCount > 0)
                            {
                                std::vector<EnemyTrait> inheritedTraits = enemy.traits;
                                auto splitIt = std::find(inheritedTraits.begin(), inheritedTraits.end(), TRAIT_SPLIT);
                                if (splitIt != inheritedTraits.end())
                                {
                                    inheritedTraits.erase(splitIt);
                                }
                                
                                int offsets[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, 
                                                     {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                                int targetSpawnCount = splitCount * 2;
                                int spawnCount = 0;
                                
                                for (int i = 0; i < 8 && spawnCount < targetSpawnCount; i++)
                                {
                                    int newX = enemy.position.x + offsets[i][0];
                                    int newY = enemy.position.y + offsets[i][1];
                                    
                                    if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight &&
                                        !isPositionOccupied(newX, newY))
                                    {
                                        enemies.push_back(Enemy(newX, newY, inheritedTraits));
                                        spawnCount++;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
                
                if (hit)
                {
                    bullet = bullets.erase(bullet);
                }
                else
                {
                    bullet++;
                }
            }
        }

        for (auto &enemy : enemies)
        {
            if (enemy.isAlive())
            {
                enemy.update(snake);
            }
        }

        Point newHead = snake[0];

        switch (direction)
        {
        case UP:
            newHead.y--;
            break;
        case DOWN:
            newHead.y++;
            break;
        case LEFT:
            newHead.x--;
            break;
        case RIGHT:
            newHead.x++;
            break;
        }

        bool shouldMove = true;
        if (newHead.x < 0 || newHead.x >= gridWidth ||
            newHead.y < 0 || newHead.y >= gridHeight)
        {
            takeDamage();
            if (gameOver) return;
            shouldMove = false;
            newHead = snake[0];
        }

        if (shouldMove)
        {
            for (const auto &segment : snake)
            {
                if (newHead.x == segment.x && newHead.y == segment.y)
                {
                    takeDamage();
                    if (gameOver) return;
                    shouldMove = false;
                    newHead = snake[0];
                    break;
                }
            }
        }

        if (shouldMove)
        {
            for (const auto &enemy : enemies)
            {
                if (!enemy.isAlive()) continue;
                
                if (newHead.x == enemy.position.x && newHead.y == enemy.position.y)
                {
                    takeDamage();
                    if (gameOver) return;
                    shouldMove = false;
                    newHead = snake[0];
                    break;
                }
            }
        }
        
        for (const auto &enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            for (const auto &segment : snake)
            {
                if (segment.x == enemy.position.x && segment.y == enemy.position.y)
                {
                    takeDamage();
                    if (gameOver) return;
                    break;
                }
            }
        }

        snake.insert(snake.begin(), newHead);

        char moveDebug[256];
        sprintf(moveDebug, "DEBUG: Snake head moved to(%d,%d) Foods:%zu", 
                newHead.x, newHead.y, foods.size());
        logAIDecision(moveDebug);
        
        bool ateFood = false;
        for (auto foodIt = foods.begin(); foodIt != foods.end();)
        {
            if (newHead.x == foodIt->x && newHead.y == foodIt->y)
            {
                logAIDecision("DEBUG: Ate food!");
                score++;
                foodCount++;
                foodIt = foods.erase(foodIt);
                ateFood = true;
            }
            else
            {
                foodIt++;
            }
        }
        
        if (foods.empty())
        {
            generateFood();
        }
        
        if (!ateFood)
        {
            logAIDecision("DEBUG: No food eaten, remove tail");
            snake.pop_back();
        }
    }

    void handleInput(Tigr *screen)
    {
        if (tigrKeyDown(screen, 'A'))
        {
            toggleAIMode();
        }
        
        if (!aiMode)
        {
            if (tigrKeyDown(screen, TK_UP) && direction != DOWN)
            {
                direction = UP;
            }
            else if (tigrKeyDown(screen, TK_DOWN) && direction != UP)
            {
                direction = DOWN;
            }
            else if (tigrKeyDown(screen, TK_LEFT) && direction != RIGHT)
            {
                direction = LEFT;
            }
            else if (tigrKeyDown(screen, TK_RIGHT) && direction != LEFT)
            {
                direction = RIGHT;
            }
        }
    }

    size_t getSnakeLength() const { return snake.size(); }
    int getFoodCount() const { return foodCount; }
    
    void draw(Tigr *screen)
    {
        tigrClear(screen, tigrRGB(0x20, 0x20, 0x20));
        
        int borderWidth = gridWidth * cellSize;
        int borderHeight = gridHeight * cellSize;
        int offsetX = (screen->w - borderWidth) / 2;
        int offsetY = (screen->h - borderHeight) / 2;
        
        tigrRect(screen, offsetX, offsetY, borderWidth, borderHeight, tigrRGB(0xFF, 0xFF, 0xFF));

        for (size_t i = 0; i < snake.size(); i++)
        {
            int x = offsetX + snake[i].x * cellSize;
            int y = offsetY + snake[i].y * cellSize;

            if (i == 0)
            {
                if (isHit && (invulnerableTimer % 10 < 5))
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0xFF, 0x00));
                }
                else
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0x00, 0xFF, 0x00));
                }
            }
            else
            {
                if (isHit && (invulnerableTimer % 10 < 5))
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0x80, 0x00));
                }
                else
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0x00, 0x80, 0x00));
                }
            }
        }

        for (const auto &food : foods)
        {
            tigrFillRect(screen, offsetX + food.x * cellSize, offsetY + food.y * cellSize, cellSize, cellSize, tigrRGB(0xFF, 0x00, 0x00));
        }
        
        for (const auto &enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            int x = offsetX + enemy.position.x * cellSize;
            int y = offsetY + enemy.position.y * cellSize;
            
            int drawSize = cellSize;
            if (enemy.hasTrait(TRAIT_GIANT))
            {
                drawSize = static_cast<int>(cellSize * enemy.size);
                x -= (drawSize - cellSize) / 2;
                y -= (drawSize - cellSize) / 2;
            }
            
            int healthRatio = (enemy.currentHealth * 255) / enemy.maxHealth;
            TPixel enemyColor = tigrRGB(255, 165 - (165 - healthRatio), 0);
            tigrFillRect(screen, x, y, drawSize, drawSize, enemyColor);
            
            TPixel borderColor = tigrRGB(0xFF, 0x00, 0x00);
            
            if (enemy.hasTrait(TRAIT_SPEED_BOOST))
                borderColor = tigrRGB(0x00, 0x00, 0xFF);
            else if (enemy.hasTrait(TRAIT_HEALTH_BOOST))
                borderColor = tigrRGB(0x00, 0xFF, 0x00);
            else if (enemy.hasTrait(TRAIT_KNOCKBACK_RESIST))
                borderColor = tigrRGB(0x80, 0x00, 0x80);
            else if (enemy.hasTrait(TRAIT_SPLIT))
                borderColor = tigrRGB(0xFF, 0xFF, 0x00);
            
            tigrRect(screen, x, y, drawSize, drawSize, borderColor);
            
            if (enemy.traits.size() > 1)
            {
                TPixel secondBorderColor = tigrRGB(0xFF, 0xFF, 0xFF);
                if (enemy.traits.size() > 1)
                {
                    if (enemy.traits[1] == TRAIT_SPEED_BOOST)
                        secondBorderColor = tigrRGB(0x00, 0x00, 0xFF);
                    else if (enemy.traits[1] == TRAIT_HEALTH_BOOST)
                        secondBorderColor = tigrRGB(0x00, 0xFF, 0x00);
                    else if (enemy.traits[1] == TRAIT_KNOCKBACK_RESIST)
                        secondBorderColor = tigrRGB(0x80, 0x00, 0x80);
                    else if (enemy.traits[1] == TRAIT_SPLIT)
                        secondBorderColor = tigrRGB(0xFF, 0xFF, 0x00);
                }
                tigrRect(screen, x + 1, y + 1, drawSize - 2, drawSize - 2, secondBorderColor);
            }
            
            if (enemy.currentHealth < enemy.maxHealth)
            {
                int barWidth = drawSize - 2;
                int barHeight = 2;
                int barX = x + 1;
                int barY = y - 4;
                
                tigrFillRect(screen, barX, barY, barWidth, barHeight, tigrRGB(0x80, 0x00, 0x00));
                
                int healthWidth = (enemy.currentHealth * barWidth) / enemy.maxHealth;
                tigrFillRect(screen, barX, barY, healthWidth, barHeight, tigrRGB(0x00, 0xFF, 0x00));
            }
        }
        
        for (const auto &bullet : bullets)
        {
            int x = offsetX + static_cast<int>(bullet.positionX) * cellSize + cellSize / 2;
            int y = offsetY + static_cast<int>(bullet.positionY) * cellSize + cellSize / 2;
            
            tigrFillCircle(screen, x, y, cellSize / 3, tigrRGB(0x00, 0x00, 0xFF));
            tigrCircle(screen, x, y, cellSize / 3, tigrRGB(0x00, 0x80, 0xFF));
        }

        char scoreText[32];
        sprintf(scoreText, "Score: %d", score);
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 5, tigrRGB(0xFF, 0xFF, 0xFF), scoreText);
        
        char lengthText[32];
        sprintf(lengthText, "Length: %zu", snake.size());
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 20, tigrRGB(0xFF, 0xFF, 0xFF), lengthText);
        
        if (invulnerableTimer > 0)
        {
            char invulnText[32];
            sprintf(invulnText, "Shield: %.1f", invulnerableTimer / 10.0f);
            tigrPrint(screen, tfont, offsetX + 5, offsetY + 35, tigrRGB(0xFF, 0xFF, 0x00), invulnText);
        }
        
        const char* modeText = aiMode ? "AI Mode: ON (Press A to toggle)" : "Manual Mode (Press A for AI)";
        TPixel modeColor = aiMode ? tigrRGB(0x00, 0xFF, 0x00) : tigrRGB(0xFF, 0xFF, 0xFF);
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 50, modeColor, modeText);

        if (gameOver)
        {
            const char *gameOverText = "Game Over! Press ESC to exit";
            int textWidth = tigrTextWidth(tfont, gameOverText);
            tigrPrint(screen, tfont, (screen->w - textWidth) / 2, screen->h / 2,
                      tigrRGB(0xFF, 0xFF, 0xFF), gameOverText);
        }
    }

    bool isGameOver() const { return gameOver; }
};

bool SnakeGame::firstRun = true;

int main(int argc, char *argv[])
{
    Tigr *screen = tigrWindow(640, 480, "Snake Game", TIGR_AUTO);
    SnakeGame game(640, 480);

    float accumulator = 0.0f;
    float baseUpdateInterval = 0.1f;

    while (!tigrClosed(screen))
    {
        if (game.isGameOver())
        {
            break;
        }

        float deltaTime = tigrTime();
        accumulator += deltaTime;
        game.handleInput(screen);

        float moveSpeedBonus = (game.getFoodCount() / 10) * 0.01f;
        float currentUpdateInterval = baseUpdateInterval - moveSpeedBonus;

        if (accumulator >= currentUpdateInterval)
        {
            game.update();
            accumulator -= currentUpdateInterval;
        }

        game.draw(screen);

        if (tigrKeyDown(screen, TK_ESCAPE))
        {
            break;
        }

        tigrUpdate(screen);
    }

    game.flushLogBuffer();
    tigrFree(screen);
    return 0;
}

const int SnakeGame::DIRECTIONS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};