#include "end.h"

#include <string.h>

#define BIGTEXT_TOP_Y 3
#define BIGTEXT_SCORE_Y (BIGTEXT_TOP_Y + 1 + BIGTEXT_ROWS)
#define BIGTEXT_SCORE_PADDING 4
#define MAX_BIGTEXT_TOP_STAGE 2

#define TEXT_HIGHSCORE_OLD_START_STR "--PERSONAL-BEST-"
#define TEXT_HIGHSCORE_OLD_START_LEN strlen(TEXT_HIGHSCORE_OLD_START_STR)
#define TEXT_HIGHSCORE_OLD_END_STR "--"
#define TEXT_HIGHSCORE_OLD_END_LEN strlen(TEXT_HIGHSCORE_OLD_END_STR)
#define TEXT_HIGHSCORE_NEW_STR "--NEW-PERSONAL-BEST--"
#define TEXT_HIGHSCORE_NEW_LEN strlen(TEXT_HIGHSCORE_NEW_STR)
#define TEXT_RESTART_STR "[PRESS ENTER TO RESTART]"
#define TEXT_RESTART_LEN strlen(TEXT_RESTART_STR)

#define BIGTEXT_DEAD_STR "```  ````  ``  ``` `  ` `    `  ` `  ``  ` ```  ```` `  ``  ` `    `  ` `  ````  ```` `  ` ``` "
#define BIGTEXT_DEAD_COLS (strlen(BIGTEXT_DEAD_STR) / BIGTEXT_ROWS)
#define BIGTEXT_DEAD_SIZE (BIGTEXT_ROWS * BIGTEXT_DEAD_COLS + 1)
#define BIGTEXT_SCORE_STR " ```   ```  ```  ````  `````     `    `   ` `   ` `    ```  `    `   ` ````  ```     ` `    `   ` ` `   `   ````   ```  ```  `  `` ````"
#define BIGTEXT_SCORE_COLS (strlen(BIGTEXT_SCORE_STR) / BIGTEXT_ROWS)
#define BIGTEXT_SCORE_SIZE (BIGTEXT_ROWS * BIGTEXT_SCORE_COLS + 1)
#define BIGTEXT_0_STR " ``` `  ``` ` ```  ` ``` "
#define BIGTEXT_0_COLS (strlen(BIGTEXT_0_STR) / BIGTEXT_ROWS)
#define BIGTEXT_0_SIZE (BIGTEXT_ROWS * BIGTEXT_0_COLS + 1)
#define BIGTEXT_1_STR " `` ```  ``  `` ````"
#define BIGTEXT_1_COLS (strlen(BIGTEXT_1_STR) / BIGTEXT_ROWS)
#define BIGTEXT_1_SIZE (BIGTEXT_ROWS * BIGTEXT_1_COLS + 1)
#define BIGTEXT_2_STR " `` `  `  `  `  ````"
#define BIGTEXT_2_COLS (strlen(BIGTEXT_2_STR) / BIGTEXT_ROWS)
#define BIGTEXT_2_SIZE (BIGTEXT_ROWS * BIGTEXT_2_COLS + 1)
#define BIGTEXT_3_STR "```    ` ``    ```` "
#define BIGTEXT_3_COLS (strlen(BIGTEXT_3_STR) / BIGTEXT_ROWS)
#define BIGTEXT_3_SIZE (BIGTEXT_ROWS * BIGTEXT_3_COLS + 1)
#define BIGTEXT_4_STR "`  ``  `````   `   `"
#define BIGTEXT_4_COLS (strlen(BIGTEXT_4_STR) / BIGTEXT_ROWS)
#define BIGTEXT_4_SIZE (BIGTEXT_ROWS * BIGTEXT_4_COLS + 1)
#define BIGTEXT_5_STR "`````   ```    ```` "
#define BIGTEXT_5_COLS (strlen(BIGTEXT_5_STR) / BIGTEXT_ROWS)
#define BIGTEXT_5_SIZE (BIGTEXT_ROWS * BIGTEXT_5_COLS + 1)
#define BIGTEXT_6_STR "  `   `   ```` `   ` ``` "
#define BIGTEXT_6_COLS (strlen(BIGTEXT_6_STR) / BIGTEXT_ROWS)
#define BIGTEXT_6_SIZE (BIGTEXT_ROWS * BIGTEXT_6_COLS + 1)
#define BIGTEXT_7_STR "`````   `   `    `    `  "
#define BIGTEXT_7_COLS (strlen(BIGTEXT_7_STR) / BIGTEXT_ROWS)
#define BIGTEXT_7_SIZE (BIGTEXT_ROWS * BIGTEXT_7_COLS + 1)
#define BIGTEXT_8_STR " ``` `   ` ``` `   ` ``` "
#define BIGTEXT_8_COLS (strlen(BIGTEXT_8_STR) / BIGTEXT_ROWS)
#define BIGTEXT_8_SIZE (BIGTEXT_ROWS * BIGTEXT_8_COLS + 1)
#define BIGTEXT_9_STR " `````   ` ````   `   `  "
#define BIGTEXT_9_COLS (strlen(BIGTEXT_9_STR) / BIGTEXT_ROWS)
#define BIGTEXT_9_SIZE (BIGTEXT_ROWS * BIGTEXT_9_COLS + 1)

static void setBigTextsTop(end_t *e)
{
	switch (e->bigTextsTopStage) {
		/* Dude Dead */
		case 0:
			strncpy(e->bigTextL.str, BIGTEXT_DUDE_STR, BIGTEXT_DUDE_SIZE);
			strncpy(e->bigTextR.str, BIGTEXT_DEAD_STR, BIGTEXT_DEAD_SIZE);
			e->bigTextL.size.cols = BIGTEXT_DUDE_COLS;
			e->bigTextR.size.cols = BIGTEXT_DEAD_COLS;
			break;
		/* Dead Dude */
		case 1:
			strncpy(e->bigTextL.str, BIGTEXT_DEAD_STR, BIGTEXT_DEAD_SIZE);
			strncpy(e->bigTextR.str, BIGTEXT_DUDE_STR, BIGTEXT_DUDE_SIZE);
			e->bigTextL.size.cols = BIGTEXT_DEAD_COLS;
			e->bigTextR.size.cols = BIGTEXT_DUDE_COLS;
			break;
		/* Tree Dude */
		case 2:
			strncpy(e->bigTextL.str, BIGTEXT_TREE_STR, BIGTEXT_TREE_SIZE);
			strncpy(e->bigTextR.str, BIGTEXT_DUDE_STR, BIGTEXT_DUDE_SIZE);
			e->bigTextL.size.cols = BIGTEXT_TREE_COLS;
			e->bigTextR.size.cols = BIGTEXT_DUDE_COLS;
			break;
	}
}

void initEnd(end_t *e)
{
	e->loops = 0;
	e->fade = IN;
	e->shown = SHOWN_MIN;

	/* Top big texts */
	e->bigTextL.size.rows = BIGTEXT_ROWS;
	e->bigTextR.size.rows = BIGTEXT_ROWS;
	e->bigTextL.pos.y = BIGTEXT_TOP_Y;
	e->bigTextR.pos.y = BIGTEXT_TOP_Y;
	e->bigTextL.pos.x = WIN_COLS / 2 - BIGTEXT_TREE_COLS;
	e->bigTextR.pos.x = WIN_COLS / 2 + 3;
	e->bigTextsTopStage = 0;
	setBigTextsTop(e);

	/* Big text score */
	strncpy(e->bigTextScore.str, BIGTEXT_SCORE_STR, BIGTEXT_SCORE_SIZE);
	e->bigTextScore.size.rows = BIGTEXT_ROWS;
	e->bigTextScore.size.cols = BIGTEXT_SCORE_COLS;
	e->bigTextScore.pos.y = BIGTEXT_SCORE_Y;
	e->bigTextScore.pos.x = X_POS_CENTER(BIGTEXT_SCORE_COLS);

	/* Big num score */
	NULL_STRING(e->bigNumScore.str);
	e->bigNumScore.size.rows = BIGTEXT_ROWS;
	e->bigNumScore.size.cols = 0;
	e->bigNumScore.pos.y = BIGTEXT_SCORE_Y;

	/* High score text */
	NULL_STRING(e->textHighScore.str);
	e->textHighScore.len = 0;
	e->textHighScore.pos.y = BIGTEXT_SCORE_Y + BIGTEXT_ROWS + 1;

	/* Restart text */
	e->showRestart = FALSE_E;
}

static void setHighScore(end_t *e, score_t *highScore, const score_t score, const flag_t mainFlags)
{
	char scoreStr[SCORE_STR_MAX];

	/* Set new high score */
	if (score > *highScore) {
		*highScore = score;

		/* Write to high score file */
		if (mainFlags & FLAG_MAIN_WRITE_HIGH_SCORE)
			saveHighScore(*highScore);

		/* Set high score text */
		strncpy(e->textHighScore.str, TEXT_HIGHSCORE_NEW_STR, TEXT_HIGHSCORE_NEW_LEN + 1);
		e->textHighScore.len = TEXT_HIGHSCORE_NEW_LEN;
	/* Old high score remains */
	} else {
		/* Set high score text */
		sprintf(scoreStr, "%lu", *highScore);
		strncpy(e->textHighScore.str, TEXT_HIGHSCORE_OLD_START_STR, TEXT_HIGHSCORE_OLD_START_LEN + 1);
		strcat(e->textHighScore.str, scoreStr);
		strncat(e->textHighScore.str, TEXT_HIGHSCORE_OLD_END_STR, TEXT_HIGHSCORE_OLD_END_LEN + 1);
		e->textHighScore.len = TEXT_HIGHSCORE_OLD_START_LEN + strlen(scoreStr) + TEXT_HIGHSCORE_OLD_END_LEN;
	}

	/* Set high score text position */
	e->textHighScore.pos.x = X_POS_CENTER(e->textHighScore.len);
}

static void setScoreBigText(end_t *e, const score_t score)
{
	char scoreStr[SCORE_STR_MAX];
	char bigTextStr[SPRITE_STR_MAX];
	size_t rowInd, numInd, scoreLen, bigTextCols;

	/* Score to string */
	sprintf(scoreStr, "%lu", score);
	scoreLen = strlen(scoreStr);

	/* Set score big text */
	for (rowInd = 0; rowInd < BIGTEXT_ROWS; rowInd++) {
		for (numInd = 0; numInd < scoreLen; numInd++) {
			/* Copy number string to buffer */
			switch (scoreStr[numInd]) {
				case '0': strncpy(bigTextStr, BIGTEXT_0_STR, BIGTEXT_0_SIZE); bigTextCols = BIGTEXT_0_COLS; break;
				case '1': strncpy(bigTextStr, BIGTEXT_1_STR, BIGTEXT_1_SIZE); bigTextCols = BIGTEXT_1_COLS; break;
				case '2': strncpy(bigTextStr, BIGTEXT_2_STR, BIGTEXT_2_SIZE); bigTextCols = BIGTEXT_2_COLS; break;
				case '3': strncpy(bigTextStr, BIGTEXT_3_STR, BIGTEXT_3_SIZE); bigTextCols = BIGTEXT_3_COLS; break;
				case '4': strncpy(bigTextStr, BIGTEXT_4_STR, BIGTEXT_4_SIZE); bigTextCols = BIGTEXT_4_COLS; break;
				case '5': strncpy(bigTextStr, BIGTEXT_5_STR, BIGTEXT_5_SIZE); bigTextCols = BIGTEXT_5_COLS; break;
				case '6': strncpy(bigTextStr, BIGTEXT_6_STR, BIGTEXT_6_SIZE); bigTextCols = BIGTEXT_6_COLS; break;
				case '7': strncpy(bigTextStr, BIGTEXT_7_STR, BIGTEXT_7_SIZE); bigTextCols = BIGTEXT_7_COLS; break;
				case '8': strncpy(bigTextStr, BIGTEXT_8_STR, BIGTEXT_8_SIZE); bigTextCols = BIGTEXT_8_COLS; break;
				case '9': strncpy(bigTextStr, BIGTEXT_9_STR, BIGTEXT_9_SIZE); bigTextCols = BIGTEXT_9_COLS; break;
				default: continue;
			}
			/* Concat row of number string to score string */
			strncat(e->bigNumScore.str, &bigTextStr[rowInd * bigTextCols], bigTextCols);
			/* Add spacing between numbers */
			strncat(e->bigNumScore.str, (numInd < scoreLen - 1) ? " " : NEW_LINE, 2);
			/* Set number of columns in sprite */ 
			if (rowInd == 0 && numInd == scoreLen - 1)
				e->bigNumScore.size.cols = (dimention_t)strlen(e->bigNumScore.str);
		}
	}

	/* Update big text score positions */
	e->bigTextScore.pos.x = X_POS_CENTER(BIGTEXT_SCORE_COLS - BIGTEXT_SCORE_PADDING - e->bigNumScore.size.cols);
	e->bigNumScore.pos.x = e->bigTextScore.pos.x + BIGTEXT_SCORE_COLS + BIGTEXT_SCORE_PADDING;
}

bool_t updateEnd(end_t *e, stage_t *stage, shown_t *gameShown, score_t *highScore, const score_t score, const input_t inp, const flag_t mainFlags)
{
	bool_t draw = FALSE_E;

	/* Initialise */
	if (e->loops == 0) {
		setHighScore(e, highScore, score, mainFlags);
		setScoreBigText(e, score);
		e->loops++;
	}

	/* Update fade */
	if (fadeShown(e->fade, &e->shown)) {
		if (e->fade == OUT)
			*gameShown = e->shown;
		if (fadeDone(OUT, e->shown))
			*stage = RESET;
		return TRUE_E;
	}

	/* Fade out on Enter key press */
	if (inp == 10) {
		e->fade = OUT;
		return FALSE_E;
	}

	/* Change big text every 0.75s after 0.5s */
	if (e->loops > LOOPS_PER_SEC / 2 && (e->loops + LOOPS_PER_SEC / 2) % ((LOOPS_PER_SEC / 4) * 3) == 0) {
		if (e->bigTextsTopStage >= MAX_BIGTEXT_TOP_STAGE)
			e->bigTextsTopStage = 0;
		else
			e->bigTextsTopStage++;
		setBigTextsTop(e);
		draw = TRUE_E;
	}

	/* Flash restart every 0.75s */
	if (e->loops > 0 && e->loops % ((LOOPS_PER_SEC / 4) * 3) == 0) {
		e->showRestart = !e->showRestart;
		draw = TRUE_E;
	}

	e->loops++;
	return draw;
}

void drawEnd(const window_t win, const end_t e)
{
	drawSprite(win, e.bigTextL, e.shown, FLAG_DRAW_BOLD | FLAG_DRAW_SKIP_SPACES | FLAG_DRAW_ACS);
	drawSprite(win, e.bigTextR, e.shown, FLAG_DRAW_BOLD | FLAG_DRAW_SKIP_SPACES | FLAG_DRAW_ACS);
	drawSprite(win, e.bigTextScore, e.shown, FLAG_DRAW_BOLD | FLAG_DRAW_SKIP_SPACES | FLAG_DRAW_ACS);
	drawSprite(win, e.bigNumScore, e.shown, FLAG_DRAW_BOLD | FLAG_DRAW_SKIP_SPACES | FLAG_DRAW_ACS);
	drawText(win, e.textHighScore, e.shown, FLAG_DRAW_BOLD);

	if (e.showRestart)
		drawString(win, TEXT_RESTART_STR, TEXT_RESTART_LEN, WIN_ROWS - 4, X_POS_CENTER(TEXT_RESTART_LEN), e.shown, FLAG_DRAW_BOLD);
}
