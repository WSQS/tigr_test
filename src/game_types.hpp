#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include <cmath>
#include <cfloat>
#include <vector>
#include <algorithm>

// 基础坐标点结构
struct Point
{
    int x, y;
};

// 方向枚举
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// 敌人特性枚举
enum EnemyTrait
{
    TRAIT_SPEED_BOOST,      // 速度提升
    TRAIT_HEALTH_BOOST,     // 血量提升
    TRAIT_KNOCKBACK_RESIST, // 击退抗性
    TRAIT_SPLIT,            // 分裂（死亡时分裂成小敌人）
    TRAIT_GIANT,            // 巨大（体型更大）
    TRAIT_COUNT             // 特性总数
};

// 炮弹类
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

// 敌人类
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

#endif // GAME_TYPES_HPP
