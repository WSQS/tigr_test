#include "test_framework.hpp"
#include "tests.hpp"
#include "../src/game_types.hpp"
#include <cmath>

void test_enemy_construction()
{
    TestFramework framework;
    
    framework.run_test("Enemy Basic Construction", [&]() {
        Enemy enemy(5, 5, {});
        
        TEST_ASSERT_EQ(enemy.position.x, 5);
        TEST_ASSERT_EQ(enemy.position.y, 5);
        TEST_ASSERT_EQ(enemy.speed, Enemy::BASE_SPEED);
        TEST_ASSERT_EQ(enemy.maxHealth, Enemy::BASE_HEALTH);
        TEST_ASSERT_EQ(enemy.currentHealth, Enemy::BASE_HEALTH);
        TEST_ASSERT_EQ(enemy.moveCounter, 0);
        TEST_ASSERT_EQ(enemy.knockbackCounter, 0);
        TEST_ASSERT_EQ(enemy.size, Enemy::BASE_SIZE);
        TEST_ASSERT(enemy.isAlive());
        TEST_ASSERT(enemy.traits.empty());
    });
    
    framework.run_test("Enemy With Traits", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_SPEED_BOOST, TRAIT_HEALTH_BOOST};
        Enemy enemy(0, 0, traits);
        
        TEST_ASSERT_EQ(enemy.traits.size(), 2);
        TEST_ASSERT(enemy.hasTrait(TRAIT_SPEED_BOOST));
        TEST_ASSERT(enemy.hasTrait(TRAIT_HEALTH_BOOST));
        TEST_ASSERT(!enemy.hasTrait(TRAIT_GIANT));
    });
    
    framework.print_summary();
}

void test_enemy_traits()
{
    TestFramework framework;
    
    framework.run_test("Speed Boost Trait", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_SPEED_BOOST};
        Enemy enemy(0, 0, traits);
        
        // 速度应该提升(移动间隔减少40%)
        float expectedSpeed = Enemy::BASE_SPEED * 0.6f;
        TEST_ASSERT_EQ(enemy.speed, expectedSpeed);
    });
    
    framework.run_test("Health Boost Trait", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_HEALTH_BOOST};
        Enemy enemy(0, 0, traits);
        
        // 血量应该+3
        int expectedHealth = Enemy::BASE_HEALTH + 3;
        TEST_ASSERT_EQ(enemy.maxHealth, expectedHealth);
        TEST_ASSERT_EQ(enemy.currentHealth, expectedHealth);
    });
    
    framework.run_test("Giant Trait", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_GIANT};
        Enemy enemy(0, 0, traits);
        
        // 体型+0.5，血量+2
        float expectedSize = Enemy::BASE_SIZE + 0.5f;
        int expectedHealth = Enemy::BASE_HEALTH + 2;
        TEST_ASSERT_EQ(enemy.size, expectedSize);
        TEST_ASSERT_EQ(enemy.maxHealth, expectedHealth);
    });
    
    framework.run_test("Multiple Traits", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_HEALTH_BOOST, TRAIT_GIANT, TRAIT_HEALTH_BOOST};
        Enemy enemy(0, 0, traits);
        
        // 两个HEALTH_BOOST应该叠加
        int expectedHealth = Enemy::BASE_HEALTH + 3 + 3 + 2; // 3+3+2
        float expectedSize = Enemy::BASE_SIZE + 0.5f;
        TEST_ASSERT_EQ(enemy.maxHealth, expectedHealth);
        TEST_ASSERT_EQ(enemy.size, expectedSize);
    });
    
    framework.print_summary();
}

void test_enemy_movement()
{
    TestFramework framework;
    
    framework.run_test("Enemy Movement Towards Target", [&]() {
        Enemy enemy(5, 5, {});
        std::vector<Point> snake = {{10, 5}}; // 蛇在右边
        
        // 更新足够多次让敌人移动一次
        for (int i = 0; i < static_cast<int>(Enemy::BASE_SPEED); i++) {
            enemy.update(snake);
        }
        
        // 敌人应该向右移动一步
        TEST_ASSERT_EQ(enemy.position.x, 6);
        TEST_ASSERT_EQ(enemy.position.y, 5);
    });
    
    framework.run_test("Enemy Movement Diagonal", [&]() {
        Enemy enemy(0, 0, {});
        std::vector<Point> snake = {{10, 10}}; // 蛇在右下角
        
        // 更新足够多次让敌人移动一次
        for (int i = 0; i < static_cast<int>(Enemy::BASE_SPEED); i++) {
            enemy.update(snake);
        }
        
        // 敌人应该向右下移动一步
        TEST_ASSERT_EQ(enemy.position.x, 1);
        TEST_ASSERT_EQ(enemy.position.y, 1);
    });
    
    framework.run_test("Enemy No Movement During Knockback", [&]() {
        Enemy enemy(5, 5, {});
        enemy.knockbackCounter = 10;
        std::vector<Point> snake = {{10, 5}};
        
        Point originalPos = enemy.position;
        enemy.update(snake);
        
        // 击退期间不应该移动
        TEST_ASSERT_EQ(enemy.position.x, originalPos.x);
        TEST_ASSERT_EQ(enemy.position.y, originalPos.y);
        TEST_ASSERT_EQ(enemy.knockbackCounter, 9);
    });
    
    framework.print_summary();
}

void test_enemy_damage_and_knockback()
{
    TestFramework framework;
    
    framework.run_test("Enemy Take Damage", [&]() {
        Enemy enemy(0, 0, {});
        int initialHealth = enemy.currentHealth;
        
        enemy.takeDamage();
        
        TEST_ASSERT_EQ(enemy.currentHealth, initialHealth - 1);
        TEST_ASSERT(enemy.isAlive());
        
        // 扣除所有血量
        for (int i = 1; i < enemy.maxHealth; i++) {
            enemy.takeDamage();
        }
        TEST_ASSERT_EQ(enemy.currentHealth, 0);
        TEST_ASSERT(!enemy.isAlive());
    });
    
    framework.run_test("Enemy Knockback", [&]() {
        Enemy enemy(5, 5, {});
        Point from = {7, 5}; // 从右边击退
        
        enemy.knockback(from, 20, 20);
        
        // 应该向左移动
        TEST_ASSERT_EQ(enemy.position.x, 3);
        TEST_ASSERT_EQ(enemy.position.y, 5);
    });
    
    framework.run_test("Enemy Knockback Boundaries", [&]() {
        Enemy enemy(1, 1, {});
        Point from = {5, 5}; // 从右下击退
        
        enemy.knockback(from, 5, 5);
        
        // 不应该超出边界
        TEST_ASSERT(enemy.position.x >= 0);
        TEST_ASSERT(enemy.position.x < 5);
        TEST_ASSERT(enemy.position.y >= 0);
        TEST_ASSERT(enemy.position.y < 5);
    });
    
    framework.run_test("Knockback Resistance Trait", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_KNOCKBACK_RESIST};
        Enemy enemy(0, 0, traits);
        
        enemy.takeDamage();
        
        // 有击退抗性，击退时间应该减少20%
        TEST_ASSERT(enemy.knockbackCounter > 0);
        TEST_ASSERT(enemy.knockbackCounter < Enemy::BASE_KNOCKBACK);
    });
    
    framework.run_test("Multiple Knockback Resistance", [&]() {
        std::vector<EnemyTrait> traits = {TRAIT_KNOCKBACK_RESIST, TRAIT_KNOCKBACK_RESIST};
        Enemy enemy(0, 0, traits);
        
        enemy.takeDamage();
        
        // 两个击退抗性，击退时间应该减少36% (0.8 * 0.8)
        float expectedKnockback = Enemy::BASE_KNOCKBACK * 0.8f * 0.8f;
        TEST_ASSERT_EQ(enemy.knockbackCounter, static_cast<int>(expectedKnockback));
    });
    
    framework.print_summary();
}

void test_enemy_find_nearest_target()
{
    TestFramework framework;
    
    framework.run_test("Find Nearest Snake Segment", [&]() {
        Enemy enemy(0, 0, {});
        std::vector<Point> snake = {{5, 5}, {10, 10}, {3, 3}};
        
        Point nearest = enemy.findNearestSnakeSegment(snake);
        
        // 最近的应该是(3,3)
        TEST_ASSERT_EQ(nearest.x, 3);
        TEST_ASSERT_EQ(nearest.y, 3);
    });
    
    framework.run_test("Find Nearest With Empty Snake", [&]() {
        Enemy enemy(5, 5, {});
        std::vector<Point> snake = {};
        
        Point nearest = enemy.findNearestSnakeSegment(snake);
        
        // 空蛇应该返回(0,0)
        TEST_ASSERT_EQ(nearest.x, 0);
        TEST_ASSERT_EQ(nearest.y, 0);
    });
    
    framework.print_summary();
}