#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include <cmath>
#include <cfloat>
#include <vector>
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

namespace GameUtils
{
    inline int manhattanDistance(const Point& a, const Point& b)
    {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }
    
    inline float euclideanDistance(const Point& a, const Point& b)
    {
        float dx = static_cast<float>(a.x - b.x);
        float dy = static_cast<float>(a.y - b.y);
        return sqrt(dx * dx + dy * dy);
    }
    
    inline float euclideanDistance(float x1, float y1, float x2, float y2)
    {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return sqrt(dx * dx + dy * dy);
    }
    
    inline Point getDirectionOffset(Direction dir)
    {
        switch (dir)
        {
            case UP:    return {0, -1};
            case DOWN:  return {0, 1};
            case LEFT:  return {-1, 0};
            case RIGHT: return {1, 0};
            default:    return {0, 0};
        }
    }
}

enum EnemyTrait
{
    TRAIT_SPEED_BOOST,
    TRAIT_HEALTH_BOOST,
    TRAIT_KNOCKBACK_RESIST,
    TRAIT_SPLIT,
    TRAIT_GIANT,
    TRAIT_COUNT
};

class Bullet
{
public:
    float positionX, positionY;
    float directionX, directionY;
    float speed;
    int lifetime;
    
    Bullet(Point startPos, Point targetPos, float bulletSpeed)
    {
        positionX = startPos.x;
        positionY = startPos.y;
        speed = bulletSpeed;
        lifetime = 50;
        
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
            directionY = -1;
        }
    }
    
    void update()
    {
        lifetime--;
        positionX += directionX * speed;
        positionY += directionY * speed;
    }
    
    bool isAlive() const { return lifetime > 0; }
    
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
    float size;
    
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
        
        speed = BASE_SPEED;
        maxHealth = BASE_HEALTH;
        size = BASE_SIZE;
        
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
                speed *= 0.6f;
                break;
            case TRAIT_HEALTH_BOOST:
                maxHealth += 3;
                break;
            case TRAIT_KNOCKBACK_RESIST:
                break;
            case TRAIT_SPLIT:
                break;
            case TRAIT_GIANT:
                size += 0.5f;
                maxHealth += 2;
                break;
            default:
                break;
        }
    }
    
    void update(const std::vector<Point>& snake)
    {
        if (knockbackCounter > 0)
        {
            knockbackCounter--;
            return;
        }
        
        moveCounter++;
        if (moveCounter >= speed)
        {
            moveCounter = 0;
            Point nearestTarget = findNearestSnakeSegment(snake);
            
            if (nearestTarget.x < position.x) position.x--;
            else if (nearestTarget.x > position.x) position.x++;
            
            if (nearestTarget.y < position.y) position.y--;
            else if (nearestTarget.y > position.y) position.y++;
        }
    }
    
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
        
        float knockback = BASE_KNOCKBACK;
        int resistCount = std::count(traits.begin(), traits.end(), TRAIT_KNOCKBACK_RESIST);
        for (int i = 0; i < resistCount; i++)
        {
            knockback *= 0.8f;
        }
        knockbackCounter = static_cast<int>(knockback);
    }
    
    void knockback(const Point& from, int maxWidth, int maxHeight)
    {
        int dx = position.x - from.x;
        int dy = position.y - from.y;
        
        if (dx != 0) position.x += (dx > 0 ? 2 : -2);
        if (dy != 0) position.y += (dy > 0 ? 2 : -2);
        
        if (position.x < 0) position.x = 0;
        if (position.x >= maxWidth) position.x = maxWidth - 1;
        if (position.y < 0) position.y = 0;
        if (position.y >= maxHeight) position.y = maxHeight - 1;
    }
    
    bool isAlive() const { return currentHealth > 0; }
};

#endif // GAME_TYPES_HPP
