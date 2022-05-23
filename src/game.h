#ifndef GAME_H
#define GAME_H

#define _XOPEN_SOURCE 500 /* C89 + POSIX.1-1990 + POSIX.2-1992 + SUSv2 */

#include "common.h"

/* Game state */
typedef enum {
	STILL,
	CHOP,
	CHOPPED,
	DEAD
} gameState_t;

/* Object side */
typedef enum {
	NONE,
	LEFT,
	RIGHT
} side_t;

/* Chunk (section) of tree */
typedef struct treeChunk {
	side_t side;
	shown_t shown;
	position_t pos;
	struct treeChunk *next;
} treeChunk_t;

/* Whole tree */
typedef struct tree {
	treeChunk_t *base;
	treeChunk_t *top;
} tree_t;

/* Inner game window */
typedef struct gameWindow {
	window_t win;
	text_t text;
} gameWindow_t;

/* Game data */
typedef struct game {
	gameState_t state;
	loop_t loops;
	loop_t loopDead;
	loop_t loopLevelBigText;
	fade_t fade;
	shown_t shown;
	side_t dudeSide;
	unsigned int level;
	score_t levelUpScore;
	unsigned int timer;
	loop_t timerDecaySpeed;
	tree_t tree;
	tree_t chopped;
	unsigned int chopTextOffsetY;
	bool_t showChopDir;
	bool_t showWatchTime;
	sprite_t levelBigText;
	shown_t levelBigTextShown;
	fade_t levelBigTextFade;
	gameWindow_t winTimer;
	gameWindow_t winScore;
} game_t;

/**
 * Update game data on term resize
 *
 * @param g       Game data
 * @param screen  Program screen
 */
void resizeGame(game_t *g, const screen_t screen);

/**
 * Initialise game data
 *
 * @param g       Game data to initialise
 * @param screen  Program screen
 */
void initGame(game_t *g, const screen_t screen);

/**
 * Update game
 *
 * @param g       Game data
 * @param stage   Program stage
 * @param score   Current score
 * @param screen  Program screen
 * @param inp     User keyboard input
 */
void updateGame(game_t *g, stage_t *stage, score_t *score, const screen_t screen, const input_t inp);

/**
 * Draw game
 *
 * @param win    Window to draw game to
 * @param g      Game data
 * @param stage  Program stage
 */
void drawGame(const window_t win, const game_t g, const stage_t stage);

/**
 * Free game memory
 *
 * @param g  Game data
 */
void freeGame(game_t *g);

#endif
