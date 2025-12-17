#include "tigr.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cfloat>
#include <queue>
#include <algorithm>

struct Point
{
    int x, y;
};

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum EnemyTrait
{
    TRAIT_SPEED_BOOST,      // 速度提升
    TRAIT_HEALTH_BOOST,     // 血量提升
    TRAIT_KNOCKBACK_RESIST, // 击退抗性
    TRAIT_SPLIT,            // 分裂（死亡时分裂成小敌人）
    TRAIT_GIANT,            // 巨大（体型更大）
    TRAIT_COUNT             // 特性总数
};

class Bullet
{
public:
    float positionX, positionY;  // 使用浮点数位置
    float directionX, directionY;  // 使用浮点数方向
    float speed;
    int lifetime;
    
    Bullet(Point startPos, Point targetPos, float bulletSpeed)
    {
        positionX = startPos.x;
        positionY = startPos.y;
        speed = bulletSpeed;
        lifetime = 50;  // 炮弹生存时间
        
        // 计算方向向量
        float dx = targetPos.x - startPos.x;
        float dy = targetPos.y - startPos.y;
        float length = sqrt(dx * dx + dy * dy);
        
        if (length > 0)
        {
            directionX = dx / length;
            directionY = dy / length;
        }
        else
        {
            directionX = 0;
            directionY = -1;  // 默认向上
        }
    }
    
    void update()
    {
        lifetime--;
        positionX += directionX * speed;
        positionY += directionY * speed;
    }
    
    bool isAlive() const { return lifetime > 0; }
    
    // 获取整数位置用于碰撞检测
    Point getPosition() const 
    { 
        return {static_cast<int>(positionX), static_cast<int>(positionY)}; 
    }
};

class Enemy
{
public:
    Point position;
    float speed;
    int moveCounter;
    int maxHealth;
    int currentHealth;
    int knockbackCounter;
    std::vector<EnemyTrait> traits;
    float size;  // 敌人大小（用于巨大特性）
    
    // 默认数值
    static constexpr float BASE_SPEED = 15.0f;
    static constexpr int BASE_HEALTH = 3;
    static constexpr int BASE_KNOCKBACK = 20;
    static constexpr float BASE_SIZE = 1.0f;
    
    Enemy(int x, int y, const std::vector<EnemyTrait>& enemyTraits = {})
    {
        position.x = x;
        position.y = y;
        traits = enemyTraits;
        moveCounter = 0;
        
        // 应用默认数值
        speed = BASE_SPEED;
        maxHealth = BASE_HEALTH;
        size = BASE_SIZE;
        
        // 应用特性效果
        for (const auto& trait : traits)
        {
            applyTrait(trait);
        }
        
        currentHealth = maxHealth;
        knockbackCounter = 0;
    }
    
    bool hasTrait(EnemyTrait trait) const
    {
        return std::find(traits.begin(), traits.end(), trait) != traits.end();
    }
    
    void applyTrait(EnemyTrait trait)
    {
        switch (trait)
        {
            case TRAIT_SPEED_BOOST:
                speed *= 0.6f;  // 速度提升（移动间隔减少40%，可叠加）
                break;
            case TRAIT_HEALTH_BOOST:
                maxHealth += 3;  // 血量+3（可叠加）
                break;
            case TRAIT_KNOCKBACK_RESIST:
                // 击退抗性在takeDamage中处理（可叠加）
                break;
            case TRAIT_SPLIT:
                // 分裂在死亡时处理（可叠加，会产生更多小敌人）
                break;
            case TRAIT_GIANT:
                size += 0.5f;  // 体型每次增加0.5倍（可叠加）
                maxHealth += 2;  // 每次额外+2血量（可叠加）
                break;
            default:
                break;
        }
    }
    
    void update(const std::vector<Point>& snake)
    {
        // 处理击退效果
        if (knockbackCounter > 0)
        {
            knockbackCounter--;
            return;  // 击退期间不移动
        }
        
        moveCounter++;
        if (moveCounter >= speed)
        {
            moveCounter = 0;
            
            // 找到最近的蛇身体部分
            Point nearestTarget = findNearestSnakeSegment(snake);
            
            // 追踪最近的蛇身体部分
            if (nearestTarget.x < position.x) position.x--;
            else if (nearestTarget.x > position.x) position.x++;
            
            if (nearestTarget.y < position.y) position.y--;
            else if (nearestTarget.y > position.y) position.y++;
        }
    }
    
    // 找到最近的蛇身体部分
    Point findNearestSnakeSegment(const std::vector<Point>& snake) const
    {
        if (snake.empty()) return {0, 0};
        
        Point nearest = snake[0];
        float minDistance = FLT_MAX;
        
        for (const auto& segment : snake)
        {
            float dx = segment.x - position.x;
            float dy = segment.y - position.y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = segment;
            }
        }
        
        return nearest;
    }
    
    void takeDamage()
    {
        currentHealth--;
        
        // 根据击退抗性特性调整击退时间（支持叠加）
        float knockback = BASE_KNOCKBACK;
        int resistCount = std::count(traits.begin(), traits.end(), TRAIT_KNOCKBACK_RESIST);
        for (int i = 0; i < resistCount; i++)
        {
            knockback *= 0.8f;  // 每个击退抗性减少20%击退时间（可叠加）
        }
        knockbackCounter = static_cast<int>(knockback);
    }
    
    void knockback(const Point& from, int maxWidth, int maxHeight)
    {
        // 计算击退方向
        int dx = position.x - from.x;
        int dy = position.y - from.y;
        
        // 归一化并应用击退
        if (dx != 0) position.x += (dx > 0 ? 2 : -2);
        if (dy != 0) position.y += (dy > 0 ? 2 : -2);
        
        // 确保不超出边界
        if (position.x < 0) position.x = 0;
        if (position.x >= maxWidth) position.x = maxWidth - 1;
        if (position.y < 0) position.y = 0;
        if (position.y >= maxHeight) position.y = maxHeight - 1;
    }
    
    bool isAlive() const { return currentHealth > 0; }
};

class SnakeGame
{
private:
    std::vector<Point> snake;
    std::vector<Point> foods;  // 支持多个食物
    Direction direction;
    int cellSize;
    int gridWidth, gridHeight;
    bool gameOver;
    int score;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    int shootCooldown;
    
    // 敌人生成系统
    int enemySpawnTimer;
    int enemySpawnInterval;
    int minEnemies;  // 最少敌人数量限制
    int gameTimer;  // 游戏运行时间，用于难度递增
    
    // 蛇的生命系统
    int invulnerableTimer;
    bool isHit;
    
    // AI模式控制
    bool aiMode;
    
    // 食物计数器系统
    int foodCount;  // 吃到的食物总数
    
    // 日志缓冲区系统
    std::vector<std::string> logBuffer;
    static const int LOG_BUFFER_SIZE = 100;  // 缓冲区大小
    int logCounter;  // 日志计数器，用于控制刷新频率
    
    // 找到最近的敌人
    Enemy* findNearestEnemy()
    {
        if (enemies.empty()) return nullptr;
        
        Enemy* nearest = nullptr;
        float minDistance = FLT_MAX;
        
        for (auto& enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            float dx = enemy.position.x - snake[0].x;
            float dy = enemy.position.y - snake[0].y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = &enemy;
            }
        }
        
        return nearest;
    }
    
    // 发射炮弹
    void shoot()
    {
        if (shootCooldown > 0) return;
        
        Enemy* target = findNearestEnemy();
        if (target)
        {
            // 从蛇头发射炮弹
            bullets.push_back(Bullet(snake[0], target->position, 0.8f));
            
            // 根据吃到的食物数量计算冷却时间：食物越多，冷却越短（攻击频率越高）
            float attackSpeedBonus = (foodCount / 5) * 2.0f; // 每吃5个食物，冷却时间减少2帧
            int dynamicCooldown = 20 - static_cast<int>(attackSpeedBonus);
            shootCooldown = dynamicCooldown;
        }
    }
    
    // 检查位置是否被占用
    bool isPositionOccupied(int x, int y)
    {
        // 检查蛇身
        for (const auto& segment : snake)
        {
            if (segment.x == x && segment.y == y) return true;
        }
        
        // 检查敌人
        for (const auto& enemy : enemies)
        {
            if (enemy.isAlive() && enemy.position.x == x && enemy.position.y == y) return true;
        }
        
        // 检查食物
        for (const auto &food : foods)
        // 检查食物
        for (const auto &food : foods)
        {
            if (food.x == x && food.y == y) return true;
        }
        
        return false;
    }
    
    // 生成新敌人
    void spawnEnemy()
    {
        // 移除最大敌人数量限制，允许无限生成
        
        // 尝试在边界附近生成敌人
        const int maxAttempts = 50;
        bool spawned = false;
        
        for (int attempt = 0; attempt < maxAttempts && !spawned; attempt++)
        {
            int x, y;
            int side = std::rand() % 4;  // 随机选择四个边之一
            
            switch (side)
            {
            case 0:  // 上边
                x = std::rand() % gridWidth;
                y = 0;
                break;
            case 1:  // 右边
                x = gridWidth - 1;
                y = std::rand() % gridHeight;
                break;
            case 2:  // 下边
                x = std::rand() % gridWidth;
                y = gridHeight - 1;
                break;
            case 3:  // 左边
                x = 0;
                y = std::rand() % gridHeight;
                break;
            default:
                continue;
            }
            
            // 确保位置不被占用且不要太靠近蛇头
            if (!isPositionOccupied(x, y))
            {
                float dx = x - snake[0].x;
                float dy = y - snake[0].y;
                float distance = sqrt(dx * dx + dy * dy);
                
                if (distance > 5.0f)  // 至少距离蛇头5格
                {
                    // 根据游戏时间决定特性数量（每30秒增加1个，无上限）
                    int maxTraitCount = gameTimer / 300;  // 每30秒增加1个特性
                    
                    // 随机抽取特性（允许同一种特性叠加）
                    std::vector<EnemyTrait> selectedTraits;
                    
                    int traitCount = std::rand() % (maxTraitCount + 1);  // 0到maxTraitCount个特性
                    for (int i = 0; i < traitCount; i++)
                    {
                        // 随机选择一个特性（可以重复）
                        EnemyTrait trait = static_cast<EnemyTrait>(std::rand() % TRAIT_COUNT);
                        selectedTraits.push_back(trait);
                    }
                    
                    enemies.push_back(Enemy(x, y, selectedTraits));
                    spawned = true;
                }
            }
        }
    }
    
    // 更新敌人生成系统
    void updateEnemySpawn()
    {
        gameTimer++;
        enemySpawnTimer++;
        
        // 计算当前存活的敌人数量
        int aliveEnemies = 0;
        for (const auto& enemy : enemies)
        {
            if (enemy.isAlive()) aliveEnemies++;
        }
        
        // 动态计算最少敌人数量：基础数量 + 时间递增（无上限）
        int dynamicMinEnemies = minEnemies + (gameTimer / 600);  // 每分钟增加1个最少敌人
        
        // 如果存活敌人少于动态最少数量，立即生成新敌人
        if (aliveEnemies < dynamicMinEnemies)
        {
            spawnEnemy();
            enemySpawnTimer = 0;
            return;  // 本帧不再进行常规生成检查
        }
        
        // 随着时间推移，加快敌人生成速度
        int currentInterval = enemySpawnInterval - (gameTimer / 300);  // 每30秒加快一次生成（更快）
        if (currentInterval < 50) currentInterval = 50;  // 更小的最小生成间隔限制
        
        if (enemySpawnTimer >= currentInterval)
        {
            spawnEnemy();
            enemySpawnTimer = 0;
        }
    }
    
    // 蛇受伤处理
    void takeDamage()
    {
        if (invulnerableTimer > 0) return;  // 无敌时间内不受伤
        
        // 如果蛇长度大于1，减少长度
        if (snake.size() > 1)
        {
            snake.pop_back();  // 移除尾部
            invulnerableTimer = 10;  // 1秒无敌时间（假设每秒10次更新）
            isHit = true;
        }
        else
        {
            // 长度为1时，游戏结束
            gameOver = true;
        }
    }
    
    // 更新无敌时间
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

        // 初始化蛇，从中间位置开始，长度为3
        snake.push_back({gridWidth / 2, gridHeight / 2});
        snake.push_back({gridWidth / 2 - 1, gridHeight / 2});
        snake.push_back({gridWidth / 2 - 2, gridHeight / 2});

        direction = RIGHT;
        gameOver = false;
        score = 0;

        // 设置随机种子
        std::srand(std::time(0));

        // 生成第一个食物
        generateFood();
        
        // 初始化敌人
        enemies.clear();
        // 在四个角落各放置一个敌人（初始无特性）
        enemies.push_back(Enemy(2, 2, {}));  // 左上角
        enemies.push_back(Enemy(gridWidth - 3, 2, {}));  // 右上角
        enemies.push_back(Enemy(2, gridHeight - 3, {}));  // 左下角
        enemies.push_back(Enemy(gridWidth - 3, gridHeight - 3, {}));  // 右下角
        
        // 初始化炮弹系统
        bullets.clear();
        shootCooldown = 0;
        
        // 初始化敌人生成系统
        enemySpawnTimer = 0;
        enemySpawnInterval = 150;  // 初始15秒生成一个新敌人（加快刷新频率）
        minEnemies = 4;   // 最少保持4个敌人
        gameTimer = 0;
        
        // 初始化蛇的生命系统
        invulnerableTimer = 0;
        isHit = false;
        
        // 初始化AI模式
        aiMode = true;
        
        // 初始化食物计数器系统
        foodCount = 0;
        
        // 初始化日志缓冲区系统
        logCounter = 0;
        logBuffer.reserve(LOG_BUFFER_SIZE);  // 预分配内存
    }
    
    // AI模式控制
    void toggleAIMode() { aiMode = !aiMode; }
    bool isAIMode() const { return aiMode; }
    
    // 计算两点之间的曼哈顿距离
    int manhattanDistance(const Point& a, const Point& b) const
    {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }
    
    // 检查位置是否安全
    bool isSafePosition(const Point& pos, bool allowFood = true) const
    {
        // 检查边界
        if (pos.x < 0 || pos.x >= gridWidth || pos.y < 0 || pos.y >= gridHeight)
            return false;
        
        // 检查与蛇身的碰撞
        for (const auto& segment : snake)
        {
            if (pos.x == segment.x && pos.y == segment.y)
                return false;
        }
        
        // 检查与敌人的碰撞
        for (const auto& enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            // 直接碰撞
            if (pos.x == enemy.position.x && pos.y == enemy.position.y)
                return false;
            
            // 预测敌人下一步位置
            int enemySpeed = static_cast<int>(enemy.speed);
            if (enemy.moveCounter >= enemySpeed - 1)
            {
                Point predictedEnemyPos = enemy.position;
                
                // 敌人会向最近的蛇身体部分移动
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
                
                // 预测敌人移动方向
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
    
    // BFS寻路到目标
    std::vector<Point> findPath(const Point& start, const Point& target) const
    {
        // 如果起点就是目标，返回空路径（已经在目标位置）
        if (start.x == target.x && start.y == target.y) {
            return {};
        }
        
        std::vector<std::vector<bool>> visited(gridWidth, std::vector<bool>(gridHeight, false));
        std::vector<std::vector<Point>> parent(gridWidth, std::vector<Point>(gridHeight, {-1, -1}));
        std::queue<Point> queue;
        
        queue.push(start);
        visited[start.x][start.y] = true;
        
        int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 上、下、左、右
        
        while (!queue.empty())
        {
            Point current = queue.front();
            queue.pop();
            
            if (current.x == target.x && current.y == target.y)
            {
                // 找到路径，回溯构建路径
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
                
                // 检查边界
                if (next.x < 0 || next.x >= gridWidth || next.y < 0 || next.y >= gridHeight)
                    continue;
                
                // 检查是否已访问
                if (visited[next.x][next.y])
                    continue;
                
                // 检查安全性
                bool isTarget = (next.x == target.x && next.y == target.y);
                bool safe = false;
                
                if (isTarget) {
                    // 目标位置：只检查边界和蛇身，允许食物位置
                    safe = true;
                    for (const auto& segment : snake) {
                        if (next.x == segment.x && next.y == segment.y) {
                            safe = false;
                            break;
                        }
                    }
                } else {
                    // 非目标位置：检查所有安全性
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
        
        return {}; // 没有找到路径
    }
    
    // 评估可达区域大小（避免进入死路）
    int evaluateAccessibleArea(const Point& start) const
    {
        std::vector<std::vector<bool>> visited(gridWidth, std::vector<bool>(gridHeight, false));
        std::queue<Point> queue;
        int areaSize = 0;
        
        queue.push(start);
        visited[start.x][start.y] = true;
        
        int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        
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
    
    // 预测敌人位置（更准确的预测）
    Point predictEnemyPosition(const Enemy& enemy, int steps = 2) const
    {
        Point predictedPos = enemy.position;
        
        for (int step = 0; step < steps; step++)
        {
            // 找到最近的蛇身体部分
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
            
            // 预测移动方向
            if (nearestSegment.x < predictedPos.x) predictedPos.x--;
            else if (nearestSegment.x > predictedPos.x) predictedPos.x++;
            else if (nearestSegment.y < predictedPos.y) predictedPos.y--;
            else if (nearestSegment.y > predictedPos.y) predictedPos.y++;
        }
        
        return predictedPos;
    }
    
    // AI日志输出（使用缓冲区）
    void logAIDecision(const std::string& message)
    {
        // 添加到缓冲区
        logBuffer.push_back(message);
        logCounter++;
        
        // 当缓冲区满了或每10条日志刷新一次
        if (logBuffer.size() >= LOG_BUFFER_SIZE || logCounter % 10 == 0)
        {
            flushLogBuffer();
        }
    }
    
    // 刷新日志缓冲区到文件
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
    
    // 静态标志，用于判断是否首次运行
    static bool firstRun;
    
    // AI决策逻辑（优化版）
    Direction makeAIDecision()
    {
        Point head = snake[0];
        
        // 只在首次运行时清空日志文件
        if (firstRun)
        {
            FILE* logFile = fopen("ai_log.txt", "w");
            if (logFile)
            {
                fprintf(logFile, "=== 贪吃蛇AI日志开始 ===\n");
                fprintf(logFile, "游戏时间: %I64d\n", (long long)time(nullptr));
                fclose(logFile);
            }
            firstRun = false;
        }
        
        // 记录当前状态
        char statusMsg[256];
        sprintf(statusMsg, "\n=== AI决策开始 [%I64d] === 蛇头:(%d,%d) 食物数:%zu 蛇长:%zu 方向:%s", 
                (long long)time(nullptr), head.x, head.y, foods.size(), snake.size(),
                direction == UP ? "UP" : direction == DOWN ? "DOWN" : direction == LEFT ? "LEFT" : "RIGHT");
        logAIDecision(statusMsg);
        
        // 找到最近的食物作为目标
        Point targetFood = {-1, -1};  // 无效位置表示没有食物
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
        
        // 记录敌人信息
        for (size_t i = 0; i < enemies.size(); i++)
        {
            if (enemies[i].isAlive())
            {
                char enemyMsg[128];
                sprintf(enemyMsg, "敌人%zu: 位置(%d,%d) 血量:%d 速度:%.1f", 
                        i, enemies[i].position.x, enemies[i].position.y, 
                        enemies[i].currentHealth, enemies[i].speed);
                logAIDecision(enemyMsg);
            }
        }
        
        // 获取可能的移动方向
        std::vector<std::pair<Direction, Point>> possibleMoves;
        
        if (direction != DOWN) possibleMoves.push_back({UP, {head.x, head.y - 1}});
        if (direction != UP) possibleMoves.push_back({DOWN, {head.x, head.y + 1}});
        if (direction != RIGHT) possibleMoves.push_back({LEFT, {head.x - 1, head.y}});
        if (direction != LEFT) possibleMoves.push_back({RIGHT, {head.x + 1, head.y}});
        
        // 为每个移动方向评分
        Direction bestMove = direction;
        float bestScore = -FLT_MAX;
        
        for (const auto& move : possibleMoves)
        {
            float score = 0.0f;
            Point nextPos = move.second;
            
            const char* dirName = move.first == UP ? "UP" : move.first == DOWN ? "DOWN" : 
                                 move.first == LEFT ? "LEFT" : "RIGHT";
            
            char moveMsg[128];
            sprintf(moveMsg, "--- 评估方向: %s 到位置(%d,%d) ---", dirName, nextPos.x, nextPos.y);
            logAIDecision(moveMsg);
            
            // 基础安全性检查
            if (!isSafePosition(nextPos))
            {
                score -= 1000.0f; // 大幅惩罚不安全的位置
                logAIDecision("  ❌ 位置不安全! -1000分");
            }
            else
            {
                logAIDecision("  ✅ 位置安全");
                
                // 1. 路径可达性评估 - 是否能到达食物
                bool canEatFood = false;
                int pathLength = 0;
                
                // 检查是否可以直接吃到食物（nextPos就是食物位置）
                bool directFood = false;
                for (const auto& food : foods) {
                    if (nextPos.x == food.x && nextPos.y == food.y) {
                        canEatFood = true;
                        pathLength = 0;  // 直接吃食物
                        char directMsg[128];
                        sprintf(directMsg, "  🎯 可以直接吃到食物! 位置(%d,%d)", nextPos.x, nextPos.y);
                        logAIDecision(directMsg);
                        directFood = true;
                        break;
                    }
                }
                
                if (!directFood) {
                    // 否则寻找路径到最近的食物
                    Point targetFood = {-1, -1}; // 无效位置
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
                            sprintf(pathMsg, "  📍 找到食物路径! 长度:%d", pathLength);
                            logAIDecision(pathMsg);
                        }
                    }
                }
                
                if (canEatFood)
                {
                    // 基础食物奖励
                    score += 1000.0f;
                    
                    // 路径长度奖励：路径越短得分越高
                    if (pathLength == 0) {
                        // 直接吃食物，给予最高奖励
                        score += 1000.0f;
                        char bonusMsg[128];
                        sprintf(bonusMsg, "  🏆 直接吃食物! 超级奖励:1000.0");
                        logAIDecision(bonusMsg);
                    } else {
                        // 需要移动，路径越短得分越高
                        score += (200.0f / pathLength); // 增加权重
                        char pathScoreMsg[128];
                        sprintf(pathScoreMsg, "  📍 路径长度奖励:%.1f", 200.0f / pathLength);
                        logAIDecision(pathScoreMsg);
                        
                        // 额外奖励：如果下一步就能吃到食物
                        if (pathLength == 1) {
                            score += 500.0f;
                            char bonusMsg[128];
                            sprintf(bonusMsg, "  🎯 下一步就能吃到食物! 额外奖励:500.0");
                            logAIDecision(bonusMsg);
                        }
                    }
                }
                else
                {
                    // 详细检查为什么无法到达食物
                    bool directToFood = isSafePosition(targetFood, true);
                    char debugMsg[256];
                    sprintf(debugMsg, "  ⚠️  无法到达食物 - 直接检查食物位置(%d,%d)安全性:%s", 
                            targetFood.x, targetFood.y, directToFood ? "安全" : "不安全");
                    logAIDecision(debugMsg);
                    
                    // 检查食物位置是否与蛇身重叠
                    bool foodOnSnake = false;
                    for (const auto& segment : snake) {
                        if (segment.x == targetFood.x && segment.y == targetFood.y) {
                            foodOnSnake = true;
                            break;
                        }
                    }
                    if (foodOnSnake) {
                        logAIDecision("    原因：食物位置与蛇身重叠");
                    }
                }
                
                // 2. 可达区域大小评估（避免死路）
                int accessibleArea = evaluateAccessibleArea(nextPos);
                score += accessibleArea * 0.5f; // 降低权重，避免过度保守
                
                char areaMsg[128];
                sprintf(areaMsg, "  🗺️  可达区域大小:%d 得分:%.1f", accessibleArea, accessibleArea * 0.5f);
                logAIDecision(areaMsg);
                
                // 3. 敌人威胁评估（更精确的预测）
                float totalThreat = 0.0f;
                for (const auto& enemy : enemies)
                {
                    if (!enemy.isAlive()) continue;
                    
                    // 当前距离
                    float currentDist = manhattanDistance(nextPos, enemy.position);
                    
                    // 预测未来距离
                    Point predictedEnemy = predictEnemyPosition(enemy, 2);
                    float futureDist = manhattanDistance(nextPos, predictedEnemy);
                    
                    // 威胁评估：距离越近威胁越大
                    float enemyThreat = 0.0f;
                    if (currentDist < 5.0f)
                    {
                        enemyThreat += (5.0f - currentDist) * 100.0f;
                    }
                    if (futureDist < 5.0f)
                    {
                        enemyThreat += (5.0f - futureDist) * 150.0f; // 未来威胁权重更大
                    }
                    
                    // 敌人朝向蛇的威胁
                    int dx = snake[0].x - enemy.position.x;
                    int dy = snake[0].y - enemy.position.y;
                    if (abs(dx) <= 2 && abs(dy) <= 2)
                    {
                        enemyThreat += 50.0f;
                    }
                    
                    totalThreat += enemyThreat;
                    
                    char threatMsg[256];
                    sprintf(threatMsg, "    👹 敌人(%d,%d) 当前距离:%.1f 预测距离:%.1f 威胁值:%.1f", 
                            enemy.position.x, enemy.position.y, currentDist, futureDist, enemyThreat);
                    logAIDecision(threatMsg);
                }
                
                char totalThreatMsg[128];
                sprintf(totalThreatMsg, "  ⚠️  总威胁值:%.1f 扣分:%.1f", totalThreat, -totalThreat);
                logAIDecision(totalThreatMsg);
                score -= totalThreat;
                
                // 4. 食物导向评分（更智能的策略）
                int foodDist = manhattanDistance(nextPos, targetFood);
                
                // 根据蛇的长度调整策略，但整体增加权重
                if (snake.size() <= 5)
                {
                    // 短蛇：非常积极寻找食物
                    score += (500.0f - foodDist * 15.0f);
                }
                else if (snake.size() <= 10)
                {
                    // 中等蛇：积极策略
                    score += (400.0f - foodDist * 12.0f);
                }
                else
                {
                    // 长蛇：平衡策略
                    score += (300.0f - foodDist * 10.0f);
                }
                
                // 5. 空间开阔度评估
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
                
                // 6. 墙壁距离评估
                float wallDist = std::min({nextPos.x, nextPos.y, 
                                         gridWidth - 1 - nextPos.x, 
                                         gridHeight - 1 - nextPos.y});
                score += wallDist * 15.0f;
                
                // 7. 蛇身连续性评估
                // 检查移动后是否会导致蛇身被自己困住
                std::vector<Point> futureSnake = snake;
                futureSnake.insert(futureSnake.begin(), nextPos);
                if (!futureSnake.empty())
                {
                    futureSnake.pop_back(); // 模拟蛇尾移动
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
            sprintf(finalScoreMsg, "  📊 最终得分:%.1f", score);
            logAIDecision(finalScoreMsg);
            
            // 选择得分最高的移动
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move.first;
                
                char bestMsg[128];
                sprintf(bestMsg, "  🏆 新的最佳选择! %s 得分:%.1f", dirName, score);
                logAIDecision(bestMsg);
            }
        }
        
        const char* finalDirName = bestMove == UP ? "UP" : bestMove == DOWN ? "DOWN" : 
                                   bestMove == LEFT ? "LEFT" : "RIGHT";
        char finalDecision[256];
        sprintf(finalDecision, "=== AI决策完成 === 选择:%s 得分:%.1f ===", finalDirName, bestScore);
        logAIDecision(finalDecision);
        
        // 额外调试：检查选择的移动是否直接到食物
        Point chosenPos = head;
        if (bestMove == UP) chosenPos.y--;
        else if (bestMove == DOWN) chosenPos.y++;
        else if (bestMove == LEFT) chosenPos.x--;
        else if (bestMove == RIGHT) chosenPos.x++;
        
        // 检查是否选择了直接移动到某个食物位置
        for (const auto& food : foods) {
            if (chosenPos.x == food.x && chosenPos.y == food.y) {
                char eatMsg[128];
                sprintf(eatMsg, "🎯 AI选择了直接移动到食物位置!(%d,%d)", chosenPos.x, chosenPos.y);
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
        // 在指定位置生成食物（敌人死亡时使用）
        foods.push_back({x, y});
    }

    void update()
    {
        if (gameOver)
            return;

        // 更新射击冷却
        if (shootCooldown > 0) shootCooldown--;

        // 自动发射炮弹
        shoot();
        
        // 更新敌人生成系统
        updateEnemySpawn();
        
        // 更新无敌时间
        updateInvulnerability();
        
        // 确保至少存在一个食物（定期检查）
        if (foods.empty())
        {
            generateFood();
        }
        
        // AI模式：自动决策
        if (aiMode)
        {
            direction = makeAIDecision();
        }

        // 更新炮弹位置
        for (auto bullet = bullets.begin(); bullet != bullets.end();)
        {
            bullet->update();
            
            // 获取炮弹的整数位置
            Point bulletPos = bullet->getPosition();
            
            // 检查炮弹是否超出边界或生存时间结束
            if (!bullet->isAlive() || bulletPos.x < 0 || bulletPos.x >= gridWidth || 
                bulletPos.y < 0 || bulletPos.y >= gridHeight)
            {
                bullet = bullets.erase(bullet);
            }
            else
            {
                // 检查炮弹与敌人的碰撞
                bool hit = false;
                for (auto &enemy : enemies)
                {
                    if (!enemy.isAlive()) continue;
                    
                    // 简单的圆形碰撞检测
                    float dx = bullet->positionX - enemy.position.x;
                    float dy = bullet->positionY - enemy.position.y;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    if (distance < 1.0f)  // 碰撞半径
                    {
                        enemy.takeDamage();
                        enemy.knockback(bulletPos, gridWidth, gridHeight);
                        hit = true;
                        
                        // 如果敌人被消灭，增加分数
                        if (!enemy.isAlive())
                        {
                            score += 5;  // 消灭敌人获得额外分数
                            
                            // 敌人死亡后掉落食物
                            generateFoodAt(enemy.position.x, enemy.position.y);
                            
                            // 检查分裂特性（支持叠加）
                            int splitCount = std::count(enemy.traits.begin(), enemy.traits.end(), TRAIT_SPLIT);
                            if (splitCount > 0)
                            {
                                // 准备继承的特性（移除一个分裂特性）
                                std::vector<EnemyTrait> inheritedTraits = enemy.traits;
                                auto splitIt = std::find(inheritedTraits.begin(), inheritedTraits.end(), TRAIT_SPLIT);
                                if (splitIt != inheritedTraits.end())
                                {
                                    inheritedTraits.erase(splitIt);  // 移除一个分裂特性
                                }
                                
                                // 每个分裂特性产生2个小敌人
                                int offsets[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, 
                                                     {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                                int targetSpawnCount = splitCount * 2;
                                int spawnCount = 0;
                                
                                for (int i = 0; i < 8 && spawnCount < targetSpawnCount; i++)
                                {
                                    int newX = enemy.position.x + offsets[i][0];
                                    int newY = enemy.position.y + offsets[i][1];
                                    
                                    // 检查位置是否有效
                                    if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight &&
                                        !isPositionOccupied(newX, newY))
                                    {
                                        enemies.push_back(Enemy(newX, newY, inheritedTraits));  // 继承特性（分裂-1）
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

        // 更新敌人位置
        for (auto &enemy : enemies)
        {
            if (enemy.isAlive())
            {
                enemy.update(snake);  // 敌人追踪最近的蛇身体部分
            }
        }

        // 计算新的头部位置
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

        // 检查墙壁碰撞
        if (newHead.x < 0 || newHead.x >= gridWidth ||
            newHead.y < 0 || newHead.y >= gridHeight)
        {
            takeDamage();
            if (gameOver) return;
            
            // 如果没有死亡，不移动到新位置
            return;
        }

        // 检查自身碰撞
        for (const auto &segment : snake)
        {
            if (newHead.x == segment.x && newHead.y == segment.y)
            {
                takeDamage();
                if (gameOver) return;
                
                // 如果没有死亡，不移动到新位置
                return;
            }
        }

        // 检查与敌人的碰撞
        for (const auto &enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            if (newHead.x == enemy.position.x && newHead.y == enemy.position.y)
            {
                takeDamage();
                if (gameOver) return;
                
                // 如果没有死亡，不移动到新位置
                return;
            }
            
            // 检查敌人是否碰到蛇身
            for (const auto &segment : snake)
            {
                if (segment.x == enemy.position.x && segment.y == enemy.position.y)
                {
                    takeDamage();
                    if (gameOver) return;
                    
                    // 如果没有死亡，不移动到新位置
                    return;
                }
            }
        }

        // 添加新头部
        snake.insert(snake.begin(), newHead);

        // 调试信息
        char moveDebug[256];
        sprintf(moveDebug, "DEBUG: 蛇头移动到(%d,%d) 食物数量:%zu", 
                newHead.x, newHead.y, foods.size());
        logAIDecision(moveDebug);
        
        // 检查是否吃到任何食物
        bool ateFood = false;
        for (auto foodIt = foods.begin(); foodIt != foods.end();)
        {
            if (newHead.x == foodIt->x && newHead.y == foodIt->y)
            {
                logAIDecision("DEBUG: 🍽️ 吃到食物了!");
                score++;
                foodCount++;  // 增加食物计数
                foodIt = foods.erase(foodIt);  // 移除被吃掉的食物
                ateFood = true;
            }
            else
            {
                foodIt++;
            }
        }
        
        // 确保至少存在一个食物
        if (foods.empty())
        {
            generateFood();
        }
        
        if (!ateFood)
        {
            logAIDecision("DEBUG: 没吃到食物，移除尾部");
            // 如果没有吃到食物，移除尾部
            snake.pop_back();
        }
    }

    void handleInput(Tigr *screen)
    {
        // AI模式切换 (按A键)
        if (tigrKeyDown(screen, 'A'))
        {
            toggleAIMode();
        }
        
        // 手动控制模式
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
        // 清屏
        tigrClear(screen, tigrRGB(0x20, 0x20, 0x20));
        
        // 计算游戏区域偏移，使其居中
        int borderWidth = gridWidth * cellSize;
        int borderHeight = gridHeight * cellSize;
        int offsetX = (screen->w - borderWidth) / 2;
        int offsetY = (screen->h - borderHeight) / 2;
        
        // 绘制游戏区域边框
        tigrRect(screen, offsetX, offsetY, borderWidth, borderHeight, tigrRGB(0xFF, 0xFF, 0xFF));

        // 绘制蛇
        for (size_t i = 0; i < snake.size(); i++)
        {
            int x = offsetX + snake[i].x * cellSize;
            int y = offsetY + snake[i].y * cellSize;

            // 头部用不同颜色
            if (i == 0)
            {
                // 无敌时间内闪烁效果
                if (isHit && (invulnerableTimer % 10 < 5))
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0xFF, 0x00));  // 黄色闪烁
                }
                else
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0x00, 0xFF, 0x00));  // 正常绿色
                }
            }
            else
            {
                // 无敌时间内蛇身也闪烁
                if (isHit && (invulnerableTimer % 10 < 5))
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0x80, 0x00));  // 橙色闪烁
                }
                else
                {
                    tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0x00, 0x80, 0x00));  // 正常深绿色
                }
            }
        }

        // 绘制所有食物
        for (const auto &food : foods)
        // 绘制所有食物
        for (const auto &food : foods)
        {
            tigrFillRect(screen, offsetX + food.x * cellSize, offsetY + food.y * cellSize, cellSize, cellSize, tigrRGB(0xFF, 0x00, 0x00));
        }
        
        // 绘制敌人
        for (const auto &enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            int x = offsetX + enemy.position.x * cellSize;
            int y = offsetY + enemy.position.y * cellSize;
            
            // 根据巨大特性调整大小
            int drawSize = cellSize;
            if (enemy.hasTrait(TRAIT_GIANT))
            {
                drawSize = static_cast<int>(cellSize * enemy.size);
                x -= (drawSize - cellSize) / 2;  // 居中绘制
                y -= (drawSize - cellSize) / 2;
            }
            
            // 根据血量调整颜色
            int healthRatio = (enemy.currentHealth * 255) / enemy.maxHealth;
            TPixel enemyColor = tigrRGB(255, 165 - (165 - healthRatio), 0);
            
            // 用橙色到红色的渐变绘制敌人
            tigrFillRect(screen, x, y, drawSize, drawSize, enemyColor);
            
            // 根据特性绘制不同颜色的边框
            TPixel borderColor = tigrRGB(0xFF, 0x00, 0x00);  // 默认红色边框
            
            if (enemy.hasTrait(TRAIT_SPEED_BOOST))
                borderColor = tigrRGB(0x00, 0x00, 0xFF);  // 蓝色：速度提升
            else if (enemy.hasTrait(TRAIT_HEALTH_BOOST))
                borderColor = tigrRGB(0x00, 0xFF, 0x00);  // 绿色：血量提升
            else if (enemy.hasTrait(TRAIT_KNOCKBACK_RESIST))
                borderColor = tigrRGB(0x80, 0x00, 0x80);  // 紫色：击退抗性
            else if (enemy.hasTrait(TRAIT_SPLIT))
                borderColor = tigrRGB(0xFF, 0xFF, 0x00);  // 黄色：分裂
            
            tigrRect(screen, x, y, drawSize, drawSize, borderColor);
            
            // 如果有多个特性，绘制内部边框显示第二个特性
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
            
            // 显示血量条
            if (enemy.currentHealth < enemy.maxHealth)
            {
                int barWidth = drawSize - 2;
                int barHeight = 2;
                int barX = x + 1;
                int barY = y - 4;
                
                // 血量条背景
                tigrFillRect(screen, barX, barY, barWidth, barHeight, tigrRGB(0x80, 0x00, 0x00));
                
                // 当前血量
                int healthWidth = (enemy.currentHealth * barWidth) / enemy.maxHealth;
                tigrFillRect(screen, barX, barY, healthWidth, barHeight, tigrRGB(0x00, 0xFF, 0x00));
            }
        }
        
        // 绘制炮弹
        for (const auto &bullet : bullets)
        {
            int x = offsetX + static_cast<int>(bullet.positionX) * cellSize + cellSize / 2;
            int y = offsetY + static_cast<int>(bullet.positionY) * cellSize + cellSize / 2;
            
            // 用蓝色圆形绘制炮弹
            tigrFillCircle(screen, x, y, cellSize / 3, tigrRGB(0x00, 0x00, 0xFF));
            tigrCircle(screen, x, y, cellSize / 3, tigrRGB(0x00, 0x80, 0xFF));
        }

        // 显示分数和长度
        char scoreText[32];
        sprintf(scoreText, "Score: %d", score);
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 5, tigrRGB(0xFF, 0xFF, 0xFF), scoreText);
        
        char lengthText[32];
        sprintf(lengthText, "Length: %zu", snake.size());
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 20, tigrRGB(0xFF, 0xFF, 0xFF), lengthText);
        
        // 显示无敌时间
        if (invulnerableTimer > 0)
        {
            char invulnText[32];
            sprintf(invulnText, "Shield: %.1f", invulnerableTimer / 10.0f);
            tigrPrint(screen, tfont, offsetX + 5, offsetY + 35, tigrRGB(0xFF, 0xFF, 0x00), invulnText);
        }
        
        // 显示AI模式状态
        const char* modeText = aiMode ? "AI Mode: ON (Press A to toggle)" : "Manual Mode (Press A for AI)";
        TPixel modeColor = aiMode ? tigrRGB(0x00, 0xFF, 0x00) : tigrRGB(0xFF, 0xFF, 0xFF);
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 50, modeColor, modeText);

        // 游戏结束提示
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

// 定义静态成员变量
bool SnakeGame::firstRun = true;

int main(int argc, char *argv[])
{
    Tigr *screen = tigrWindow(640, 480, "Snake Game", TIGR_AUTO);
    SnakeGame game(640, 480);

    float accumulator = 0.0f;
    float baseUpdateInterval = 0.1f; // 基础更新间隔

    while (!tigrClosed(screen))
    {
        // tigrTime() 返回自上次调用以来的时间间隔
        float deltaTime = tigrTime();
        accumulator += deltaTime;

        // 处理输入
        game.handleInput(screen);

// 根据吃到的食物数量计算动态更新间隔（食物越多，移动越快）
        float moveSpeedBonus = (game.getFoodCount() / 10) * 0.01f; // 每吃10个食物，速度提升0.01秒
        float currentUpdateInterval = baseUpdateInterval - moveSpeedBonus;

        // 按动态间隔更新游戏状态
        if (accumulator >= currentUpdateInterval)
        {
            game.update();
            accumulator -= currentUpdateInterval;
        }

        // 绘制游戏
        game.draw(screen);

        // ESC键退出游戏
        if (tigrKeyDown(screen, TK_ESCAPE))
        {
            break;
        }

        tigrUpdate(screen);
    }

    // 游戏退出前刷新所有日志
    game.flushLogBuffer();

    tigrFree(screen);
    return 0;
}