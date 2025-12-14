#include "tigr.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cfloat>

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
    
    Enemy(int x, int y, float moveSpeed)
    {
        position.x = x;
        position.y = y;
        speed = moveSpeed;
        moveCounter = 0;
        maxHealth = 3;  // 敌人需要被击中3次才能消灭
        currentHealth = maxHealth;
        knockbackCounter = 0;
    }
    
    void update(const Point& target)
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
            
            // 简单的追踪AI：向目标方向移动
            if (target.x < position.x) position.x--;
            else if (target.x > position.x) position.x++;
            
            if (target.y < position.y) position.y--;
            else if (target.y > position.y) position.y++;
        }
    }
    
    void takeDamage()
    {
        currentHealth--;
        knockbackCounter = 20;  // 被击中后的击退时间
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
    Point food;
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
    int maxEnemies;
    int gameTimer;  // 游戏运行时间，用于难度递增
    
    // 蛇的生命系统
    int invulnerableTimer;
    bool isHit;
    
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
            shootCooldown = 20;  // 发射冷却时间
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
        if (food.x == x && food.y == y) return true;
        
        return false;
    }
    
    // 生成新敌人
    void spawnEnemy()
    {
        // 计算当前存活的敌人数量
        int aliveEnemies = 0;
        for (const auto& enemy : enemies)
        {
            if (enemy.isAlive()) aliveEnemies++;
        }
        
        // 如果达到最大数量，不生成新敌人
        if (aliveEnemies >= maxEnemies) return;
        
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
                    // 随机敌人速度，随着游戏进行变快
                    float baseSpeed = 15.0f;
                    float speedVariation = (gameTimer / 600.0f);  // 每60秒速度增加1（更快）
                    float enemySpeed = baseSpeed - speedVariation;
                    if (enemySpeed < 3.0f) enemySpeed = 3.0f;  // 更低的最小速度限制
                    
                    enemies.push_back(Enemy(x, y, enemySpeed));
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
            invulnerableTimer = 50;  // 5秒无敌时间（假设每秒10次更新）
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
        // 在四个角落各放置一个敌人
        enemies.push_back(Enemy(2, 2, 15.0f));  // 左上角，移动较慢
        enemies.push_back(Enemy(gridWidth - 3, 2, 12.0f));  // 右上角
        enemies.push_back(Enemy(2, gridHeight - 3, 12.0f));  // 左下角
        enemies.push_back(Enemy(gridWidth - 3, gridHeight - 3, 10.0f));  // 右下角，移动较快
        
        // 初始化炮弹系统
        bullets.clear();
        shootCooldown = 0;
        
        // 初始化敌人生成系统
        enemySpawnTimer = 0;
        enemySpawnInterval = 150;  // 初始15秒生成一个新敌人（加快刷新频率）
        maxEnemies = 10;  // 增加最大敌人数量
        gameTimer = 0;
        
        // 初始化蛇的生命系统
        invulnerableTimer = 0;
        isHit = false;
    }

    void generateFood()
    {
        bool validPosition = false;
        while (!validPosition)
        {
            food.x = std::rand() % gridWidth;
            food.y = std::rand() % gridHeight;

            validPosition = true;
            for (const auto &segment : snake)
            {
                if (segment.x == food.x && segment.y == food.y)
                {
                    validPosition = false;
                    break;
                }
            }
        }
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
                enemy.update(snake[0]);  // 敌人追踪蛇头
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

        // 检查是否吃到食物
        if (newHead.x == food.x && newHead.y == food.y)
        {
            score++;
            generateFood();
        }
        else
        {
            // 如果没有吃到食物，移除尾部
            snake.pop_back();
        }
    }

    void handleInput(Tigr *screen)
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

    size_t getSnakeLength() const { return snake.size(); }
    
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

        // 绘制食物
        tigrFillRect(screen, offsetX + food.x * cellSize, offsetY + food.y * cellSize, cellSize, cellSize, tigrRGB(0xFF, 0x00, 0x00));
        
        // 绘制敌人
        for (const auto &enemy : enemies)
        {
            if (!enemy.isAlive()) continue;
            
            int x = offsetX + enemy.position.x * cellSize;
            int y = offsetY + enemy.position.y * cellSize;
            
            // 根据血量调整颜色
            int healthRatio = (enemy.currentHealth * 255) / enemy.maxHealth;
            TPixel enemyColor = tigrRGB(255, 165 - (165 - healthRatio), 0);
            
            // 用橙色到红色的渐变绘制敌人
            tigrFillRect(screen, x, y, cellSize, cellSize, enemyColor);
            tigrRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0x00, 0x00));
            
            // 显示血量条
            if (enemy.currentHealth < enemy.maxHealth)
            {
                int barWidth = cellSize - 2;
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

int main(int argc, char *argv[])
{
    Tigr *screen = tigrWindow(640, 480, "贪吃蛇游戏", TIGR_AUTO);
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

// 根据蛇的长度计算动态更新间隔（长度越长，移动越快）
        float speedBonus = (game.getSnakeLength() - 3) * 0.005f; // 每增加1节长度，速度提升0.005秒
        if (speedBonus > 0.08f) speedBonus = 0.08f; // 最大速度提升限制
        float currentUpdateInterval = baseUpdateInterval - speedBonus;
        if (currentUpdateInterval < 0.05f) currentUpdateInterval = 0.05f; // 最小更新间隔限制

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

    tigrFree(screen);
    return 0;
}