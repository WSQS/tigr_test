#include "tests.hpp"
#include <iostream>

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "       Running All Unit Tests          " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- Basic Functionality Tests ---" << std::endl;
    test_basic_functionality();
    std::cout << std::endl;
    
    std::cout << "--- Bullet Class Tests ---" << std::endl;
    test_bullet_construction();
    test_bullet_update();
    test_bullet_lifetime();
    std::cout << std::endl;
    
    std::cout << "--- Enemy Class Tests ---" << std::endl;
    test_enemy_construction();
    test_enemy_traits();
    test_enemy_movement();
    test_enemy_damage_and_knockback();
    test_enemy_find_nearest_target();
    std::cout << std::endl;
    
    std::cout << "--- GameUtils Tests ---" << std::endl;
    test_manhattan_distance();
    test_euclidean_distance();
    test_direction_offset();
    std::cout << std::endl;
    
    std::cout << "--- GameConfig Tests ---" << std::endl;
    test_game_config_construction();
    test_game_config_dynamic_calculations();
    test_game_config_constants();
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "       All Tests Completed              " << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}