// snake.c

#include "snake.h" // for the snake game header file
#include <stdio.h> // for printf function
#include <curses.h> // for drawing the snake on the screen, curses or ncurses? 
#include <unistd.h> // for usleep function
#include <stdbool.h> // for boolean type
#include <stdlib.h> // for rand and srand functions
#include <time.h> // for seeding the random number generator

// Preprocessor directives for the snake game
#define MIN_WINDOW_SIZE 100 // Minimum window size for the game to run
#define MAX_SNAKE_LENGTH 100 // Maximum length of the snake
#define SNAKE_SPEED 100000 // Speed of the snake in microseconds
#define LINES 24 // Number of lines in the terminal window
#define COLS 80 // Number of columns in the terminal window
#define BOARD_CHAR '#' // Character used for the border
#define SNAKE_HEAD_CHAR 'O' // Character used for the snake's head
#define SNAKE_BODY_CHAR 'o' // Character used for the snake's body
#define FOOD_CHAR '*' // Character used for the food
#define SCORE_POSITION_Y 0 // Y position for the score display
#define SCORE_POSITION_X 0 // X position for the score display
#define SCORE_TEXT "Score: " // Text for the score display
#define SCORE_TEXT_LENGTH 7 // Length of the score text
#define SCORE_DIGITS 5 // Maximum number of digits for the score display
#define BOARD_WIDTH (COLS - 2) // Width of the game board
#define BOARD_HEIGHT (LINES - 2) // Height of the game board
#define SNAKE_COLOR_PAIR 1 // Color pair number for the snake (green)
#define FOOD_COLOR_PAIR 2 // Color pair number for the food (yellow)



// Initialize curses or any other necessary libraries for drawing the snake on the screen would go here. For simplicity, we will not include actual drawing code in this snippet.
void initialize_curses() {
    initscr(); // initialize the curses mode
    cbreak(); // disable line buffering
    noecho(); // do not echo input characters
    keypad(stdscr, TRUE); // enable special keys (like arrow keys)
    nodelay(stdscr, TRUE); // make getch() non-blocking
    curs_set(0); // hide the cursor
    if (has_colors()) {
        start_color(); // enable color support
        use_default_colors(); // let -1 mean the terminal's own background
        init_pair(SNAKE_COLOR_PAIR, COLOR_GREEN, -1);
        init_pair(FOOD_COLOR_PAIR, COLOR_YELLOW, -1);
    }
}

void initialize_board() {
    // Draw the border of the game board
    for (int x = 0; x < COLS; x++) {
        mvaddch(0, x, BOARD_CHAR); // top border
        mvaddch(LINES - 1, x, BOARD_CHAR); // bottom border
    }
    for (int y = 0; y < LINES; y++) {
        mvaddch(y, 0, BOARD_CHAR); // left border
        mvaddch(y, COLS - 1, BOARD_CHAR); // right border
    }
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

// Global food position
static int food_x;
static int food_y;

// Function to place the food at a random spot inside the border, off the snake
void place_food() {
    bool on_snake;
    do {
        food_x = 1 + rand() % BOARD_WIDTH; // columns 1 to COLS - 2
        food_y = 1 + rand() % BOARD_HEIGHT; // lines 1 to LINES - 2
        on_snake = false;
        for (int i = 0; i < snake.length; i++) {
            if (snake.x[i] == food_x && snake.y[i] == food_y) {
                on_snake = true;
                break;
            }
        }
    } while (on_snake);
}

// Function to initialize the snake's state
int initialize_game() {
    snake.length = 1; // start with a length of 1
    snake.x[0] = 5; // starting x position
    snake.y[0] = 5; // starting y position
    snake.direction = 1; // start moving right
    snake.alive = true; // snake is alive at the start
    snake.moving = true; // snake is moving at the start
    snake.growing = false; // snake is not growing at the start
    place_food(); // put the first food on the board
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
        // Remember where the tail is, so a new segment can go there if the snake eats
        int tail_x = snake.x[snake.length - 1];
        int tail_y = snake.y[snake.length - 1];

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
        // Grow by one segment at the old tail position and move the food
        if (head_on_food()) {
            if (snake.length < MAX_SNAKE_LENGTH) {
                snake.x[snake.length] = tail_x;
                snake.y[snake.length] = tail_y;
                snake.length++;
            }
            place_food();
        }
    }
    return 0; // success
}

// Function to check if the snake's head is on the food
bool head_on_food() {
    return snake.x[0] == food_x && snake.y[0] == food_y;
}

// Function to check if the snake hit a wall or itself
bool check_collision() {
    // The border sits on the outermost lines and columns, so touching it ends the game
    if (snake.x[0] <= 0 || snake.x[0] >= COLS - 1 || snake.y[0] <= 0 || snake.y[0] >= LINES - 1) {
        return true;
    }
    // The body has already moved, so the head only collides if it landed on another segment
    for (int i = 1; i < snake.length; i++) {
        if (snake.x[i] == snake.x[0] && snake.y[i] == snake.y[0]) {
            return true;
        }
    }
    return false;
}

// Function to draw the snake on the screen
void draw_snake() {
    attron(COLOR_PAIR(SNAKE_COLOR_PAIR));
    for (int i = 0; i < snake.length; i++) {
        char segment = (i == 0) ? SNAKE_HEAD_CHAR : SNAKE_BODY_CHAR; // head gets its own character
        mvaddch(snake.y[i], snake.x[i], segment);
    }
    attroff(COLOR_PAIR(SNAKE_COLOR_PAIR));
}

// Function to draw the border around the game area
void draw_border() {
    initialize_board(); // erase() wipes the border each frame, so redraw it
}

// Function to draw the food on the screen
void draw_food() {
    attron(COLOR_PAIR(FOOD_COLOR_PAIR));
    mvaddch(food_y, food_x, FOOD_CHAR);
    attroff(COLOR_PAIR(FOOD_COLOR_PAIR));
}

// Function to draw the current score on the screen
void draw_score() {
    // TODO: print SCORE_TEXT and the score at SCORE_POSITION_Y, SCORE_POSITION_X
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

// Function to show the game over message and ask whether to play again
// Returns true to play again, false to quit
bool draw_game_over() {
    const char* message = "GAME OVER";
    const char* prompt = "Press R to play again or Q to quit";
    mvprintw(LINES / 2 - 1, (COLS - (int)strlen(message)) / 2, "%s", message);
    mvprintw(LINES / 2 + 1, (COLS - (int)strlen(prompt)) / 2, "%s", prompt);
    refresh();
    flushinp(); // discard keys pressed during play so they don't skip this screen
    nodelay(stdscr, FALSE); // make getch() wait for a key

    int key;
    do {
        key = getch();
    } while (key != 'r' && key != 'R' && key != 'q' && key != 'Q'); // ignore any other key

    nodelay(stdscr, TRUE); // make getch() non-blocking again for the next game
    return key == 'r' || key == 'R';
}

// Function to clean up resources used by the game
void cleanup_game() {
    // TODO: nothing to free yet
}

int main(void) {
    initialize_curses(); // Initialize curses for drawing
    initialize_board(); // Initialize the game board
    if (window_handler() != 0) {
        endwin(); // End curses mode if window size is insufficient
        return -1; // Exit the program with an error code
    }
    srand(time(NULL)); // seed the random number generator for food placement

    do {
        initialize_game(); // Initialize (or reset) the game state
        draw_game(); // Draw the initial game state

        while (snake.alive) {
            char* input = handle_input(); // Handle user input
            int new_direction = -1; // -1 means no direction key was pressed
            if (strcmp(input, "up") == 0) {
                new_direction = 0;
            } else if (strcmp(input, "right") == 0) {
                new_direction = 1;
            } else if (strcmp(input, "down") == 0) {
                new_direction = 2;
            } else if (strcmp(input, "left") == 0) {
                new_direction = 3;
            }
            // Directions go clockwise (up, right, down, left), so the opposite is two steps away.
            // Ignore a reversal once the snake has a body, or the head would turn into it.
            bool reversing = new_direction == (snake.direction + 2) % 4;
            if (new_direction != -1 && !(reversing && snake.length > 1)) {
                snake.direction = new_direction;
            }

            update_snake(); // Update the snake's position
            draw_game(); // Draw the whole frame

            usleep(SNAKE_SPEED); // Control the speed of the game loop
        }
    } while (draw_game_over()); // Start a new game if the player chooses to play again

    cleanup_game(); // Clean up resources used by the game
    endwin(); // End curses mode
    return 0; // Exit the program
}

