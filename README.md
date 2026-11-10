# Snake (C + ncurses)

A classic Snake game for the terminal, hand-written in C from scratch using the ncurses library.

This README is both the project description and a **build roadmap**: it breaks the game into small milestones, each with goals, hints, and a check to confirm it works before moving on. It deliberately contains **no game code**. The point is to write it yourself.

---

## Table of Contents

1. [Gameplay](#gameplay)
2. [Controls](#controls)
3. [Requirements](#requirements)
4. [Building and Running](#building-and-running)
5. [Project Structure](#project-structure)
6. [Core Concepts](#core-concepts)
7. [Roadmap](#roadmap)
   - [Milestone 0: Toolchain](#milestone-0-toolchain)
   - [Milestone 1: ncurses Basics](#milestone-1-ncurses-basics)
   - [Milestone 2: The Playfield](#milestone-2-the-playfield)
   - [Milestone 3: The Snake](#milestone-3-the-snake)
   - [Milestone 4: Movement](#milestone-4-movement)
   - [Milestone 5: The Game Loop and Timing](#milestone-5-the-game-loop-and-timing)
   - [Milestone 6: Input](#milestone-6-input)
   - [Milestone 7: Food and Growth](#milestone-7-food-and-growth)
   - [Milestone 8: Collisions and Game Over](#milestone-8-collisions-and-game-over)
   - [Milestone 9: Polish](#milestone-9-polish)
8. [Common Pitfalls](#common-pitfalls)
9. [Debugging Tips](#debugging-tips)
10. [Stretch Goals](#stretch-goals)
11. [Reference](#reference)

---

## Gameplay

You control a snake moving around a walled board. Eat food to grow longer and score points. The game ends when the snake hits a wall or its own body. The longer you survive, the harder it gets.

## Controls

| Key                 | Action                      |
| ------------------- | --------------------------- |
| Arrow keys / `WASD` | Change direction            |
| `p`                 | Pause / resume *(planned)*  |
| `r`                 | Restart after game over     |
| `q`                 | Quit                        |

## Requirements

- **macOS** (or Linux) with a terminal that is at least as large as the playfield
- **A C compiler**: `clang` comes with the Xcode Command Line Tools (`xcode-select --install`)
- **ncurses**: included with macOS; on Linux, install your distro's ncurses development package
- **make** *(optional, recommended)*

## Building and Running

> To be filled in once Milestone 0 is done.

Hints for working it out:

- You compile a single source file with `clang` and must **link the ncurses library**. Look up how the `-l` flag works.
- Always compile with warnings on (`-Wall -Wextra`). Treat every warning as a bug until proven otherwise.
- While developing, add debug symbols (`-g`) and AddressSanitizer (`-fsanitize=address`).
- Write a `Makefile` with a default target that builds the game and a `clean` target that deletes the binary.

## Project Structure

Planned layout:

```
snake/
├── README.md     this file
├── Makefile      build rules
└── snake.c       the whole game (split into modules later if desired)
```

Planned functions inside `snake.c`:

| Function       | Responsibility                                                       |
| -------------- | -------------------------------------------------------------------- |
| `main`         | Set up ncurses, run the loop, restore the terminal on exit           |
| `init_game`    | Reset all game state (snake, direction, food, score, flags)          |
| `handle_input` | Read every pending key and record the requested direction or action  |
| `update`       | Advance the game by exactly one tick                                 |
| `draw`         | Render the border, snake, food, and score                            |
| `spawn_food`   | Place food on a random empty cell                                    |

All game state lives in **one struct** that is passed by pointer to these functions. That avoids a pile of globals and makes restarting trivial.

---

## Core Concepts

### 1. Everything is a grid

The board is a grid of cells. Every position (each snake segment, the food) is stored as a **grid coordinate**, a small struct with an `x` and a `y`. Collision checks then become "are these two coordinates equal?"

Conversion to screen position happens **only when drawing**.

### 2. ncurses coordinates are (row, column)

ncurses functions take **y first, then x**. Row numbers increase **downward**. Mixing this up is the most common early bug.

### 3. Terminal cells aren't square

A terminal character is roughly twice as tall as it is wide, so a square grid looks stretched and vertical movement feels faster. A common fix is to draw each grid cell as **two characters wide** (screen column = x × 2).

### 4. The snake is an ordered list

Index 0 is the **head**, and the last index is the **tail**. Movement does not move every segment independently. Instead, a new head is added in front and the tail is dropped. The body "follows" automatically.

### 5. Game logic runs on fixed ticks

The game advances in discrete steps (ticks), for example every 100 ms. Input can arrive at any time, but it only takes effect at the next tick.

---

## Roadmap

Work through these in order. **Don't start a milestone until the previous one passes its check.**

### Milestone 0: Toolchain

**Goal:** you can compile and run a C program.

- [ ] Confirm `clang --version` works (install the Command Line Tools if it doesn't)
- [ ] Create `snake.c` with a program that prints a message
- [ ] Compile it with warnings enabled
- [ ] Create a `Makefile` so that `make` builds and `make clean` cleans

**Check:** `make && ./snake` prints your message, with zero warnings.

---

### Milestone 1: ncurses Basics

**Goal:** take over the terminal, draw a character, and give the terminal back cleanly.

The ncurses lifecycle is **initialize → configure → use → restore**.

Functions to look up:

| Function   | Purpose                                                       |
| ---------- | ------------------------------------------------------------- |
| `initscr`  | Start ncurses mode                                            |
| `cbreak`   | Deliver keys immediately, without waiting for Enter           |
| `noecho`   | Don't print typed keys to the screen                          |
| `keypad`   | Turn arrow keys into single codes (`KEY_UP`, etc.)            |
| `curs_set` | Hide the blinking cursor                                      |
| `mvaddch`  | Draw one character at (row, col)                              |
| `mvprintw` | Draw formatted text at (row, col)                             |
| `erase`    | Clear the screen buffer (less flicker than `clear`)           |
| `refresh`  | Push the buffer to the actual screen                          |
| `getch`    | Read a key                                                    |
| `endwin`   | Leave ncurses mode and restore the terminal                   |

- [ ] Initialize and configure ncurses
- [ ] Draw `@` in the middle of the screen (look up `LINES` and `COLS`)
- [ ] Wait for a keypress
- [ ] Clean up and exit

**Check:** you see the `@`, a keypress exits, and your terminal behaves normally afterward.

> If your terminal gets stuck in a weird state after a crash, type `reset` and press Enter (even if you can't see what you're typing).

---

### Milestone 2: The Playfield

**Goal:** draw a bordered board.

- [ ] Define constants for board **width** and **height** in cells (for example 30 × 20)
- [ ] Define a **point struct** (`x`, `y`)
- [ ] Draw a border around the board using `#` or the ncurses box-drawing characters (look up `ACS_HLINE`, `ACS_VLINE`, `ACS_ULCORNER`, and so on)
- [ ] Decide whether you'll use double-width cells (see [Core Concepts #3](#3-terminal-cells-arent-square))
- [ ] Write a helper that converts a grid point to a screen (row, col), and use it for all drawing

**Check:** a clean rectangle is drawn. If the terminal is too small, print a friendly message and exit instead of drawing garbage.

---

### Milestone 3: The Snake

**Goal:** a snake is stored in memory and drawn on screen.

Choose a data structure:

| Option                          | Pros                                     | Cons                                |
| ------------------------------- | ---------------------------------------- | ----------------------------------- |
| **Fixed array + length** ⭐      | Simple, no `malloc`, no leaks            | Shifting the body costs O(n) per tick |
| Circular buffer                 | O(1) moves, still no `malloc`            | Index math is trickier              |
| Linked list                     | Classic exercise in pointers             | Manual memory management            |

Recommended: **fixed array + length**. Size the array as width × height, since the snake can never be longer than the board.

- [ ] Create a game-state struct holding the snake array, its length, direction, food, score, and a game-over flag
- [ ] Write `init_game` to place a snake of length 3 near the center, horizontally
- [ ] Draw the head with one character (for example `@`) and the body with another (for example `o`)

**Check:** a 3-segment snake appears inside the border, head distinguishable from body.

---

### Milestone 4: Movement

**Goal:** the snake moves one cell per call to `update`.

Store the direction as a point used as a vector:

| Direction | Vector     |
| --------- | ---------- |
| Right     | `( 1,  0)` |
| Left      | `(-1,  0)` |
| Down      | `( 0,  1)` |
| Up        | `( 0, -1)` |

Algorithm per tick:

1. Compute the **new head** = current head + direction.
2. **Shift every segment back one slot**, looping from the **tail toward the head**. Going backward avoids overwriting data you haven't copied yet.
3. Write the new head into index 0.

- [ ] Implement `update` with the algorithm above
- [ ] Temporarily call `update` once per keypress to step through movement manually

**Check:** each keypress moves the snake one cell, and the body follows the head's path exactly.

---

### Milestone 5: The Game Loop and Timing

**Goal:** the snake moves on its own at a steady speed.

The main loop: **handle input → update (if a tick has passed) → draw → sleep briefly**.

`getch` blocks by default, so you need non-blocking input. Options:

| Approach               | How                                                      | Trade-off                                          |
| ---------------------- | -------------------------------------------------------- | -------------------------------------------------- |
| `timeout(ms)`          | `getch` waits at most `ms` milliseconds                   | Simple, but keypresses shorten ticks, so speed is uneven |
| `nodelay` + sleep      | `getch` returns `ERR` immediately when no key is pressed | Even ticks if you sleep a fixed amount             |
| `nodelay` + clock ⭐    | Track elapsed time and only update when the tick interval has passed | Most accurate; slightly more code  |

For the clock approach, look up `clock_gettime` with `CLOCK_MONOTONIC`, and use `nanosleep` or `usleep` for short sleeps (a few ms) between checks.

- [ ] Make input non-blocking
- [ ] Define a tick interval (start around 100–150 ms)
- [ ] Only call `update` when a full tick has elapsed

**Check:** the snake glides across the board at a constant speed, regardless of whether you're pressing keys.

---

### Milestone 6: Input

**Goal:** responsive, bug-free steering.

- [ ] Map arrow keys and `WASD` to direction vectors
- [ ] `q` quits cleanly
- [ ] Read **all** pending keys each loop iteration, not just one, so input doesn't lag behind
- [ ] **Forbid reversing**: ignore a direction that is the exact opposite of the current one
  - 💡 Two vectors are opposite when they add up to `(0, 0)`
- [ ] **Fix the quick-double-turn bug** (see [Common Pitfalls](#common-pitfalls)): store a *pending* direction, validate it against the direction the snake **actually moved last tick**, and apply it only at the start of `update`

**Check:** you can steer freely, and no combination of fast keypresses makes the snake turn back into itself.

---

### Milestone 7: Food and Growth

**Goal:** eating food makes the snake longer and raises the score.

- [ ] Seed the random number generator **once** at startup (look up `srand` and `time`)
- [ ] Write `spawn_food` to pick a random cell **not occupied by the snake**
  - Simple: pick random cells until one is free
  - Robust: collect all free cells into a list and pick one at random (this stays fast when the snake fills most of the board)
- [ ] Draw the food with a distinct character (for example `*`)
- [ ] When the new head lands on the food: grow, add to the score, spawn new food
  - 💡 Growing with an array: increase `length` by 1 **before** shifting, so the old tail is kept instead of dropped
- [ ] Display the score above or below the board

**Check:** food never appears on the snake, eating grows the snake by exactly one segment, and the score updates.

---

### Milestone 8: Collisions and Game Over

**Goal:** the game ends correctly and can be restarted.

- [ ] **Wall collision**: the new head is outside `0..width-1` or `0..height-1`
- [ ] **Self collision**: the new head equals any body segment
  - 💡 Subtle: on a tick where the snake is **not** growing, the tail moves out of its cell, so moving into the current tail is technically legal. Decide on your rule and order your checks to match.
- [ ] Check collisions **before** committing the move
- [ ] On game over: show a message with the final score and "press `r` to restart, `q` to quit"
- [ ] Restart by calling `init_game` again

**Check:** hitting a wall or yourself ends the game, restarting gives a completely fresh state, and quitting restores the terminal.

---

### Milestone 9: Polish

Pick any of these:

- [ ] **Colors**: look up `has_colors`, `start_color`, `init_pair`, `attron`, `attroff`
- [ ] **Speed ramp**: shorten the tick interval every N foods, down to a minimum
- [ ] **Pause** with `p`
- [ ] **High score** saved to a file (`fopen`, `fscanf`, `fprintf`)
- [ ] **Win condition**: the snake fills the entire board
- [ ] **Wrap-around mode**: leaving one edge enters the opposite edge
  - ⚠️ In C, `%` on a negative number gives a negative result. Add the board size before taking the modulo.
- [ ] **Handle terminal resizes** gracefully (look up `KEY_RESIZE`)

---

## Common Pitfalls

| Symptom | Likely cause |
| --- | --- |
| Terminal is broken after the game exits | `endwin` wasn't called (crash, early `return`, or Ctrl-C). Run `reset`. Consider handling `SIGINT` to clean up. |
| Things are drawn in the wrong place | Swapped x and y. ncurses takes **(row, col)**. |
| Arrow keys print junk or do nothing | `keypad` wasn't enabled on `stdscr`. |
| Snake moves faster while you hold a key | Using `timeout` alone; each keypress ends the wait early. Use clock-based ticks. |
| Snake dies when pressing two keys quickly | Direction changed twice in one tick, reversing into the neck. Validate against the last *moved* direction. |
| Snake body gets corrupted when moving | Shifting segments from head to tail instead of **tail to head**. |
| Same food positions every game | `srand` not called, or called with a constant. |
| Random crashes or weird values | Writing past the end of the snake array. Use AddressSanitizer. |
| Screen flickers | Using `clear` every frame. Use `erase`, then a single `refresh`. |
| Vertical movement looks faster | Terminal cells aren't square. Use double-width cells. |

## Debugging Tips

- **`printf` doesn't mix with ncurses.** Instead:
  - Print debug info to a spare screen row with `mvprintw`, or
  - Write to a log file (`fprintf` to a `FILE*` opened at startup) and watch it from a second terminal with `tail -f`.
- **Compile with `-g -fsanitize=address`** during development. It reports out-of-bounds writes at the exact line they happen.
- **Step through ticks manually.** Temporarily make each keypress advance exactly one tick (as in Milestone 4) to inspect behavior frame by frame.
- **`lldb ./snake`** works, but ncurses and the debugger fight over the terminal. Attaching from a second terminal (`lldb -p <pid>`) works better.

## Stretch Goals

Once the game is complete:

- Replace the array shift with a **circular buffer** for O(1) movement
- Split the code into modules (`game.c`, `render.c`, `input.c`) with header files
- Add obstacles or multiple levels loaded from text files
- Two-player mode on one keyboard
- An AI that plays by itself (start with greedy pathing toward the food, then try BFS)
- Port the renderer to **SDL2** for a real graphical window, keeping the game logic untouched. A clean separation between logic and drawing makes this easy.

## Reference

- `man ncurses`: overview of the library
- `man <function>` works for every ncurses function (for example `man getch`, `man timeout`)
- *NCURSES Programming HOWTO* by Pradeep Padala: the classic beginner tutorial
- `man 3 clock_gettime`, `man 3 nanosleep`, `man 3 rand`
