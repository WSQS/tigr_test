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
    
    std::cout << "========================================" << std::endl;
    std::cout << "       All Tests Completed              " << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}