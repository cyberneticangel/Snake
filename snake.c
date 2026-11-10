// snake.c

#include "snake.h"
#include <stdio.h>

// First, we need to define the snake's properties and state
#define MAX_SNAKE_LENGTH 100
#define SNAKE_SPEED 100000 // microseconds between updates

// Next, we need to define the snake's properties and state
typedef struct {
    int x[MAX_SNAKE_LENGTH]; // x-coordinates of the snake segments
    int y[MAX_SNAKE_LENGTH]; // y-coordinates of the snake segments
    int length; // current length of the snake
    int direction; // current direction of the snake (0 = up, 1 = right, 2 = down, 3 = left)
    bool alive; // whether the snake is alive or has collided
    bool moving; // whether the snake is currently moving
    bool growing; // whether the snake is currently growing
} Snake;