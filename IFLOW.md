# TIGR Snake Game Project

## Project Overview

This is an enhanced snake game developed based on the TIGR (TIny GRaphics Library) graphics library, combining traditional snake gameplay with modern game elements. The project is developed using the C++17 standard and includes an intelligent AI system, combat mechanics, and a complex enemy system.

### Key Features

- **Core Snake Mechanics**: Movement, growth, collision detection, food collection
- **Combat System**: Automatic bullet attacks, enemy tracking, knockback mechanics
- **Enemy AI System**: Multiple traits (speed, health, knockback resistance, split, giant)
- **Intelligent AI Mode**: BFS pathfinding algorithm, threat assessment, automatic decision-making
- **Dynamic Difficulty**: Increasing enemy spawning and trait complexity over time
- **Detailed Logging System**: AI decision-making process recording and analysis
- **Complete Test Suite**: 40 test cases ensuring code quality (100% pass rate)
- **International Codebase**: Fully English comments and documentation
- **Optimized Performance**: Eliminated code redundancy and improved memory efficiency
- **Modular Architecture**: Clean separation of concerns with reusable utility functions

## Technical Architecture

### Core Technology Stack
- **Programming Language**: C++17
- **Graphics Library**: TIGR v3.2 (Lightweight C graphics library)
- **Build System**: Custom template metaprogramming build system (SOB)
- **Testing Framework**: Custom lightweight testing framework
- **Algorithms**: BFS pathfinding, Manhattan distance, threat assessment algorithms

### Project Structure
```
tigr_t/
├── src/
│   ├── game_types.hpp     # Game type definitions (Point, Direction, Bullet, etc.)
│   └── main.cpp           # Main program entry (contains SnakeGame and Enemy classes)
├── tests/
│   ├── test_framework.hpp # Lightweight testing framework
│   ├── tests.hpp          # Test function declarations
│   ├── simple_test.cpp    # Basic functionality test module
│   ├── bullet_test.cpp    # Bullet class test module
│   ├── enemy_test.cpp     # Enemy class test module
│   └── all_tests.cpp      # Unified test entry (main function)
├── thirdparty/tigr/       # TIGR graphics library
│   ├── include/tigr.h     # Graphics library header file
│   └── src/tigr.c         # Graphics library implementation
├── build/                 # Build output directory
│   ├── sob.o              # SOB build system object file
│   ├── src/main.o         # Main program object file
│   ├── tests/             # Test object file directory
│   │   ├── simple_test.o
│   │   ├── bullet_test.o
│   │   ├── enemy_test.o
│   │   └── all_tests.o
│   └── thirdparty/tigr/src/ti.o # TIGR library object file
├── sob.cpp                # Build system configuration
├── sob.hpp                # Build system implementation (template metaprogramming)
├── main                   # Main game executable
├── all_tests              # Unified test executable
├── sob                    # Build system executable
├── run_tests              # Old test suite (to be integrated)
├── ai_log.txt             # AI decision log file (generated at runtime)
├── IFLOW.md               # Project documentation (this file)
└── README.md              # Project description (not tracked)
```

## Build and Run

### Build Commands
```bash
# Compile all targets using the SOB build system
./sob

# Build results:
# - sob: Build system itself (bootstrapped)
# - main: Main game program
# - all_tests: Unified test suite (contains all test modules)
```

### Running the Game
```bash
# Run the main game
./main

# Run all tests
./all_tests
```

### Game Controls
- **Arrow Keys**: Control snake movement direction (manual mode)
- **A Key**: Toggle AI mode/manual mode
- **ESC Key**: Exit game

## Test System

### Test Architecture
Uses a **modular test architecture**:
- One executable (`all_tests`) contains all tests
- Test code distributed across multiple module files
- Each module tests independent functional units
- Tests directly use actual code from `src/game_types.hpp`

### Current Test Status
- **Total Tests**: 40 (100% passing)
- **Test Modules**:
  - `simple_test.cpp`: Basic functionality tests (2)
    - Mathematical operations and logical testing
  - `bullet_test.cpp`: Bullet class tests (10)
    - Constructor and direction calculation (5)
    - Position update and lifetime (3)
    - Lifecycle management (2)
  - `enemy_test.cpp`: Enemy class tests (16)
    - Constructor and trait system (6)
    - Movement logic and tracking (3)
    - Damage and knockback mechanics (5)
    - Target finding algorithm (2)
  - `game_utils_test.cpp`: Utility function tests (12)
    - Distance calculations (8)
    - Direction offset operations (4)

### Running Tests
```bash
# Compile and run all tests
./sob && ./all_tests

# Test output example:
# ========================================
#        Running All Unit Tests          
# ========================================
# 
# --- Basic Functionality Tests ---
# [PASS] Basic Math Test (0.000782ms)
# [PASS] Basic Logic Test (0.000251ms)
# 
# --- Bullet Class Tests ---
# [PASS] Bullet Basic Construction (0.001704ms)
# [PASS] Bullet Direction Calculation - Horizontal (0.000341ms)
# 
# --- Enemy Class Tests ---
# [PASS] Enemy Basic Construction (0.004218ms)
# [PASS] Speed Boost Trait (0.000961ms)
# ...
# Total tests: 28
# Passed: 28
# Success rate: 100%
```

### Test Framework Features
- Lightweight assertion macros (TEST_ASSERT, TEST_ASSERT_EQ)
- Automatic performance timing
- Modular test organization
- Unified test entry point
- Tests actual code (no code duplication)

### Code Optimization Achievements
- **Redundancy Elimination**: Removed duplicate code blocks and array definitions
- **Memory Efficiency**: Implemented static constants for shared data structures
- **Performance Optimization**: Optimized direction array usage with static member references
- **Clean Code Principles**: Applied DRY principle and eliminated code duplication
- **Maintainability**: Improved code structure for easier future development

## Development Conventions

### Development Workflow Principles

**Core Philosophy**: Every change should be **atomic**, **testable**, and **clean**

1. **Atomic Commits**
   - Each commit should do one thing only
   - Commit messages clearly describe the purpose and content of changes
   - Avoid mixing multiple unrelated modifications
   - Each commit should be in a compilable, runnable stable state
   - **Complex modifications can be split into multiple commits**
   - Split principles: by functional module, by logical hierarchy, by dependency relationship
   - Example: "Add enemy split trait" instead of "Fix bugs and add features"
   - Instance: Modular refactoring can be split into:
     1. Extract testable code
     2. Implement test architecture
     3. Update build system
     4. Synchronize documentation updates

2. **Test-Driven**
   - Run existing tests before modifying code to ensure baseline
   - Write test cases first when adding new features
   - Write reproduction tests first when fixing bugs
   - Run test suite to verify after each modification
   - Use `./sob && ./all_tests` for quick verification
   - Tests should test actual code, not duplicate code

3. **Clean Code**
   - Check for compilation warnings and errors before committing
   - Remove debug code and useless comments
   - Maintain consistent code formatting
   - Don't commit temporary files (build artifacts, logs, etc.)
   - Use .gitignore to manage non-version-controlled files
   - Regularly clean up dead code and outdated comments

4. **Documentation Sync**
   - **All code modifications must synchronize documentation updates**
   - Update IFLOW.md and README.md when modifying project structure
   - Document usage methods when adding new features
   - Update build instructions when modifying build configuration
   - Update test documentation and statistics when adding tests
   - Documentation should always reflect the current state of the code
   - Check if documentation needs updating before committing
   - Example: Add new test file → Update test system chapter

5. **Change Verification Process**
    ```bash
    # 1. Modify code
    # 2. Compile verification
    ./sob
    # 3. Run tests
    ./all_tests
    # 4. Check status
    git status
    git diff
    # 5. Atomic commit
    git add <specific-files>
    git commit -m "Clear, concise message"
    ```

6. **Internationalization Standards**
   - All code comments must be in English
   - All documentation must be in English
   - User-facing messages should be in English
   - Maintain consistent terminology across all files
   - Use clear, concise English for technical descriptions
### Code Style
- Use C++17 standard features
- Class names use PascalCase
- Functions and variables use camelCase
- Constants use UPPER_CASE
- Detailed English comments explaining complex logic

### Core Class Design

**Code Organization**:
- `src/game_types.hpp`: Testable basic types and class definitions
- `src/main.cpp`: Main game logic and complex class implementations

**Basic Types** (defined in `game_types.hpp`):
- `Point`: Simple coordinate point structure (x, y)
- `Direction`: Direction enumeration (UP, DOWN, LEFT, RIGHT)
- `EnemyTrait`: Enemy trait enumeration (5 traits)
- `Bullet`: Bullet class
  - Floating-point position and direction
  - Lifetime management
  - Collision detection
  - **Independently testable**
- `Enemy`: Enemy class
  - Trait system (speed, health, knockback resistance, split, giant)
  - AI tracking behavior
  - Damage and knockback mechanics
  - **Independently testable**

**Game Classes** (defined in `main.cpp`):
- `SnakeGame`: Main game class (core)
  - Snake movement and growth logic
  - Food generation and collision detection
  - Combat system (bullet firing and hits)
  - BFS pathfinding AI system
  - Enemy spawning and management
  - Game state and difficulty control
- `TestFramework`: Lightweight testing framework (tests/test_framework.hpp)

### AI System Design
- Uses BFS algorithm for pathfinding
- Multi-dimensional scoring system (safety, food reachability, threat assessment)
- Detailed decision logging to `ai_log.txt`
- Supports real-time mode switching

### Build System Features
- **Template Metaprogramming Based**: SOB (Sopho Build) system uses C++17 template metaprogramming
- **Cross-platform Support**: Automatically detects MSVC (_MSC_VER) and GCC (__GNUC__) compilers
- **Three Compilation Contexts**:
  - `SobCxxContext`: Build system itself (sob.cpp → sob)
  - `TigrCxxContext`: Main game (tigr.c + main.cpp → main)
  - `TestCxxContext`: Test suite (simple_test.cpp → simple_test)
- **Automatic Dependency Management**: Declares dependency relationships through Dependent tuples
- **Incremental Compilation**: Automatically generates object files to build/ directory
- **Platform-specific Configuration**:
  - Linux: `-lGLU -lGL -lX11` link flags
  - Windows: `opengl32.lib gdi32.lib` and subsystem settings

### Test Framework Features
- Easy-to-use assertion macros
- Performance testing tools
- Automated test reporting
- Supports benchmarking and regression testing

## Game Mechanics Detailed Description

### Enemy Trait System
- **TRAIT_SPEED_BOOST**: Speed boost (movement interval reduced by 40%)
- **TRAIT_HEALTH_BOOST**: Health boost (+3 health)
- **TRAIT_KNOCKBACK_RESIST**: Knockback resistance (20% reduction in knockback time)
- **TRAIT_SPLIT**: Split trait (spawns small enemies on death)
- **TRAIT_GIANT**: Giant (size +0.5x, health +2)

### Dynamic Difficulty System
- Increase enemy trait count limit every 30 seconds
- Increase minimum enemy count every minute
- Enemy spawn interval decreases over time
- Food collection affects snake movement speed and attack frequency

### AI Decision Algorithm
1. **Safety Assessment**: Check collisions and boundaries
2. **Pathfinding**: BFS algorithm finds shortest path to food
3. **Area Evaluation**: Calculate reachable area to avoid dead ends
4. **Threat Prediction**: Predict enemy positions to assess danger level
5. **Comprehensive Scoring**: Multi-dimensional weighted calculation of optimal direction

## Extensions and Modifications

### Adding New Enemy Traits
1. Add new trait to `EnemyTrait` enumeration
2. Implement trait logic in `applyTrait` method
3. Update trait assignment in enemy spawn system
4. Add corresponding test cases

### Customizing AI Behavior
1. Modify scoring algorithm in `makeAIDecision` method
2. Adjust weight parameters and evaluation dimensions (safety, food, threat)
3. Extend logging format (output to ai_log.txt)
4. Test AI decision quality and game performance

### Integrating Old Test Suite
1. Analyze test cases in run_tests
2. Add new build targets in sob.cpp
3. Create test source file structure
4. Configure compilation dependencies and link options
5. Verify all tests pass

## Project Status

### ✅ Completed
- Core game implementation (main.cpp + game_types.hpp)
- SOB build system fully configured (supports 3 targets: sob, main, all_tests)
- Complete English localization across all project files
  - All Chinese comments converted to English
  - Documentation fully translated
  - Consistent English-only codebase
- Modular test architecture (all_tests, 40/40 passing, 100%)
  - Complete test framework with performance timing
  - Tests actual code (no code duplication)
  - Modular organization (simple_test, bullet_test, enemy_test, game_utils_test)
- Code optimization and redundancy removal
  - Eliminated duplicate code blocks
  - Optimized direction array usage with static constants
  - Improved memory efficiency and maintainability
- Cross-platform compilation support (MSVC/GCC)
- Code refactoring (extracted game_types.hpp to improve testability)
- AI logging system (English output)
- Automatic exit on game over functionality

### ⚠️ Pending
- **File Cleanup**: 
  - Untracked files: README.md, ai_log.txt, build/, tests/, executables
  - Old executables: snake, run_tests, simple_test, bullet_test
  - Need to configure .gitignore

### 🔄 Next Steps Suggestions
1. Add tests for SnakeGame core logic
2. Consider extracting more testable components
3. Performance optimization and feature expansion
4. Integrate old test suite (run_tests)
5. Add more game mechanics tests

### 📊 Code Quality
- **Code Size**: main.cpp + game_types.hpp
- **Test Coverage**: 40 tests, 100% passing
  - Basic functionality: Complete coverage (2 tests)
  - Bullet class: Complete coverage (10 tests)
  - Enemy class: Complete coverage (16 tests)
  - GameUtils utility functions: Complete coverage (12 tests)
- **Build Status**: All targets compile successfully
- **Executables**: main, all_tests, sob
- **Code Refactoring**: Minimal extraction refactoring following SOLID principles
- **Internationalization**: 100% English codebase and documentation

### 📈 Recent Development History
- Complete Chinese to English conversion across all files
- Redundant code elimination and performance optimizations
- Comprehensive test suite expansion with GameUtils testing
- Code quality improvements following SOLID principles
- Enhanced build system with modular test architecture

This project demonstrates the complete workflow of modern C++ game development, including architectural design, AI algorithms, template metaprogramming build systems, and test-driven development, making it an excellent example for learning modern software engineering practices.