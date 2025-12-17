# TIGR 贪吃蛇游戏项目

## 项目概述

这是一个基于TIGR(TIny GRaphics Library)图形库开发的增强版贪吃蛇游戏，结合了传统贪吃蛇玩法与现代游戏元素。项目使用C++17标准开发，包含智能AI系统、战斗机制和复杂的敌人系统。

### 主要特性

- **核心贪吃蛇机制**: 移动、成长、碰撞检测、食物收集
- **战斗系统**: 自动炮弹攻击、敌人追踪、击退机制
- **敌人AI系统**: 多种特性(速度、血量、击退抗性、分裂、巨大化)
- **智能AI模式**: BFS寻路算法、威胁评估、自动决策
- **动态难度**: 随时间递增的敌人生成和特性复杂度
- **详细日志系统**: AI决策过程记录和分析
- **完整测试套件**: 18个测试用例，确保代码质量

## 技术架构

### 核心技术栈
- **编程语言**: C++17
- **图形库**: TIGR v3.2 (轻量级C图形库)
- **构建系统**: 自定义模板元编程构建系统(SOB)
- **测试框架**: 自研轻量级测试框架
- **算法**: BFS寻路、曼哈顿距离、威胁评估算法

### 项目结构
```
tigr_t/
├── src/
│   ├── game_types.hpp     # 游戏类型定义(Point, Direction, Bullet等)
│   └── main.cpp           # 主程序入口(包含SnakeGame和Enemy类)
├── tests/
│   ├── test_framework.hpp # 轻量级测试框架
│   ├── tests.hpp          # 测试函数声明
│   ├── simple_test.cpp    # 基础功能测试模块
│   ├── bullet_test.cpp    # Bullet类测试模块
│   └── all_tests.cpp      # 统一测试入口(main函数)
├── thirdparty/tigr/       # TIGR图形库
│   ├── include/tigr.h     # 图形库头文件
│   └── src/tigr.c         # 图形库实现
├── build/                 # 构建输出目录
│   ├── sob.o              # SOB构建系统目标文件
│   ├── src/main.o         # 主程序目标文件
│   ├── tests/             # 测试目标文件目录
│   │   ├── simple_test.o
│   │   ├── bullet_test.o
│   │   └── all_tests.o
│   └── thirdparty/tigr/src/ti.o # TIGR库目标文件
├── sob.cpp                # 构建系统配置
├── sob.hpp                # 构建系统实现(模板元编程)
├── main                   # 主游戏可执行文件
├── all_tests              # 统一测试可执行文件
├── sob                    # 构建系统可执行文件
├── run_tests              # 旧测试套件(待整合)
├── ai_log.txt             # AI决策日志文件(运行时生成)
├── IFLOW.md               # 项目文档(本文件)
└── README.md              # 项目说明(未追踪)
```

## 构建和运行

### 构建命令
```bash
# 使用SOB构建系统编译所有目标
./sob

# 构建结果：
# - sob: 构建系统本身(自举)
# - main: 主游戏程序
# - all_tests: 统一测试套件(包含所有测试模块)
```

### 运行游戏
```bash
# 运行主游戏
./main

# 运行所有测试
./all_tests
```

### 游戏控制
- **方向键**: 控制蛇的移动方向(手动模式)
- **A键**: 切换AI模式/手动模式
- **ESC键**: 退出游戏

## 测试系统

### 测试架构
采用**模块化测试架构**：
- 一个可执行文件(`all_tests`)包含所有测试
- 测试代码分布在多个模块文件中
- 每个模块测试独立的功能单元
- 测试直接使用`src/game_types.hpp`中的实际代码

### 当前测试状态
- **总测试数**: 12个 (100%通过)
- **测试模块**:
  - `simple_test.cpp`: 基础功能测试 (2个)
  - `bullet_test.cpp`: Bullet类测试 (10个)
    - 构造函数和方向计算 (5个)
    - 位置更新和生命值 (3个)
    - 生命周期管理 (2个)

### 运行测试
```bash
# 编译并运行所有测试
./sob && ./all_tests

# 测试输出示例：
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
# ...
# Total tests: 12
# Passed: 12
# Success rate: 100%
```

### 测试框架特性
- 轻量级断言宏(TEST_ASSERT, TEST_ASSERT_EQ)
- 自动性能计时
- 模块化测试组织
- 统一测试入口
- 测试实际代码(无代码复制)

## 开发约定

### 开发工作流原则

**核心理念**: 每次修改都应该做到**原子化**、**可测试**、**干净**

1. **原子化提交 (Atomic Commits)**
   - 每次提交只做一件事情
   - 提交信息清晰描述改动的目的和内容
   - 避免混合多个不相关的修改
   - 每个提交都应该是可编译、可运行的稳定状态
   - **复杂修改可以拆分成多次提交**
   - 拆分原则: 按功能模块、按逻辑层次、按依赖关系
   - 示例: "Add enemy split trait" 而不是 "Fix bugs and add features"
   - 实例: 模块化重构可以拆分为:
     1. 提取可测试代码
     2. 实现测试架构
     3. 更新构建系统
     4. 同步文档更新

2. **测试驱动 (Test-Driven)**
   - 修改代码前先运行现有测试确保基线
   - 添加新功能时先编写测试用例
   - 修复bug时先写复现测试
   - 每次修改后运行测试套件验证
   - 使用`./sob && ./all_tests`快速验证
   - 测试应该测试实际代码，不复制代码

3. **代码整洁 (Clean Code)**
   - 提交前检查编译警告和错误
   - 移除调试代码和无用注释
   - 保持代码格式一致
   - 不提交临时文件(构建产物、日志等)
   - 使用.gitignore管理非版本控制文件
   - 定期清理死代码和过时注释

4. **文档同步 (Documentation Sync)**
   - **所有代码修改都必须同步更新文档**
   - 修改项目结构时更新IFLOW.md和README.md
   - 添加新功能时在文档中说明使用方法
   - 修改构建配置时更新构建说明
   - 添加测试时更新测试文档和统计
   - 文档应该始终反映代码的当前状态
   - 提交前检查文档是否需要更新
   - 示例: 添加新测试文件 → 更新测试系统章节

5. **变更验证流程**
   ```bash
   # 1. 修改代码
   # 2. 编译验证
   ./sob
   # 3. 运行测试
   ./all_tests
   # 4. 检查状态
   git status
   git diff
   # 5. 原子化提交
   git add <specific-files>
   git commit -m "Clear, concise message"
   ```

### 代码风格
- 使用C++17标准特性
- 类名使用PascalCase
- 函数和变量使用camelCase
- 常量使用UPPER_CASE
- 详细的中文注释说明复杂逻辑

### 核心类设计

**代码组织**:
- `src/game_types.hpp`: 可测试的基础类型和类定义
- `src/main.cpp`: 主游戏逻辑和复杂类实现

**基础类型** (定义在`game_types.hpp`):
- `Point`: 简单的坐标点结构(x, y)
- `Direction`: 方向枚举(UP, DOWN, LEFT, RIGHT)
- `EnemyTrait`: 敌人特性枚举(5种特性)
- `Bullet`: 炮弹类
  - 浮点数位置和方向
  - 生存时间管理
  - 碰撞检测
  - **可独立测试**

**游戏类** (定义在`main.cpp`):
- `Enemy`: 敌人类
  - 特性系统(速度、血量、击退抗性等)
  - AI追踪行为
  - 击退和分裂机制
- `SnakeGame`: 主游戏类(核心)
  - 蛇的移动和成长逻辑
  - 食物生成和碰撞检测
  - 战斗系统(炮弹发射和命中)
  - BFS寻路AI系统
  - 敌人生成和管理
  - 游戏状态和难度控制
- `TestFramework`: 轻量级测试框架(tests/test_framework.hpp)

### AI系统设计
- 使用BFS算法进行路径寻找
- 多维度评分系统(安全性、食物可达性、威胁评估)
- 详细的决策日志记录到`ai_log.txt`
- 支持实时模式切换

### 构建系统特点
- **基于模板元编程**: SOB(Sopho Build)系统使用C++17模板元编程
- **跨平台支持**: 自动检测MSVC(_MSC_VER)和GCC(__GNUC__)编译器
- **三个编译上下文**:
  - `SobCxxContext`: 构建系统自身(sob.cpp → sob)
  - `TigrCxxContext`: 主游戏(tigr.c + main.cpp → main)
  - `TestCxxContext`: 测试套件(simple_test.cpp → simple_test)
- **自动依赖管理**: 通过Dependent元组声明依赖关系
- **增量编译**: 自动生成目标文件到build/目录
- **平台特定配置**:
  - Linux: `-lGLU -lGL -lX11`链接标志
  - Windows: `opengl32.lib gdi32.lib`和子系统设置

### 测试框架特点
- 简单易用的断言宏
- 性能测试工具
- 自动化测试报告
- 支持基准测试和回归测试

## 游戏机制详细说明

### 敌人特性系统
- **TRAIT_SPEED_BOOST**: 速度提升(移动间隔减少40%)
- **TRAIT_HEALTH_BOOST**: 血量提升(+3血量)
- **TRAIT_KNOCKBACK_RESIST**: 击退抗性(减少20%击退时间)
- **TRAIT_SPLIT**: 分裂特性(死亡时产生小敌人)
- **TRAIT_GIANT**: 巨大化(体型+0.5倍，血量+2)

### 动态难度系统
- 每30秒增加敌人特性数量上限
- 每分钟增加最少敌人数量
- 敌人生成间隔随时间递减
- 食物收集影响蛇的移动速度和攻击频率

### AI决策算法
1. **安全性评估**: 检查碰撞和边界
2. **路径寻找**: BFS算法找到到食物的最短路径
3. **区域评估**: 计算可达区域避免死路
4. **威胁预测**: 预测敌人位置评估危险程度
5. **综合评分**: 多维度权重计算最优方向

## 扩展和修改

### 添加新敌人特性
1. 在`EnemyTrait`枚举中添加新特性
2. 在`applyTrait`方法中实现特性逻辑
3. 更新敌人生成系统的特性分配
4. 添加相应的测试用例

### 自定义AI行为
1. 修改`makeAIDecision`方法的评分算法
2. 调整权重参数和评估维度(安全性、食物、威胁)
3. 扩展日志记录格式(输出到ai_log.txt)
4. 测试AI决策质量和游戏表现

### 集成旧测试套件
1. 分析run_tests的测试用例
2. 在sob.cpp中添加新的构建目标
3. 创建测试源文件结构
4. 配置编译依赖和链接选项
5. 验证所有测试通过

## 项目状态

### ✅ 已完成
- 核心游戏实现(main.cpp + game_types.hpp)
- SOB构建系统完全配置(支持3个目标: sob, main, all_tests)
- 模块化测试架构(all_tests, 12/12通过, 100%)
  - 测试框架完整
  - 测试实际代码(无代码复制)
  - 模块化组织(simple_test, bullet_test)
- 跨平台编译支持(MSVC/GCC)
- 代码重构(提取game_types.hpp提高可测试性)
- AI日志系统(英文输出)
- 游戏结束自动退出功能

### ⚠️ 待处理
- **文件清理**: 
  - 未追踪文件: README.md, ai_log.txt, build/, tests/, 可执行文件
  - 旧可执行文件: snake, run_tests, simple_test, bullet_test
  - 需要配置.gitignore
- **已修改未提交**: 多个文件待提交

### 🔄 下一步建议
1. 提交当前更改(代码重构和测试架构)
2. 配置.gitignore排除构建产物
3. 清理旧文件和可执行文件
4. 为Enemy类添加单元测试
5. 考虑提取Enemy类到game_types.hpp
6. 性能优化和功能扩展

### 📊 代码质量
- **代码规模**: main.cpp + game_types.hpp
- **测试覆盖**: 12个测试，100%通过
  - Bullet类: 完整覆盖
  - 基础功能: 已测试
  - Enemy类: 待添加测试
- **构建状态**: 所有目标正常编译
- **可执行文件**: main, all_tests, sob

这个项目展示了现代C++游戏开发的完整流程，包括架构设计、AI算法、模板元编程构建系统和测试驱动开发，是学习现代软件工程实践的优秀实例。