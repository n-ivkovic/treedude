#ifndef INTRO_H
#define INTRO_H

#define _XOPEN_SOURCE 500 /* C89 + POSIX.1-1990 + POSIX.2-1992 + SUSv2 */

#include "common.h"

/* Intro stage */
typedef enum {
	INIT,
	TEXT_TOP,
	DUDE_FADE_IN,
	DUDE_CHOP,
	TEXT_BOTTOM,
	FADE_OUT,
	TITLE
} introStage_t;

/* Intro data */
typedef struct intro {
	introStage_t stage;
	loop_t loops;
	loop_t loopStageStart;
	loop_t loopStart;
	fade_t fade;
	shown_t shown;
	shown_t shownCenter;
	shown_t shownTop;
	shown_t shownBottom;
	sprite_t center;
	text_t textTop;
	text_t textBottom;
} intro_t;

/**
 * Initialise intro data
 *
 * @param i  Intro data to initalise
 */
void initIntro(intro_t *i);

/**
 * Update intro
 *
 * @param i      Intro data
 * @param stage  Program stage
 * @param inp    User keyboard input
 */
void updateIntro(intro_t *i, stage_t *stage, const input_t inp);

/**
 * Draw intro
 *
 * @param win  Window to draw intro on
 * @param i    Intro data
 */
void drawIntro(const window_t win, const intro_t i);

#endif
