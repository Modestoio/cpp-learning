# Snake Game

A simple console-based Snake game written in C++ as part of my C++ learning journey.

## Features

- Console map border
- WASD movement
- Random food generation
- Snake growth
- Score system
- Wall collision
- Self collision
- Controlled game-loop speed

## Controls

- W - Up
- S - Down
- A - Left
- D - Right
- X - Exit

## Build

This project uses the Windows console APIs from `<windows.h>` and `<conio.h>`.
With MinGW-w64 `g++`, run the following command in this directory:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o snake.exe
```

Then run it in PowerShell:

```powershell
.\snake.exe
```

## Learning Goals

This project is intended to help practice:

- Simple functions
- `while` and `for` loops
- `if` and `switch` statements
- Enums and basic structs
- `std::vector`
- Map coordinates
- Keyboard input
- Random food placement
- The game loop and basic game logic

## Code Structure

- `Setup()` initializes the snake, score, direction, and food.
- `Draw()` renders the map, snake, food, controls, and score.
- `Input()` reads keyboard input without blocking the game loop.
- `Logic()` moves the snake and checks food and collisions.
- `main()` runs the game loop at a controlled speed.