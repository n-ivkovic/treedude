#include "intro.h"

#define TEXT_INTRO_TOP_STR "HIS NAME IS TREEDUDE"
#define TEXT_INTRO_TOP_LEN strlen(TEXT_INTRO_TOP_STR)
#define TEXT_INTRO_BOTTOM_STR "AND HIS SONG IS..."
#define TEXT_INTRO_BOTTOM_LEN strlen(TEXT_INTRO_BOTTOM_STR)
#define TEXT_START_STR "[PRESS ENTER TO START]"
#define TEXT_START_LEN strlen(TEXT_START_STR)

#define TOP_SCROLL_PER_LOOP TEXT_INTRO_TOP_LEN / LOOPS_PER_SEC
#define BOTTOM_SCROLL_PER_LOOP TEXT_INTRO_BOTTOM_LEN / LOOPS_PER_SEC
#define BOTTOM_SLOW_SCROLL_LEN TEXT_INTRO_BOTTOM_LEN - 3

#define MS_SINCE_INTRO_STAGE_START (long)((float)(i->loops - i->loopStageStart) / (float)LOOPS_PER_SEC * 1000)

void initIntro(intro_t *i)
{
	i->stage = INIT;

	i->loops = 0;
	i->loopStageStart = 0;
	i->loopStart = LOOPS_MAX;

	i->fade = IN_DONE;
	i->shown = SHOWN_MIN;
	i->shownCenter = SHOWN_MIN;
	i->shownTop = SHOWN_MAX;
	i->shownBottom = SHOWN_MAX;

	/* Centre sprite (init as dude) */
	strncpy(i->center.str, DUDE_STILL_L_STR, DUDE_SIZE);
	i->center.pos.y = Y_POS_CENTER(DUDE_ROWS);
	i->center.pos.x = X_POS_CENTER(DUDE_COLS);
	i->center.size.rows = DUDE_ROWS;
	i->center.size.cols = DUDE_COLS;

	/* Top text */
	strncpy(i->textTop.str, TEXT_INTRO_TOP_STR, TEXT_INTRO_TOP_LEN + 1);
	i->textTop.len = 0;
	i->textTop.pos.y = Y_POS_CENTER(DUDE_ROWS) - 2;
	i->textTop.pos.x = X_POS_CENTER(TEXT_INTRO_TOP_LEN);

	/* Bottom text */
	strncpy(i->textBottom.str, TEXT_INTRO_BOTTOM_STR, TEXT_INTRO_BOTTOM_LEN + 1);
	i->textBottom.len = 0;
	i->textBottom.pos.y = Y_POS_CENTER(DUDE_ROWS) + DUDE_ROWS + 1;
	i->textBottom.pos.x = X_POS_CENTER(TEXT_INTRO_BOTTOM_LEN);
}

static void progressIntroStage(intro_t *i)
{
	i->loopStageStart = i->loops + 1;

	if (i->stage == TITLE)
		i->loopStageStart -= LOOPS_PER_SEC;
	else
		i->stage++;
}

void updateIntro(intro_t *i, stage_t *stage, const input_t inp)
{
	/* Update fade */
	if (updateFade(&i->fade, &i->shown)) {
		if (i->fade == OUT_DONE)
			(*stage) = GAME;
		return;
	}

	/* Fade out on key press */
	if (inp == 10) /* Enter */
		i->fade = OUT;

	/* Process stages */
	switch (i->stage) {
		case INIT:
			if (MS_SINCE_INTRO_STAGE_START >= 500)
				progressIntroStage(i);
			break;

		case TEXT_TOP:
			/* Scroll top text */
			if (i->textTop.len < TEXT_INTRO_TOP_LEN)
				i->textTop.len += (TOP_SCROLL_PER_LOOP > 0) ? TOP_SCROLL_PER_LOOP : 1;
			/* Finish stage */
			if (i->textTop.len >= TEXT_INTRO_TOP_LEN) {
				i->textTop.len = TEXT_INTRO_TOP_LEN;
				progressIntroStage(i);
			}
			break;

		case DUDE_FADE_IN:
			/* Wait before fading */
			if (MS_SINCE_INTRO_STAGE_START < 250)
				break;
			/* Fade in dude */
			i->shownCenter += FADE_SPEED;
			if (i->shownCenter >= SHOWN_MAX) {
				i->shownCenter = SHOWN_MAX;
				progressIntroStage(i);
			}
			break;

		case DUDE_CHOP:
			switch (MS_SINCE_INTRO_STAGE_START) {
				case 250:
					/* Set dude to chop */
					strncpy(i->center.str, DUDE_CHOP_L_STR, DUDE_SIZE);
					i->center.pos.y--;
					i->center.pos.x += 2;
					break;
				case 500:
					/* Set dude to still */
					strncpy(i->center.str, DUDE_STILL_R_STR, DUDE_SIZE);
					i->center.pos.y++;
					i->center.pos.x -= 2;
					break;
				case 1500:
					/* Finish stage */
					progressIntroStage(i);
					break;
			}
			break;

		case TEXT_BOTTOM:
			/* Scroll bottom text */
			if (i->textBottom.len < BOTTOM_SLOW_SCROLL_LEN || i->loops % (LOOPS_PER_SEC / 2) == 0)
				i->textBottom.len += (i->textBottom.len < BOTTOM_SLOW_SCROLL_LEN && BOTTOM_SCROLL_PER_LOOP > 0) ? BOTTOM_SCROLL_PER_LOOP : 1;
			/* Finish stage */
			if (i->textBottom.len >= TEXT_INTRO_BOTTOM_LEN) {
				i->textBottom.len = TEXT_INTRO_BOTTOM_LEN;
				progressIntroStage(i);
			}
			break;

		case FADE_OUT:
			/* Wait before fading */
			if (MS_SINCE_INTRO_STAGE_START < 1250)
				break;
			/* Fade out all */
			if (i->shownCenter > SHOWN_MIN || i->shownTop > SHOWN_MIN || i->shownBottom > SHOWN_MIN) {
				i->shownCenter -= FADE_SPEED;
				i->shownTop -= FADE_SPEED;
				i->shownBottom -= FADE_SPEED;
				if (i->shownCenter < SHOWN_MIN || i->shownTop < SHOWN_MIN || i->shownBottom < SHOWN_MIN) {
					i->shownCenter = SHOWN_MIN;
					i->shownTop = SHOWN_MIN;
					i->shownBottom = SHOWN_MIN;
				}
			/* Finish stage */
			} else {
				progressIntroStage(i);
			}
			break;

		case TITLE:
			switch (MS_SINCE_INTRO_STAGE_START) {
				case 0:
					/* Show start text in 7 sec */
					i->loopStart = i->loops + LOOPS_PER_SEC * 7;
					/* Center to hidden big text */
					i->shownCenter = SHOWN_MIN;
					i->center.pos.y = Y_POS_CENTER(BIGTEXT_ROWS);
					i->center.size.rows = BIGTEXT_ROWS;
					/* Bottom text hidden to start text */
					i->shownBottom = SHOWN_MIN;
					strncpy(i->textBottom.str, TEXT_START_STR, TEXT_START_LEN + 1);
					i->textBottom.pos.y = Y_POS_CENTER(DUDE_ROWS) + DUDE_ROWS + 3;
					i->textBottom.pos.x = X_POS_CENTER(TEXT_START_LEN);
					i->textBottom.len = TEXT_START_LEN;
					break;
				case 1000:
					/* Center to tree big text */
					i->shownCenter = SHOWN_MAX;
					strncpy(i->center.str, BIGTEXT_TREE_STR, BIGTEXT_TREE_SIZE);
					i->center.pos.x = X_POS_CENTER(BIGTEXT_TREE_COLS);
					i->center.size.cols = BIGTEXT_TREE_COLS;
					break;
				case 1500:
					/* Center to hidden dude big text */
					i->shownCenter = SHOWN_MIN;
					strncpy(i->center.str, BIGTEXT_DUDE_STR, BIGTEXT_DUDE_SIZE);
					i->center.pos.x = X_POS_CENTER(BIGTEXT_DUDE_COLS);
					i->center.size.cols = BIGTEXT_DUDE_COLS;
					break;
				case 2000:
					/* Show dude big text */
					i->shownCenter = SHOWN_MAX;
					break;
				case 2250:
					/* Center to hidden dude */
					i->shownCenter = SHOWN_MIN;
					strncpy(i->center.str, DUDE_STILL_L_STR, DUDE_SIZE);
					i->center.pos.y = Y_POS_CENTER(DUDE_ROWS) - 1;
					i->center.pos.x = X_POS_CENTER(DUDE_COLS);
					i->center.size.rows = DUDE_ROWS;
					i->center.size.cols = DUDE_COLS;
					break;
				case 2500:
					/* Show dude */
					i->shownCenter = SHOWN_MAX;
					break;
				case 2750:
					/* Center to hidden tree big text */
					i->shownCenter = SHOWN_MIN;
					strncpy(i->center.str, BIGTEXT_TREE_STR, BIGTEXT_TREE_SIZE);
					i->center.pos.y = Y_POS_CENTER(BIGTEXT_ROWS);
					i->center.pos.x = X_POS_CENTER(BIGTEXT_TREE_COLS);
					i->center.size.rows = BIGTEXT_ROWS;
					i->center.size.cols = BIGTEXT_TREE_COLS;
					break;
				case 3000:
					/* Show tree big text */
					i->shownCenter = SHOWN_MAX;
					break;
				case 3500:
					/* Center to hidden dude big text */
					i->shownCenter = SHOWN_MIN;
					strncpy(i->center.str, BIGTEXT_DUDE_STR, BIGTEXT_DUDE_SIZE);
					i->center.pos.x = X_POS_CENTER(BIGTEXT_DUDE_COLS);
					i->center.size.cols = BIGTEXT_DUDE_COLS;
					break;
				case 4000:
				case 4500:
					/* Show dude big text */
					i->shownCenter = SHOWN_MAX;
					break;
				case 4250:
				case 4750:
					/* Hide dude big text */
					i->shownCenter = SHOWN_MIN;
					break;
				case 5000:
					/* Finish stage */
					progressIntroStage(i);
					break;
				default: break;
			}

			/* Fade in start text */
			if (i->loops - i->loopStart >= 0 && i->shownBottom < SHOWN_MAX) {
				i->shownBottom += FADE_SPEED;
				if (i->shownBottom >= SHOWN_MAX)
					i->shownBottom = SHOWN_MAX;
			}

			break;
	}

	i->loops++;
}

void drawIntro(const window_t win, const intro_t i)
{
	drawSprite(win, i.center, (i.shown < i.shownCenter) ? i.shown : i.shownCenter, FLAG_DRAW_BOLD + ((i.stage == TITLE) ? FLAG_DRAW_ACS : FLAG_DRAW_NONE));
	drawText(win, i.textTop, (i.shown < i.shownTop) ? i.shown : i.shownTop, FLAG_DRAW_BOLD);
	drawText(win, i.textBottom, (i.shown < i.shownBottom) ? i.shown : i.shownBottom, FLAG_DRAW_BOLD);
}
