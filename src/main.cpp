#include "tigr.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>

struct Point
{
    int x, y;
};

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Enemy
{
public:
    Point position;
    float speed;
    int moveCounter;
    
    Enemy(int x, int y, float moveSpeed)
    {
        position.x = x;
        position.y = y;
        speed = moveSpeed;
        moveCounter = 0;
    }
    
    void update(const Point& target)
    {
        moveCounter++;
        if (moveCounter >= speed)
        {
            moveCounter = 0;
            
            // 简单的追踪AI：向目标方向移动
            if (target.x < position.x) position.x--;
            else if (target.x > position.x) position.x++;
            
            if (target.y < position.y) position.y--;
            else if (target.y > position.y) position.y++;
        }
    }
};

class SnakeGame
{
private:
    std::vector<Point> snake;
    Point food;
    Direction direction;
    int cellSize;
    int gridWidth, gridHeight;
    bool gameOver;
    int score;
    std::vector<Enemy> enemies;

public:
    SnakeGame(int width, int height)
    {
        cellSize = 10;
        gridWidth = width / cellSize;
        gridHeight = height / cellSize;

        // 初始化蛇，从中间位置开始，长度为3
        snake.push_back({gridWidth / 2, gridHeight / 2});
        snake.push_back({gridWidth / 2 - 1, gridHeight / 2});
        snake.push_back({gridWidth / 2 - 2, gridHeight / 2});

        direction = RIGHT;
        gameOver = false;
        score = 0;

        // 设置随机种子
        std::srand(std::time(0));

        // 生成第一个食物
        generateFood();
        
        // 初始化敌人
        enemies.clear();
        // 在四个角落各放置一个敌人
        enemies.push_back(Enemy(2, 2, 15.0f));  // 左上角，移动较慢
        enemies.push_back(Enemy(gridWidth - 3, 2, 12.0f));  // 右上角
        enemies.push_back(Enemy(2, gridHeight - 3, 12.0f));  // 左下角
        enemies.push_back(Enemy(gridWidth - 3, gridHeight - 3, 10.0f));  // 右下角，移动较快
    }

    void generateFood()
    {
        bool validPosition = false;
        while (!validPosition)
        {
            food.x = std::rand() % gridWidth;
            food.y = std::rand() % gridHeight;

            validPosition = true;
            for (const auto &segment : snake)
            {
                if (segment.x == food.x && segment.y == food.y)
                {
                    validPosition = false;
                    break;
                }
            }
        }
    }

    void update()
    {
        if (gameOver)
            return;

        // 更新敌人位置
        for (auto &enemy : enemies)
        {
            enemy.update(snake[0]);  // 敌人追踪蛇头
        }

        // 计算新的头部位置
        Point newHead = snake[0];

        switch (direction)
        {
        case UP:
            newHead.y--;
            break;
        case DOWN:
            newHead.y++;
            break;
        case LEFT:
            newHead.x--;
            break;
        case RIGHT:
            newHead.x++;
            break;
        }

        // 检查墙壁碰撞
        if (newHead.x < 0 || newHead.x >= gridWidth ||
            newHead.y < 0 || newHead.y >= gridHeight)
        {
            gameOver = true;
            return;
        }

        // 检查自身碰撞
        for (const auto &segment : snake)
        {
            if (newHead.x == segment.x && newHead.y == segment.y)
            {
                gameOver = true;
                return;
            }
        }

        // 检查与敌人的碰撞
        for (const auto &enemy : enemies)
        {
            if (newHead.x == enemy.position.x && newHead.y == enemy.position.y)
            {
                gameOver = true;
                return;
            }
            
            // 检查敌人是否碰到蛇身
            for (const auto &segment : snake)
            {
                if (segment.x == enemy.position.x && segment.y == enemy.position.y)
                {
                    gameOver = true;
                    return;
                }
            }
        }

        // 添加新头部
        snake.insert(snake.begin(), newHead);

        // 检查是否吃到食物
        if (newHead.x == food.x && newHead.y == food.y)
        {
            score++;
            generateFood();
        }
        else
        {
            // 如果没有吃到食物，移除尾部
            snake.pop_back();
        }
    }

    void handleInput(Tigr *screen)
    {
        if (tigrKeyDown(screen, TK_UP) && direction != DOWN)
        {
            direction = UP;
        }
        else if (tigrKeyDown(screen, TK_DOWN) && direction != UP)
        {
            direction = DOWN;
        }
        else if (tigrKeyDown(screen, TK_LEFT) && direction != RIGHT)
        {
            direction = LEFT;
        }
        else if (tigrKeyDown(screen, TK_RIGHT) && direction != LEFT)
        {
            direction = RIGHT;
        }
    }

    void draw(Tigr *screen)
    {
        // 清屏
        tigrClear(screen, tigrRGB(0x20, 0x20, 0x20));
        
        // 计算游戏区域偏移，使其居中
        int borderWidth = gridWidth * cellSize;
        int borderHeight = gridHeight * cellSize;
        int offsetX = (screen->w - borderWidth) / 2;
        int offsetY = (screen->h - borderHeight) / 2;
        
        // 绘制游戏区域边框
        tigrRect(screen, offsetX, offsetY, borderWidth, borderHeight, tigrRGB(0xFF, 0xFF, 0xFF));

        // 绘制蛇
        for (size_t i = 0; i < snake.size(); i++)
        {
            int x = offsetX + snake[i].x * cellSize;
            int y = offsetY + snake[i].y * cellSize;

            // 头部用不同颜色
            if (i == 0)
            {
                tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0x00, 0xFF, 0x00));
            }
            else
            {
                tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0x00, 0x80, 0x00));
            }
        }

        // 绘制食物
        tigrFillRect(screen, offsetX + food.x * cellSize, offsetY + food.y * cellSize, cellSize, cellSize, tigrRGB(0xFF, 0x00, 0x00));
        
        // 绘制敌人
        for (const auto &enemy : enemies)
        {
            int x = offsetX + enemy.position.x * cellSize;
            int y = offsetY + enemy.position.y * cellSize;
            // 用橙色绘制敌人，并添加边框使其更醒目
            tigrFillRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0xA5, 0x00));
            tigrRect(screen, x, y, cellSize, cellSize, tigrRGB(0xFF, 0x00, 0x00));
        }

        // 显示分数
        char scoreText[32];
        sprintf(scoreText, "Score: %d", score);
        tigrPrint(screen, tfont, offsetX + 5, offsetY + 5, tigrRGB(0xFF, 0xFF, 0xFF), scoreText);

        // 游戏结束提示
        if (gameOver)
        {
            const char *gameOverText = "Game Over! Press ESC to exit";
            int textWidth = tigrTextWidth(tfont, gameOverText);
            tigrPrint(screen, tfont, (screen->w - textWidth) / 2, screen->h / 2,
                      tigrRGB(0xFF, 0xFF, 0xFF), gameOverText);
        }
    }

    bool isGameOver() const { return gameOver; }
};

int main(int argc, char *argv[])
{
    Tigr *screen = tigrWindow(640, 480, "贪吃蛇游戏", TIGR_AUTO);
    SnakeGame game(640, 480);

    float accumulator = 0.0f;
    const float updateInterval = 0.1f; // 每0.1秒更新一次游戏状态

    while (!tigrClosed(screen))
    {
        // tigrTime() 返回自上次调用以来的时间间隔
        float deltaTime = tigrTime();
        accumulator += deltaTime;

        // 处理输入
        game.handleInput(screen);
        std::cout << "deltaTime: " << deltaTime << std::endl;

        // 按固定间隔更新游戏状态
        if (accumulator >= updateInterval)
        {
            std::cout << "update" << std::endl;
            game.update();
            accumulator -= updateInterval;
        }

        // 绘制游戏
        game.draw(screen);

        // ESC键退出游戏
        if (tigrKeyDown(screen, TK_ESCAPE))
        {
            break;
        }

        tigrUpdate(screen);
    }

    tigrFree(screen);
    return 0;
}