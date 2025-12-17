#include "test_framework.hpp"
#include "tests.hpp"

void test_basic_functionality()
{
    TestFramework framework;
    
    framework.run_test("Basic Math Test", [&]() {
        TEST_ASSERT_EQ(2 + 2, 4);
        TEST_ASSERT_EQ(5 * 3, 15);
    });
    
    framework.run_test("Basic Logic Test", [&]() {
        TEST_ASSERT(true);
        TEST_ASSERT(!false);
    });
    
    framework.print_summary();
}