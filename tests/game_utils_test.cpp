#include "tests.hpp"
#include "test_framework.hpp"
#include "../src/game_types.hpp"

void test_manhattan_distance()
{
    TestFramework framework;
    
    framework.run_test("Manhattan Distance - Origin to (3,4)", [&]() {
        TEST_ASSERT_EQ(GameUtils::manhattanDistance(Point{0, 0}, Point{3, 4}), 7);
    });
    
    framework.run_test("Manhattan Distance - Same Point", [&]() {
        TEST_ASSERT_EQ(GameUtils::manhattanDistance(Point{5, 5}, Point{5, 5}), 0);
    });
    
    framework.run_test("Manhattan Distance - Negative Coordinates", [&]() {
        TEST_ASSERT_EQ(GameUtils::manhattanDistance(Point{-2, -3}, Point{2, 3}), 10);
    });
    
    framework.run_test("Manhattan Distance - Large Distance", [&]() {
        TEST_ASSERT_EQ(GameUtils::manhattanDistance(Point{10, 0}, Point{0, 10}), 20);
    });
    
    framework.print_summary();
}

void test_euclidean_distance()
{
    TestFramework framework;
    
    framework.run_test("Euclidean Distance - 3-4-5 Triangle", [&]() {
        float dist = GameUtils::euclideanDistance(Point{0, 0}, Point{3, 4});
        TEST_ASSERT(abs(dist - 5.0f) < 0.001f);
    });
    
    framework.run_test("Euclidean Distance - Same Point", [&]() {
        float dist = GameUtils::euclideanDistance(Point{5, 5}, Point{5, 5});
        TEST_ASSERT(abs(dist - 0.0f) < 0.001f);
    });
    
    framework.run_test("Euclidean Distance - Float Version", [&]() {
        float dist = GameUtils::euclideanDistance(0.0f, 0.0f, 3.0f, 4.0f);
        TEST_ASSERT(abs(dist - 5.0f) < 0.001f);
    });
    
    framework.run_test("Euclidean Distance - Float Coordinates", [&]() {
        float dist = GameUtils::euclideanDistance(1.5f, 2.5f, 4.5f, 6.5f);
        TEST_ASSERT(abs(dist - 5.0f) < 0.001f);
    });
    
    framework.print_summary();
}

void test_direction_offset()
{
    TestFramework framework;
    
    framework.run_test("Direction Offset - UP", [&]() {
        Point up = GameUtils::getDirectionOffset(UP);
        TEST_ASSERT_EQ(up.x, 0);
        TEST_ASSERT_EQ(up.y, -1);
    });
    
    framework.run_test("Direction Offset - DOWN", [&]() {
        Point down = GameUtils::getDirectionOffset(DOWN);
        TEST_ASSERT_EQ(down.x, 0);
        TEST_ASSERT_EQ(down.y, 1);
    });
    
    framework.run_test("Direction Offset - LEFT", [&]() {
        Point left = GameUtils::getDirectionOffset(LEFT);
        TEST_ASSERT_EQ(left.x, -1);
        TEST_ASSERT_EQ(left.y, 0);
    });
    
    framework.run_test("Direction Offset - RIGHT", [&]() {
        Point right = GameUtils::getDirectionOffset(RIGHT);
        TEST_ASSERT_EQ(right.x, 1);
        TEST_ASSERT_EQ(right.y, 0);
    });
    
    framework.print_summary();
}
