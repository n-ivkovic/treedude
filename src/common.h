#ifndef COMMON_H
#define COMMON_H

#define _XOPEN_SOURCE 500 /* SUSv2 (inc. C89/90, POSIX.1-1990, POSIX.2-1992, POSIX.1b-1993, POSIX.1c-1995) */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>

#define VERSION "1.0.5"

/* Loops per sec must be >= 12 and divisible by 4. Decrease to 16 if the terminal 'flickers' too much at 20 */
#define LOOPS_PER_SEC 20
#define MS_PER_LOOP (1000 / LOOPS_PER_SEC)
#define FADE_SPEED (LOOPS_PER_SEC / 2)

#define NEW_LINE "\n"
#ifndef PATH_MAX
#define PATH_MAX 0x1000
#endif
#define LOOPS_MAX 0x7FFFFFFF /* Max loop_t value */
#define TEXT_SCORE_COLS_MAX 10 /* Length of max score_t value as a string */
#define WIN_COLS 64
#define WIN_ROWS 24
#define SHOWN_MIN 0
#define SHOWN_MAX 100
#define SPRITE_STR_MAX ((WIN_COLS * WIN_ROWS) / 4) /* Sprites are not larger than 1/4 of the main window */

#define FLAG_MAIN_NONE            0x00
#define FLAG_MAIN_NOHIGHSCOREFILE 0x01

#define FLAG_DRAW_NONE        0x00
#define FLAG_DRAW_BOLD        0x01
#define FLAG_DRAW_DIM         0x02
#define FLAG_DRAW_SKIP_SPACES 0x04
#define FLAG_DRAW_ACS         0x08

#define DUDE_ROWS 6
#define DUDE_COLS 15
#define DUDE_SIZE (DUDE_ROWS * DUDE_COLS + 1)
#define DUDE_STILL_L_STR "      ###      [   ]#####       |  |- -|_      |/  \\_/  \\     ||_______|      |___|___|   "
#define DUDE_STILL_R_STR "      ###           #####[   ]     |- -|_ |     /  \\_/  \\|     |_______||     |___|___|   "
#define DUDE_CHOP_L_STR "    ###           #####          |o o|_       /  \\_/  \\      |___   ----=== |___|___|     "
#define DUDE_CHOP_R_STR "        ###           #####          |o o|_       /  \\_/  \\ ===-----   --|      |___|___| "
#define DUDE_DEAD_STR "                   -------       /       \\      | R.I.P.|      |       |      | _  _ _|   "

#define BIGTEXT_ROWS 5
#define BIGTEXT_TREE_STR "````` ````  ```` ````  `   `   ` `    `     `   ````  ```  ```   `   ` `   `    `     `   `  `` ```` ````"
#define BIGTEXT_TREE_COLS strlen(BIGTEXT_TREE_STR) / BIGTEXT_ROWS
#define BIGTEXT_TREE_SIZE (BIGTEXT_ROWS * BIGTEXT_TREE_COLS + 1)
#define BIGTEXT_DUDE_STR "```  `   ` ```  `````  ` `   ` `  ` `   `  ` `   ` `  ` ``` `  ` `   ` `  ` `   ```   ```  ```  ````"
#define BIGTEXT_DUDE_COLS strlen(BIGTEXT_DUDE_STR) / BIGTEXT_ROWS
#define BIGTEXT_DUDE_SIZE (BIGTEXT_ROWS * BIGTEXT_DUDE_COLS + 1)

/**
 * Get the reverse value
 *
 * @param val  Current value
 * @param a    First of two possible values
 * @param b    Second of two possible values
 * @returns    The reverse of the current value
 */
#define REVERSE(val, a, b) ((val == a) ? b : a)

/**
 * Null out a string
 *
 * @param str  String to empty
 */
#define NULL_STRING(str) (str[0] = '\0')

/**
 * Get center Y-pos of an object
 *
 * @param rows  Rows in object
 */
#define Y_POS_CENTER(rows) ((WIN_ROWS - rows) / 2)

/**
 * Get center X-pos of an object
 *
 * @param cols  Cols in object
 */
#define X_POS_CENTER(cols) ((WIN_COLS - cols) / 2)

/* Abstract typedefs */
typedef int           input_t;
typedef signed long   loop_t;
typedef signed int    coord_t;
typedef unsigned char dimention_t;
typedef signed char   shown_t;
typedef unsigned long score_t;
typedef unsigned int  flag_t;

/* Boolean */
typedef enum {
	FALSE_E,
	TRUE_E
} bool_t;

/* Program stage */
typedef enum {
	INTRO,
	GAME,
	END,
	RESET,
	QUIT
} stage_t;

/* Object size */
typedef struct dimentions {
	dimention_t rows;
	dimention_t cols;
} dimentions_t;

/* Object position */
typedef struct position {
	coord_t y;
	coord_t x;
} position_t;

/* Bordered window */
typedef struct window {
	WINDOW *win;
	dimentions_t size;
	position_t pos;
} window_t;

/* Program screen */
typedef struct screen {
	window_t win;
	dimentions_t size;
} screen_t;

/* Drawable text object */
typedef struct text {
	char str[WIN_COLS];
	dimention_t len;
	position_t pos;
} text_t;

/* Drawable sprite object */
typedef struct sprite {
	char str[SPRITE_STR_MAX];
	dimentions_t size;
	position_t pos;
} sprite_t;

/* Object fade stages */
typedef enum {
	IN,
	IN_DONE,
	OUT,
	OUT_DONE
} fade_t;

/**
 * Get current Unix epoch timestamp in ms
 *
 * @returns Current timestamp
 */
long getEpochMs(void);

/**
 * Sleep for a given length of time in ms
 *
 * @param ms  Time to sleep
 */
void sleepMs(long ms);

/**
 * Initialise random seed
 */
void initRandSeed(void);

/**
 * Get random integer within a range
 *
 * @param min  Minimum inclusive integer
 * @param max  Maximum inclusive integer
 * @returns    Random integer
 */
int randInt(int min, int max);

/**
 * Round a float to the nearest int
 *
 * @param val  Value to round
 * @returns    Rounded value
 */
int roundToInt(float val);

/**
 * Initialise display
 */
void initDisplay(void);

/**
 * Free display
 */
void freeDisplay(void);

/**
 * Initialise window
 *
 * @param win     Window to initialise
 * @param parWin  Parent window
 * @param y       Y-pos of window
 * @param x       X-pos of window
 * @param rows    Number of rows in window
 * @param cols    Number of columns in window
 */
void initWindow(window_t *win, const window_t *parWin, const coord_t y, const coord_t x, const dimention_t rows, const dimention_t cols);

/**
 * Update window contents
 *
 * @param win    Window to update
 * @param flags  Drawing flags
 */
void updateWindow(const window_t win, const flag_t flags);

/**
 * Clear window contents
 *
 * @param win  Window to clear
 */
void clearWindow(const window_t win);

/**
 * Free window memory
 *
 * @param win  Window to free
 */
void freeWindow(window_t *win);

/**
 * Initialise screen
 *
 * @param screen  Screen to initalise
 * @param rows    Number of rows in screen
 * @param cols    Number of columns in screen
 */
void initScreen(screen_t *screen, const dimention_t rows, const dimention_t cols);

/**
 * Update screen contents
 *
 * @param screen  Screen to update
 */
void updateScreen(screen_t *screen);

/**
 * Free screen memory 
 *
 * @param screen  Screen to free
 */
void freeScreen(screen_t *screen);

/**
 * Draw single line string
 *
 * @param win    Window to draw to
 * @param str    String to draw
 * @param len    Length of string
 * @param y      Y-pos of string
 * @param x      X-pos of string
 * @param shown  Percentage (0-100) of string shown
 * @param flags  Drawing flags
 */
void drawString(const window_t win, const char* str, const dimention_t len, const coord_t y, const coord_t x, const shown_t shown, const flag_t flags);

/**
 * Draw text object
 *
 * @param win    Window to draw to
 * @param text   Text to draw
 * @param shown  Percentage (0-100) of object shown
 * @param flags  Drawing flags
 */
void drawText(const window_t win, const text_t text, const shown_t shown, const flag_t flags);

/**
 * Draw sprite object
 *
 * @param win    Window to draw to
 * @param spr    Sprite to draw
 * @param shown  Percentage (0-100) of object shown
 * @param flags  Drawing flags
 */
void drawSprite(const window_t win, const sprite_t spr, const shown_t shown, const flag_t flags);

/**
 * Update fade in/out
 *
 * @param fade   Fade state to update
 * @param shown  Percentage (0-100) of object to show during fade
 * @param multi  Multiplier of fade speed
 * @returns      Whether fade has been updated 
 */
bool_t updateFadeMulti(fade_t *fade, shown_t *shown, const float multi);

/**
 * Update fade in/out
 *
 * @param fade   Fade state to update
 * @param shown  Percentage (0-100) of object to show during fade
 * @returns      Whether fade has been updated
 */
bool_t updateFade(fade_t *fade, shown_t *shown);

/**
 * Load high score from file
 *
 * @returns High score loaded from file, defaults to 0
 */
score_t loadHighScore(void);

/**
 * Save high score to file
 */
void saveHighScore(score_t highScore);

#endif
