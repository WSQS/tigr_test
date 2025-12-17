#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <chrono>

// 简单的测试框架
class TestFramework {
private:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
        double duration_ms;
    };
    
    std::vector<TestResult> results;
    int total_tests = 0;
    int passed_tests = 0;
    
public:
    void run_test(const std::string& test_name, std::function<void()> test_func) {
        total_tests++;
        TestResult result;
        result.name = test_name;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            test_func();
            result.passed = true;
            result.message = "PASSED";
            passed_tests++;
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("FAILED: ") + e.what();
        } catch (...) {
            result.passed = false;
            result.message = "FAILED: Unknown exception";
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
        
        results.push_back(result);
        
        std::cout << "[" << (result.passed ? "PASS" : "FAIL") << "] " 
                  << test_name << " (" << result.duration_ms << "ms)" << std::endl;
        
        if (!result.passed) {
            std::cout << "  " << result.message << std::endl;
        }
    }
    
    void assert_true(bool condition, const std::string& message = "Assertion failed") {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }
    
    void assert_equals(int expected, int actual, const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error(message + " Expected: " + std::to_string(expected) + 
                                   ", Actual: " + std::to_string(actual));
        }
    }
    
    void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << total_tests << std::endl;
        std::cout << "Passed: " << passed_tests << std::endl;
        std::cout << "Failed: " << (total_tests - passed_tests) << std::endl;
        std::cout << "Success rate: " << (total_tests > 0 ? (passed_tests * 100.0 / total_tests) : 0) << "%" << std::endl;
        
        double total_time = 0;
        for (const auto& result : results) {
            total_time += result.duration_ms;
        }
        std::cout << "Total time: " << total_time << "ms" << std::endl;
    }
    
    bool all_passed() const {
        return passed_tests == total_tests;
    }
};

// 测试辅助宏
#define TEST_ASSERT(condition) framework.assert_true(condition, #condition)
#define TEST_ASSERT_EQ(expected, actual) framework.assert_equals(expected, actual)