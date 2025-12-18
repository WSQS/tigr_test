#include "test_framework.hpp"
#include "tests.hpp"
#include "../src/game_config.hpp"

void test_game_config_construction()
{
    TestFramework framework;
    
    framework.run_test("GameConfig Default Construction", [&]() {
        GameConfig config;
        
        TEST_ASSERT_EQ(config.cellSize, 10);
        TEST_ASSERT_EQ(config.initialSnakeLength, 3);
        TEST_ASSERT_EQ(config.bulletLifetime, 50);
        TEST_ASSERT_EQ(config.bulletSpeed, 1);
        TEST_ASSERT(config.isValid());
    });
    
    framework.run_test("GameConfig Custom Construction", [&]() {
        GameConfig config(20, 5);
        
        TEST_ASSERT_EQ(config.cellSize, 20);
        TEST_ASSERT_EQ(config.initialSnakeLength, 5);
        TEST_ASSERT(config.isValid());
    });
    
    framework.run_test("GameConfig Invalid Values", [&]() {
        GameConfig config;
        config.cellSize = -1;  // Invalid
        
        TEST_ASSERT(!config.isValid());
    });
    
    framework.print_summary();
}

void test_game_config_dynamic_calculations()
{
    TestFramework framework;
    
    framework.run_test("Dynamic Trait Count Calculation", [&]() {
        GameConfig config;
        
        // At 300 seconds, should have 1 trait max
        TEST_ASSERT_EQ(config.getDynamicTraitCount(300), 1);
        
        // At 600 seconds, should have 2 traits max
        TEST_ASSERT_EQ(config.getDynamicTraitCount(600), 2);
        
        // At 0 seconds, should have 0 traits max
        TEST_ASSERT_EQ(config.getDynamicTraitCount(0), 0);
    });
    
    framework.run_test("Dynamic Min Enemies Calculation", [&]() {
        GameConfig config;
        
        // At 600 seconds, should have 5 min enemies (4 + 1)
        TEST_ASSERT_EQ(config.getDynamicMinEnemies(600), 5);
        
        // At 1200 seconds, should have 6 min enemies (4 + 2)
        TEST_ASSERT_EQ(config.getDynamicMinEnemies(1200), 6);
        
        // At 0 seconds, should have 4 min enemies
        TEST_ASSERT_EQ(config.getDynamicMinEnemies(0), 4);
    });
    
    framework.run_test("Dynamic Spawn Interval Calculation", [&]() {
        GameConfig config;
        
        // At 300 seconds, interval should decrease by 1
        int interval1 = config.getDynamicSpawnInterval(300, 150);
        TEST_ASSERT_EQ(interval1, 149);
        
        // At 600 seconds, interval should decrease by 2
        int interval2 = config.getDynamicSpawnInterval(600, 150);
        TEST_ASSERT_EQ(interval2, 148);
        
        TEST_ASSERT(interval2 < interval1);
        
        // At very high time, should hit minimum limit
        // Calculation: 150 - (50000 / 300) = 150 - 166 = -16, so return 50
        int minInterval = config.getDynamicSpawnInterval(50000, 150);
        TEST_ASSERT_EQ(minInterval, config.minSpawnIntervalLimit);
        
        // Test that it doesn't go below minimum
        TEST_ASSERT(minInterval >= config.minSpawnIntervalLimit);
    });
    
    framework.run_test("Dynamic Attack Speed Calculation", [&]() {
        GameConfig config;
        
        // With 5 foods, should reduce cooldown
        float cooldown1 = config.getDynamicAttackSpeed(0);
        float cooldown2 = config.getDynamicAttackSpeed(5);
        
        TEST_ASSERT(cooldown2 < cooldown1);
        
        // With 10 foods, should reduce more
        float cooldown3 = config.getDynamicAttackSpeed(10);
        TEST_ASSERT(cooldown3 < cooldown2);
    });
    
    framework.print_summary();
}

void test_game_config_constants()
{
    TestFramework framework;
    
    framework.run_test("Direction Constants", [&]() {
        GameConfig config;
        
        // Check UP direction
        TEST_ASSERT_EQ(config.DIRECTIONS[0][0], 0);
        TEST_ASSERT_EQ(config.DIRECTIONS[0][1], -1);
        
        // Check DOWN direction
        TEST_ASSERT_EQ(config.DIRECTIONS[1][0], 0);
        TEST_ASSERT_EQ(config.DIRECTIONS[1][1], 1);
        
        // Check LEFT direction
        TEST_ASSERT_EQ(config.DIRECTIONS[2][0], -1);
        TEST_ASSERT_EQ(config.DIRECTIONS[2][1], 0);
        
        // Check RIGHT direction
        TEST_ASSERT_EQ(config.DIRECTIONS[3][0], 1);
        TEST_ASSERT_EQ(config.DIRECTIONS[3][1], 0);
    });
    
    framework.run_test("Split Offset Constants", [&]() {
        GameConfig config;
        
        // Check first offset (left)
        TEST_ASSERT_EQ(config.SPLIT_OFFSETS[0][0], -1);
        TEST_ASSERT_EQ(config.SPLIT_OFFSETS[0][1], 0);
        
        // Check diagonal offset
        TEST_ASSERT_EQ(config.SPLIT_OFFSETS[4][0], -1);
        TEST_ASSERT_EQ(config.SPLIT_OFFSETS[4][1], -1);
        
        // Check total count
        TEST_ASSERT_EQ(config.SPLIT_OFFSETS_COUNT, 8);
    });
    
    framework.print_summary();
}