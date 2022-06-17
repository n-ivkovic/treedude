#ifndef END_H
#define END_H

#define _XOPEN_SOURCE 500 /* SUSv2 (inc. C89/90, POSIX.1-1990, POSIX.2-1992, POSIX.1b-1993, POSIX.1c-1995) */

#include "common.h"

/* End data */
typedef struct end {
	loop_t loops;
	fade_t fade;
	shown_t shown;
	unsigned int bigTextsTopStage;
	sprite_t bigTextL;
	sprite_t bigTextR;
	sprite_t bigTextScore;
	sprite_t bigNumScore;
	text_t textHighScore;
	bool_t showRestart;
} end_t;

/**
 * Initialise end data
 *
 * @param e  End data to initalise
 */
void initEnd(end_t *e);

/**
 * Update end
 *
 * @param e          End data
 * @param stage      Program stage
 * @param gameShown  Percentage (0-100) of game stage shown
 * @param highScore  Highest score achieved
 * @param score      Score achieved
 * @param inp        User keyboard input
 */
void updateEnd(end_t *e, stage_t *stage, shown_t *gameShown, score_t *highScore, const score_t score, const input_t inp, const flag_t flags);

/**
 * Draw end
 *
 * @param win  Window to draw end to
 * @param e    End data
 */
void drawEnd(const window_t win, const end_t e);

#endif
