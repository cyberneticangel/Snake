// snake.c

#include "snake.h"
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h> // for usleep function

// Preprocessor directives for the snake game
#define MIN_WINDOW_SIZE 100 // Minimum window size for the game to run
#define MAX_SNAKE_LENGTH 100 // Maximum length of the snake
#define SNAKE_SPEED 100000 // Speed of the snake in microseconds
#define LINES 24 // Number of lines in the terminal window
#define COLS 80 // Number of columns in the terminal window


// Initialize curses or any other necessary libraries for drawing the snake on the screen would go here. For simplicity, we will not include actual drawing code in this snippet.
void initialize_curses() {
    initscr(); // initialize the curses mode
    cbreak(); // disable line buffering
    noecho(); // do not echo input characters
    keypad(stdscr, TRUE); // enable special keys (like arrow keys)
    nodelay(stdscr, TRUE); // make getch() non-blocking
    curs_set(0); // hide the cursor
}

int window_handler() {
    int window_size = getmaxx(stdscr) * getmaxy(stdscr); // get the current window size
    if (window_size < MIN_WINDOW_SIZE) {
        // log the failure due to insufficient window size
        mvprintw(0, 0, "Window size too small! Please resize the window to at least %d characters.", MIN_WINDOW_SIZE);
        return -1; // failure due to insufficient window size
    }
    return 0; // success
}

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

// Global snake instance
static Snake snake;

// Function to initialize the snake's state
int initialize_game() {
    snake.length = 1; // start with a length of 1
    snake.x[0] = 5; // starting x position
    snake.y[0] = 5; // starting y position
    snake.direction = 1; // start moving right
    snake.alive = true; // snake is alive at the start
    snake.moving = true; // snake is moving at the start
    snake.growing = false; // snake is not growing at the start
    return 0; // success
}

// Function to handle user input and change the snake's direction
char* handle_input() {
    switch (getch()) {
        case KEY_UP:
            return "up";
        case KEY_RIGHT:
            return "right";
        case KEY_DOWN:
            return "down";
        case KEY_LEFT:
            return "left";
        default:
            // print error message for invalid input
            mvprintw(0, 0, "Invalid input! Use arrow keys to control the snake.");
            return ""; // no input
    }
}

// Function to update the snake's position based on its current direction
int update_snake() {
    if (!snake.alive) {
        return -1; // cannot update if the snake is dead
    }
    else {
        // Move the snake's body segments
        for (int i = snake.length - 1; i > 0; i--) {
            snake.x[i] = snake.x[i - 1];
            snake.y[i] = snake.y[i - 1];
        }
        // Update the head position based on the current direction
        switch (snake.direction) {
            case 0: // up
                snake.y[0]--;
                break;
            case 1: // right
                snake.x[0]++;
                break;
            case 2: // down
                snake.y[0]++;
                break;
            case 3: // left
                snake.x[0]--;
                break;
        }
        // Check for collisions
        if (check_collision()) {
            snake.alive = false; // mark the snake as dead
            return -1; // collision occurred
        }
    }
    return 0; // success
}

// Function to check if the snake hit a wall or itself
bool check_collision() {
    // TODO: check the head against the walls and the body segments
    return false; // stub: never collides yet
}

// Function to draw the snake on the screen
void draw_snake() {
    // TODO: draw each segment (head and body can use different characters)
}

// Function to draw one full frame of the game
void draw_game() {
    erase(); // clear the screen buffer (less flicker than clear())
    draw_border(); // draw the border
    draw_snake(); // draw the snake
    draw_food(); // draw the food
    draw_score(); // draw the score
    refresh(); // push the finished frame to the terminal
}

// Function to clean up resources used by the game
void cleanup_game() {
    // TODO: nothing to free yet
}

int main(void) {
    initialize_curses(); // Initialize curses for drawing
    initialize_game(); // Initialize the game state
    draw_game(); // Draw the initial game state

    while (snake.alive) {
        char* input = handle_input(); // Handle user input
        if (strcmp(input, "up") == 0) {
            snake.direction = 0;
        } else if (strcmp(input, "right") == 0) {
            snake.direction = 1;
        } else if (strcmp(input, "down") == 0) {
            snake.direction = 2;
        } else if (strcmp(input, "left") == 0) {
            snake.direction = 3;
        }

        update_snake(); // Update the snake's position
        draw_game(); // Draw the whole frame

        usleep(SNAKE_SPEED); // Control the speed of the game loop
    }

    cleanup_game(); // Clean up resources used by the game
    endwin(); // End curses mode
    return 0; // Exit the program
}

