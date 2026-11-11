// snake.h

#include <stdbool.h>
#include <string.h>

// Forward declarations for snake game

void initialize_curses(); // initializes curses mode and terminal settings
void initialize_board(); // draws the border of the game board
char* handle_input(); // returns a string representing the input direction
int update_snake(); // updates the snake's position and returns 0 on success, -1 on collision
void draw_snake(); // draws the snake on the screen
bool check_collision(); // returns true if the snake collides with itself or the wall
void reset_game(); // resets the game state to the initial conditions
void game_loop(); // main game loop that handles input, updates the snake, and draws the game state
int initialize_game(); // returns 0 on success, -1 on failure
void cleanup_game(); // cleans up resources used by the game
void set_snake_direction(int direction); // sets the direction of the snake (0 = up, 1 = right, 2 = down, 3 = left)
void grow_snake(); // increases the length of the snake by one segment
void shrink_snake(); // decreases the length of the snake by one segment
void set_snake_speed(int speed); // sets the speed of the snake (higher value = faster)
void set_snake_color(int color); // sets the color of the snake (0 = default, 1 = red, 2 = green, etc.)
void set_snake_length(int length); // sets the length of the snake (minimum length = 1)
void set_snake_position(int x, int y); // sets the position of the snake's head on the grid
bool is_snake_alive(); // returns true if the snake is alive, false if it has collided
bool is_snake_moving(); // returns true if the snake is currently moving
bool is_snake_growing(); // returns true if the snake is currently growing
bool head_on_food(); // returns true if the snake's head is on the food
void place_food(); // places the food at a random empty spot inside the border
int window_handler(); // handles window resizing and returns 0 on success, -1 on failure
void draw_game(); // draws the entire game state, including the snake, food, and any other elements
void draw_border(); // draws the border around the game area
void draw_food(); // draws the food on the screen
void draw_score(); // draws the current score on the screen
bool draw_game_over(); // shows the game over message; returns true to play again, false to quit