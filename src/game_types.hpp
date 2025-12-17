#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include <cmath>

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

#endif // GAME_TYPES_HPP
