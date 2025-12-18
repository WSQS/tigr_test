#ifndef TESTS_HPP
#define TESTS_HPP

// Basic functionality tests
void test_basic_functionality();

// Bullet class tests
void test_bullet_construction();
void test_bullet_update();
void test_bullet_lifetime();

// Enemy class tests
void test_enemy_construction();
void test_enemy_traits();
void test_enemy_movement();
void test_enemy_damage_and_knockback();
void test_enemy_find_nearest_target();

// GameUtils utility function tests
void test_manhattan_distance();
void test_euclidean_distance();
void test_direction_offset();

#endif // TESTS_HPP
