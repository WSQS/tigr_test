#include "test_framework.hpp"
#include "tests.hpp"
#include "../src/game_types.hpp"
#include <cmath>

void test_bullet_construction()
{
    TestFramework framework;
    
    framework.run_test("Bullet Basic Construction", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 1.0f);
        
        TEST_ASSERT_EQ(bullet.positionX, 0.0f);
        TEST_ASSERT_EQ(bullet.positionY, 0.0f);
        TEST_ASSERT_EQ(bullet.speed, 1.0f);
        TEST_ASSERT_EQ(bullet.lifetime, 50);
        TEST_ASSERT(bullet.isAlive());
    });
    
    framework.run_test("Bullet Direction Calculation - Horizontal", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 1.0f);
        
        // Moving right, directionX should be 1, directionY should be 0
        TEST_ASSERT_EQ(bullet.directionX, 1.0f);
        TEST_ASSERT_EQ(bullet.directionY, 0.0f);
    });
    
    framework.run_test("Bullet Direction Calculation - Vertical", [&]() {
        Point start = {0, 0};
        Point target = {0, 10};
        Bullet bullet(start, target, 1.0f);
        
        // Moving down, directionX should be 0, directionY should be 1
        TEST_ASSERT_EQ(bullet.directionX, 0.0f);
        TEST_ASSERT_EQ(bullet.directionY, 1.0f);
    });
    
    framework.run_test("Bullet Direction Calculation - Diagonal", [&]() {
        Point start = {0, 0};
        Point target = {10, 10};
        Bullet bullet(start, target, 1.0f);
        
        // Diagonal movement, direction vector should be normalized
        float expected = 1.0f / sqrt(2.0f);
        TEST_ASSERT(fabs(bullet.directionX - expected) < 0.001f);
        TEST_ASSERT(fabs(bullet.directionY - expected) < 0.001f);
    });
    
    framework.run_test("Bullet Zero Distance Target", [&]() {
        Point start = {5, 5};
        Point target = {5, 5};  // Same position
        Bullet bullet(start, target, 1.0f);
        
        // Zero distance should default to upward
        TEST_ASSERT_EQ(bullet.directionX, 0.0f);
        TEST_ASSERT_EQ(bullet.directionY, -1.0f);
    });
    
    framework.print_summary();
}

void test_bullet_update()
{
    TestFramework framework;
    
    framework.run_test("Bullet Position Update", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 2.0f);
        
        float initialX = bullet.positionX;
        bullet.update();
        
        // Move 2.0 units to the right
        TEST_ASSERT_EQ(bullet.positionX, initialX + 2.0f);
        TEST_ASSERT_EQ(bullet.positionY, 0.0f);
    });
    
    framework.run_test("Bullet Lifetime Decrease", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 1.0f);
        
        int initialLifetime = bullet.lifetime;
        bullet.update();
        
        TEST_ASSERT_EQ(bullet.lifetime, initialLifetime - 1);
    });
    
    framework.run_test("Bullet Multiple Updates", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 1.0f);
        
        // Update 5 times
        for (int i = 0; i < 5; i++) {
            bullet.update();
        }
        
        TEST_ASSERT_EQ(bullet.positionX, 5.0f);
        TEST_ASSERT_EQ(bullet.lifetime, 45);
    });
    
    framework.print_summary();
}

void test_bullet_lifetime()
{
    TestFramework framework;
    
    framework.run_test("Bullet Alive Status", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 1.0f);
        
        TEST_ASSERT(bullet.isAlive());
        
        // Update 49 times, should still be alive
        for (int i = 0; i < 49; i++) {
            bullet.update();
        }
        TEST_ASSERT(bullet.isAlive());
        TEST_ASSERT_EQ(bullet.lifetime, 1);
        
        // Update 1 more time, should die
        bullet.update();
        TEST_ASSERT(!bullet.isAlive());
        TEST_ASSERT_EQ(bullet.lifetime, 0);
    });
    
    framework.run_test("Bullet GetPosition", [&]() {
        Point start = {0, 0};
        Point target = {10, 0};
        Bullet bullet(start, target, 1.5f);
        
        bullet.update();
        Point pos = bullet.getPosition();
        
        // Position should be 1.5, converted to integer should be 1
        TEST_ASSERT_EQ(pos.x, 1);
        TEST_ASSERT_EQ(pos.y, 0);
    });
    
    framework.print_summary();
}


