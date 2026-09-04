#include <conio.h>
#include <windows.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

const int MAP_WIDTH = 30;
const int MAP_HEIGHT = 16;
const int FRAME_DELAY_MS = 110;

// 地图坐标：x 从左向右增加，y 从上向下增加。
struct Point {
    int x;
    int y;
};

enum Direction {
    DIR_STOP,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
};

bool gameOver = false;
bool quitRequested = false;
int score = 0;
Direction direction = DIR_STOP;

// snake[0] 是蛇头，其余元素依次保存蛇身坐标。
std::vector<Point> snake;
Point food;

bool IsOnSnake(int x, int y) {
    for (const Point& part : snake) {
        if (part.x == x && part.y == y) {
            return true;
        }
    }
    return false;
}

void SpawnFood() {
    const int availableCells = (MAP_WIDTH - 2) * (MAP_HEIGHT - 2);
    if (static_cast<int>(snake.size()) >= availableCells) {
        gameOver = true;
        return;
    }

    do {
        // 食物只生成在边框内部，并且不能与蛇身重合。
        food.x = 1 + std::rand() % (MAP_WIDTH - 2);
        food.y = 1 + std::rand() % (MAP_HEIGHT - 2);
    } while (IsOnSnake(food.x, food.y));
}

void HideCursor() {
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(output, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(output, &cursorInfo);
}

void Setup() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    gameOver = false;
    quitRequested = false;
    score = 0;
    direction = DIR_STOP;

    snake.clear();
    // 蛇头从地图中央开始。
    snake.push_back({MAP_WIDTH / 2, MAP_HEIGHT / 2});

    SpawnFood();
    HideCursor();
}

void Draw() {
    // 把光标移回左上角，再绘制下一帧，减少清屏造成的闪烁。
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, {0, 0});

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                std::cout << '#';
            } else if (snake[0].x == x && snake[0].y == y) {
                std::cout << 'O';
            } else if (food.x == x && food.y == y) {
                std::cout << '*';
            } else {
                bool drewBody = false;
                for (std::size_t i = 1; i < snake.size(); ++i) {
                    if (snake[i].x == x && snake[i].y == y) {
                        std::cout << 'o';
                        drewBody = true;
                        break;
                    }
                }

                if (!drewBody) {
                    std::cout << ' ';
                }
            }
        }
        std::cout << '\n';
    }

    std::cout << "Score: " << score << "    \n";
    std::cout << "W/A/S/D: Move    X: Exit    \n";

    if (direction == DIR_STOP && !gameOver) {
        std::cout << "Press a movement key to start.    \n";
    } else {
        std::cout << "                              \n";
    }
}

void Input() {
    if (!_kbhit()) {
        return;
    }

    const char key = static_cast<char>(_getch());

    switch (key) {
        case 'w':
        case 'W':
            if (direction != DIR_DOWN) {
                direction = DIR_UP;
            }
            break;
        case 's':
        case 'S':
            if (direction != DIR_UP) {
                direction = DIR_DOWN;
            }
            break;
        case 'a':
        case 'A':
            if (direction != DIR_RIGHT) {
                direction = DIR_LEFT;
            }
            break;
        case 'd':
        case 'D':
            if (direction != DIR_LEFT) {
                direction = DIR_RIGHT;
            }
            break;
        case 'x':
        case 'X':
            quitRequested = true;
            gameOver = true;
            break;
        default:
            break;
    }
}

void Logic() {
    if (gameOver || direction == DIR_STOP) {
        return;
    }

    Point nextHead = snake[0];

    switch (direction) {
        case DIR_LEFT:
            --nextHead.x;
            break;
        case DIR_RIGHT:
            ++nextHead.x;
            break;
        case DIR_UP:
            --nextHead.y;
            break;
        case DIR_DOWN:
            ++nextHead.y;
            break;
        case DIR_STOP:
            break;
    }

    // 蛇头碰到地图边框时游戏结束。
    if (nextHead.x <= 0 || nextHead.x >= MAP_WIDTH - 1 ||
        nextHead.y <= 0 || nextHead.y >= MAP_HEIGHT - 1) {
        gameOver = true;
        return;
    }

    const bool ateFood = nextHead.x == food.x && nextHead.y == food.y;

    // 没吃到食物时，尾部这一格会立即移走，因此允许蛇头进入旧尾部位置。
    std::size_t bodyPartsToCheck = snake.size();
    if (!ateFood && bodyPartsToCheck > 0) {
        --bodyPartsToCheck;
    }

    for (std::size_t i = 0; i < bodyPartsToCheck; ++i) {
        if (snake[i].x == nextHead.x && snake[i].y == nextHead.y) {
            gameOver = true;
            return;
        }
    }

    // 在 vector 开头插入新蛇头，相当于让整条蛇向前移动一格。
    // 与数组实现类似，蛇身坐标必须从尾部方向更新，避免前面的旧坐标过早被覆盖。
    snake.insert(snake.begin(), nextHead);

    if (ateFood) {
        // 吃到食物后不删除尾部，因此蛇会增长一格。
        score += 10;
        SpawnFood();
    } else {
        // 没吃到食物时删除最后一节，保持蛇的长度不变。
        snake.pop_back();
    }
}

int main() {
    Setup();

    // 游戏循环：每一帧依次绘制、读取输入、更新游戏状态，再短暂等待。
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(FRAME_DELAY_MS);
    }

    Draw();
    if (quitRequested) {
        std::cout << "Game closed by player.\n";
    } else {
        std::cout << "Game Over! Final score: " << score << "\n";
    }

    return 0;
}