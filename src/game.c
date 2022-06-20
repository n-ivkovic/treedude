#include "game.h"

#define WIN_DER_ROWS 3
#define WIN_TIMER_Y 3
#define WIN_SCORE_X_PADDING 2

#define TREE_CHUNKS 4
#define TREE_FALL_SPEED (LOOPS_PER_SEC / 8)
#define TREE_CHUNK_ROWS 6
#define TREE_CHUNK_COLS 13
#define TREE_CHUNK_SIZE (TREE_CHUNK_ROWS * TREE_CHUNK_COLS + 1)
#define TREE_TRUNK_1_STR "|   {  /    || }   /     }{       }   ||        {  }| }    { \\  |{  |     /  }"
#define TREE_TRUNK_2_STR "|           ||    {   |  ||        /  }{ } }   }   || | }    }  |{           }"
#define TREE_BRANCH_L_STR "     o  o       o0Oo  \\     O0Oo  --\\    oOOOo  /->-    o0O-/                 "
#define TREE_BRANCH_R_STR "     oOo          / oO       /--00oO   --<   OOo       \\oOO                   "
#define TREE_CHUNK_X roundToInt((float)(WIN_COLS - TREE_CHUNK_COLS) / 2.0f)
#define TREE_BASE_ROW (WIN_ROWS - TREE_CHUNK_ROWS - 1)

#define TIMER_MAX 21
#define TIMER_INIT 11
#define TIMER_WARN 5
#define TIMER_CHOP_DIR (TIMER_INIT - 2)
#define TIMER_DECAY_SPEED_MIN (TREE_FALL_SPEED + (LOOPS_PER_SEC / 11))

#define LEVEL_UP_SCORE_MULTI (10 * g->level)
#define DUDE_CHOP_X_PADDING 2

#define TEXT_CHOP_STR "CHOP!"
#define TEXT_CHOP_LEN strlen(TEXT_CHOP_STR)
#define TEXT_CHOP_L_STR "CHOP-LEFT-->"
#define TEXT_CHOP_L_LEN strlen(TEXT_CHOP_L_STR)
#define TEXT_CHOP_R_STR "<--CHOP-RIGHT"
#define TEXT_CHOP_R_LEN strlen(TEXT_CHOP_R_STR)
#define TEXT_CHOP_DIR_Y (WIN_ROWS - DUDE_ROWS - 3)
#define TEXT_CHOP_DIR_PADDING 1
#define TEXT_WATCHTIME_STR "WATCH TIME!"
#define TEXT_WATCHTIME_LEN strlen(TEXT_WATCHTIME_STR)

#define BIGTEXT_FASTER_STR "````  ``   ``` `````` ```` ```` `    `  ` `      ``   `    `   ````  ````  ```   ``   ```  ```` `    `  `     `  ``   `    ` `  `    `  ` ````   ``   ```` `  ``"
#define BIGTEXT_FASTER_COLS (strlen(BIGTEXT_FASTER_STR) / BIGTEXT_ROWS)
#define BIGTEXT_FASTER_SIZE (BIGTEXT_ROWS * BIGTEXT_FASTER_COLS + 1)
#define BIGTEXT_LEVELUP_STR "`    ```` `     ` ```` `       `   ` ```` `    `    `     ` `    `       `   ` `   ``    ```   `   `  ```  `       `   ` ```` `    `      ` `   `    `       `   ` `    ```` ````    `    ```` ````     ```  `    "
#define BIGTEXT_LEVELUP_COLS (strlen(BIGTEXT_LEVELUP_STR) / BIGTEXT_ROWS)
#define BIGTEXT_LEVELUP_SIZE (BIGTEXT_ROWS * BIGTEXT_LEVELUP_COLS + 1)
#define BIGTEXT_SPEEDUP_STR " ```  ````  ```` ```` ```      `   ` ```` `     `   ` `    `    `  `     `   ` `   ` ```  ````  ```  ```  `  `     `   ` ````     ` `     `    `    `  `     `   ` `    ````  `     ```` ```` ```       ```  `    "
#define BIGTEXT_SPEEDUP_COLS (strlen(BIGTEXT_SPEEDUP_STR) / BIGTEXT_ROWS)
#define BIGTEXT_SPEEDUP_SIZE (BIGTEXT_ROWS * BIGTEXT_SPEEDUP_COLS + 1)
#define BIGTEXT_TICKTOCK_STR "`````` ```  ``` `  `    `````` ```   ``` `  `  ``    `  `    ` `       ``  `   ` `    ` `   ``    `  `    ``        ``  `   ` `    ``    ``    `  `    ` `       ``  `   ` `    ` `   ``   ```  ``` `  `      ``   ```   ``` `  `"
#define BIGTEXT_TICKTOCK_COLS (strlen(BIGTEXT_TICKTOCK_STR) / BIGTEXT_ROWS)
#define BIGTEXT_TICKTOCK_SIZE (BIGTEXT_ROWS * BIGTEXT_TICKTOCK_COLS + 1)
#define BIGTEXT_CHOPCHOP_STR " ``` `  `  ```  ````       ``` `  `  ```  ```` `    `  ` `   ` `   `     `    `  ` `   ` `   ``    ```` `   ` ````      `    ```` `   ` ```` `    `  ` `   ` `         `    `  ` `   ` `     ``` `  `  ```  `          ``` `  `  ```  `    "
#define BIGTEXT_CHOPCHOP_COLS (strlen(BIGTEXT_CHOPCHOP_STR) / BIGTEXT_ROWS)
#define BIGTEXT_CHOPCHOP_SIZE (BIGTEXT_ROWS * BIGTEXT_CHOPCHOP_COLS + 1)

static void initTree(tree_t *tree)
{
	tree->base = NULL;
	tree->top = NULL;
}

static void pushTree(tree_t *tree, treeChunk_t *chunk)
{
	if (!tree->base)
		tree->base = chunk;
	if (tree->top)
		tree->top->next = chunk;
	tree->top = chunk;
}

static treeChunk_t *popTree(tree_t *tree)
{
	treeChunk_t *result = tree->base;
	
	if (tree->base)
		tree->base = tree->base->next;
	if (!tree->base)
		tree->top = NULL;
	if (result)
		result->next = NULL;

	return result;
}

static void growTree(tree_t *tree)
{
	treeChunk_t *chunk = malloc(sizeof(treeChunk_t));
	side_t randSide = (side_t)randInt(0, 2);

	chunk->next = NULL;
	chunk->shown = SHOWN_MAX;
	chunk->pos.y = (tree->top) ? tree->top->pos.y - TREE_CHUNK_ROWS : TREE_BASE_ROW;
	chunk->pos.x = TREE_CHUNK_X;
	/* No side if the player must move, otherwise random */	
	chunk->side = (tree->top && ((randSide == LEFT && tree->top->side == RIGHT) || (randSide == RIGHT && tree->top->side == LEFT))) ? NONE : randSide;

	pushTree(tree, chunk);
}

static void freeTree(tree_t *tree)
{
	while (tree->base)
		free(popTree(tree));
}

static void initTimerWindow(const screen_t screen, gameWindow_t *winTimer)
{
	initWindow(&winTimer->win, &screen.win, WIN_TIMER_Y, (WIN_COLS - TIMER_MAX) / 2, WIN_DER_ROWS, TIMER_MAX + 2);
}

static void initScoreWindow(const screen_t screen, gameWindow_t *winScore)
{
	initWindow(&winScore->win, &screen.win, WIN_TIMER_Y + WIN_DER_ROWS - 1, (WIN_COLS - ((winScore->text.str[0] == '\0') ? 1 : winScore->text.len) - 2) / 2, WIN_DER_ROWS, winScore->text.len + WIN_SCORE_X_PADDING + 2);
}

static void updateTimer(const int timer, text_t *timerText)
{
	size_t ind;

	for (ind = 0; ind < TIMER_MAX; ind++)
		timerText->str[ind] = ((int)ind < timer) ? '`' : '-';
	timerText->str[ind] = '\0';
}

static void updateScore(const score_t score, const screen_t screen, gameWindow_t *winScore)
{
	size_t ind;

	sprintf(winScore->text.str, "%ld", score);
	winScore->text.len = (dimention_t)strlen(winScore->text.str);

	/* Add spacing to centre of score string */
	if (winScore->text.len > 0 && winScore->text.len % 2 == 0) {
		for (ind = winScore->text.len; ind > winScore->text.len / 2; ind--)
			winScore->text.str[ind] = winScore->text.str[ind - 1];
		winScore->text.str[ind] = ' ';
		winScore->text.len++;
	}

	/* Resize score window */
	if (winScore->text.len != winScore->win.size.cols) {
		freeWindow(&winScore->win);
		initScoreWindow(screen, winScore);
	}
}

void resizeGame(game_t *g, const screen_t screen)
{
	freeWindow(&g->winTimer.win);
	freeWindow(&g->winScore.win);
	initTimerWindow(screen, &g->winTimer);
	initScoreWindow(screen, &g->winScore);
}

void initGame(game_t *g, const screen_t screen)
{
	size_t ind;

	g->state = STILL;
	g->shown = SHOWN_MIN;
	g->fade = IN;

	g->loops = 0;
	g->loopLevelBigText = LOOPS_MAX;
	g->loopDead = LOOPS_MAX;

	g->dudeSide = LEFT;
	g->level = 1;
	g->levelUpScore = LEVEL_UP_SCORE_MULTI;
	g->timerDecaySpeed = (LOOPS_PER_SEC / 4) * 3;
	g->timer = TIMER_INIT;

	/* Trees */
	initTree(&g->tree);
	initTree(&g->chopped);

	/* Grow tree */
	for (ind = 1; ind <= TREE_CHUNKS; ind++) {
		growTree(&g->tree);
		if (ind <= 2)
			g->tree.top->side = NONE;
	}

	/* Misc text */
	g->chopTextOffsetY = 0;
	g->showChopDir = FALSE_E;
	g->showWatchTime = FALSE_E;

	/* Level up big text */
	NULL_STRING(g->levelBigText.str);
	g->levelBigText.size.rows = BIGTEXT_ROWS;
	g->levelBigText.size.cols = 0;
	g->levelBigText.pos.y = 3;
	g->levelBigText.pos.x = 0;
	g->levelBigTextShown = SHOWN_MIN;
	g->levelBigTextFade = OUT_DONE;

	/* Timer window */
	initTimerWindow(screen, &g->winTimer);
	g->winTimer.text.len = TIMER_MAX;
	g->winTimer.text.pos.y = 1;
	g->winTimer.text.pos.x = 1;
	updateTimer(g->timer, &g->winTimer.text);

	/* Score window */
	initScoreWindow(screen, &g->winScore);
	NULL_STRING(g->winScore.text.str);
	g->winScore.text.len = 0;
	g->winScore.text.pos.y = 1;
	g->winScore.text.pos.x = 2;
	updateScore(0, screen, &g->winScore);
}

static void dudeDie(game_t *g)
{
	g->state = DEAD;
	g->loopDead = g->loops + LOOPS_PER_SEC;
}

static void levelUp(game_t *g, const score_t score)
{
	g->level++;
	g->levelUpScore = score + LEVEL_UP_SCORE_MULTI + 1;

	/* Increase timer decay speed */
	if (g->timerDecaySpeed > TIMER_DECAY_SPEED_MIN) {
		g->timerDecaySpeed -= (LOOPS_PER_SEC / 5);
		if (g->timerDecaySpeed < TIMER_DECAY_SPEED_MIN)
			g->timerDecaySpeed = TIMER_DECAY_SPEED_MIN;
	}

	/* Set level up text */
	g->levelBigTextShown = SHOWN_MIN + ((SHOWN_MAX - SHOWN_MIN) / 2); /* 50% shown */
	g->levelBigTextFade = IN;
	switch (randInt(1,5)) {
		case 1:
			strncpy(g->levelBigText.str, BIGTEXT_FASTER_STR, BIGTEXT_FASTER_SIZE);
			g->levelBigText.size.cols = BIGTEXT_FASTER_COLS;
			break;
		case 2:
			strncpy(g->levelBigText.str, BIGTEXT_LEVELUP_STR, BIGTEXT_LEVELUP_SIZE);
			g->levelBigText.size.cols = BIGTEXT_LEVELUP_COLS;
			break;
		case 3:
			strncpy(g->levelBigText.str, BIGTEXT_SPEEDUP_STR, BIGTEXT_SPEEDUP_SIZE);
			g->levelBigText.size.cols = BIGTEXT_SPEEDUP_COLS;
			break;
		case 4:
			strncpy(g->levelBigText.str, BIGTEXT_TICKTOCK_STR, BIGTEXT_TICKTOCK_SIZE);
			g->levelBigText.size.cols = BIGTEXT_TICKTOCK_COLS;
			break;
		case 5:
			strncpy(g->levelBigText.str, BIGTEXT_CHOPCHOP_STR, BIGTEXT_CHOPCHOP_SIZE);
			g->levelBigText.size.cols = BIGTEXT_CHOPCHOP_COLS;
		default: break;
	}
	g->levelBigText.pos.x = X_POS_CENTER(g->levelBigText.size.cols);
}

bool_t updateGame(game_t *g, stage_t *stage, score_t *score, const screen_t screen, const input_t inp)
{
	bool_t draw = g->state == CHOPPED;
	treeChunk_t *chunk;

	/* Update fade */
	if (updateFade(&g->fade, &g->shown)) 
		return TRUE_E;

	/* End after dead for 1s */	
	if (g->state == DEAD && g->loops >= g->loopDead) {
		(*stage) = END;
		return FALSE_E;
	}

	/* Process input */
	if (g->fade == IN_DONE && g->state == STILL)
		switch (inp) {
			case KEY_LEFT:
			case 'a':
			case 'A':
			case 'h':
			case 'H':
				g->dudeSide = LEFT;
				g->state = CHOP;
				break;
			case KEY_RIGHT:
			case 'd':
			case 'D':
			case 'l':
			case 'L':
				g->dudeSide = RIGHT;
				g->state = CHOP;
				break;
		}
	
	/* Chop tree */
	if (g->state == CHOP) {
		/* Set chop text row */
		g->chopTextOffsetY = randInt(0, 3);
		/* Hide chop direction text */
		g->showChopDir = FALSE_E;
		
		/* Side collision */
		if (g->dudeSide == g->tree.base->side) {
			dudeDie(g);
			draw = TRUE_E;
		/* Chop tree base */
		} else {
			/* Move tree base to top of chopped chunks */
			pushTree(&g->chopped, popTree(&g->tree));
			growTree(&g->tree);
			/* Set chopped chunk side */
			g->chopped.top->pos.x += ((g->dudeSide == RIGHT) ? -1 : 1) * (TREE_CHUNK_COLS / 2);
		}

		/* Above collision */
		if (g->dudeSide == g->tree.base->side) {
			dudeDie(g);
			draw = TRUE_E;
		}
	}

	/* Successful chop */
	if (g->state == CHOP) {
		g->state = CHOPPED;

		/* Increment timer */
		if (g->timer < TIMER_MAX) {
			g->timer++;
			updateTimer(g->timer, &g->winTimer.text);
			if (g->timer > TIMER_WARN)
				g->showWatchTime = FALSE_E;
		}

		/* Level up */
		if (*score == g->levelUpScore)
			levelUp(g, *score);

		/* Increment score */
		(*score)++;
		updateScore(*score, screen, &g->winScore);
	}

	/* Move tree */
	if (g->state != STILL && g->tree.base->pos.y < TREE_BASE_ROW) {
		chunk = g->tree.base;
		while (chunk) {
			chunk->pos.y += TREE_CHUNK_ROWS / TREE_FALL_SPEED;
			chunk = chunk->next;
		}

		/* Finish chop */
		if (g->tree.base->pos.y >= TREE_BASE_ROW && g->state != DEAD)
			g->state = STILL;
		draw = TRUE_E;
	}

	/* Update chopped tree chunks */
	chunk = g->chopped.base;
	while (chunk) {
		chunk->pos.x += ((chunk->pos.x < TREE_CHUNK_X) ? -1 : 1) * 2;
		chunk->shown -= SHOWN_MAX / (LOOPS_PER_SEC / 5);
		chunk = chunk->next;
		draw = TRUE_E;
	}

	/* Remove hidden chopped tree chunks */
	while (g->chopped.base && g->chopped.base->shown <= SHOWN_MIN) {
		free(popTree(&g->chopped));
		draw = TRUE_E;
	}

	/* Do not update additional items if dead */
	if (g->state == DEAD) {
		g->loops++;
		return draw;
	}

	/* Decrement timer */
	if (g->loops % g->timerDecaySpeed == 0) {
		g->timer--;
		updateTimer(g->timer, &g->winTimer.text);
		if (g->timer == 0)
			dudeDie(g);
		draw = TRUE_E;
	}

	/* Update level text */
	if (g->levelBigTextFade != OUT_DONE) {
		if (g->loops == g->loopLevelBigText && g->levelBigTextFade >= IN_DONE)
			g->levelBigTextFade = OUT;
		if (updateFadeMulti(&g->levelBigTextFade, &g->levelBigTextShown, 2.0f)) {
			if (g->levelBigTextFade == IN_DONE)
				g->loopLevelBigText = g->loops + LOOPS_PER_SEC;
			draw = TRUE_E;
		}
	}

	/* Flash chop directions text every 0.5s */
	if ((*score) == 0 && g->state == STILL && g->timer <= TIMER_CHOP_DIR && g->loops > 0 && g->loops % (LOOPS_PER_SEC / 2) == 0) {
		g->showChopDir = REVERSE(g->showChopDir, TRUE_E, FALSE_E);
		draw = TRUE_E;
	}

	/* Flash watch time text every 0.25s */
	if (g->timer <= TIMER_WARN && g->loops % (LOOPS_PER_SEC / 4) == 0) {
		g->showWatchTime = REVERSE(g->showWatchTime, TRUE_E, FALSE_E);
		draw = TRUE_E;
	}

	g->loops++;
	return draw;
}

static void drawGameWindow(const gameWindow_t gameWin, const flag_t flags)
{
	clearWindow(gameWin.win);
	drawText(gameWin.win, gameWin.text, SHOWN_MAX, flags);
	updateWindow(gameWin.win, FLAG_DRAW_NONE);
}

static void drawTree(const window_t win, const tree_t tree, const shown_t shown, const flag_t flags)
{
	treeChunk_t *chunk = tree.base;
	shown_t chunkShown;
	sprite_t treeTrunk;
	sprite_t treeBranch;

	treeTrunk.size.rows = TREE_CHUNK_ROWS;
	treeTrunk.size.cols = TREE_CHUNK_COLS;
	treeBranch.size = treeTrunk.size;

	while (chunk) {
		chunkShown = (shown < chunk->shown) ? shown : chunk->shown;
		treeTrunk.pos = chunk->pos;
		treeBranch.pos = treeTrunk.pos;

		switch (chunk->side) {
			case NONE:
				strncpy(treeTrunk.str, TREE_TRUNK_1_STR, TREE_CHUNK_SIZE);
				break;
			case LEFT:
				strncpy(treeTrunk.str, TREE_TRUNK_2_STR, TREE_CHUNK_SIZE);
				strncpy(treeBranch.str, TREE_BRANCH_L_STR, TREE_CHUNK_SIZE);
				treeBranch.pos.x -= TREE_CHUNK_COLS;
				break;
			case RIGHT:
				strncpy(treeTrunk.str, TREE_TRUNK_2_STR, TREE_CHUNK_SIZE);
				strncpy(treeBranch.str, TREE_BRANCH_R_STR, TREE_CHUNK_SIZE);
				treeBranch.pos.x += TREE_CHUNK_COLS;
				break;
			default: break;
		}

		drawSprite(win, treeTrunk, chunkShown, flags);

		if (chunk->side != NONE)
			drawSprite(win, treeBranch, chunkShown, flags);

		chunk = chunk->next;
	}
}

static void drawDude(const window_t win, const side_t side, const gameState_t state, const shown_t shown, const flag_t flags)
{
	sprite_t dude;

	dude.pos.y = WIN_ROWS - DUDE_ROWS - ((state == CHOPPED) ? 2 : 1);
	dude.size.rows = DUDE_ROWS;
	dude.size.cols = DUDE_COLS;

	switch (side) {
		case LEFT:
			dude.pos.x = roundToInt((float)(WIN_COLS - TREE_CHUNK_COLS) / 2.0f) - DUDE_COLS;
			switch (state) {
				case STILL:
					strncpy(dude.str, DUDE_STILL_L_STR, DUDE_SIZE);
					break;
				case CHOPPED:
					strncpy(dude.str, DUDE_CHOP_L_STR, DUDE_SIZE);
					dude.pos.x += DUDE_CHOP_X_PADDING;
					break;
				default:
					strncpy(dude.str, DUDE_DEAD_STR, DUDE_SIZE);
					break;
			}
			break;
		case RIGHT:
			dude.pos.x = roundToInt((float)(WIN_COLS + TREE_CHUNK_COLS) / 2.0f);
			switch (state) {
				case STILL:
					strncpy(dude.str, DUDE_STILL_R_STR, DUDE_SIZE);
					break;
				case CHOPPED:
					strncpy(dude.str, DUDE_CHOP_R_STR, DUDE_SIZE);
					dude.pos.x -= DUDE_CHOP_X_PADDING;
					break;
				default:
					strncpy(dude.str, DUDE_DEAD_STR, DUDE_SIZE);
					break;
			}
			break;
		default: break;
	}

	drawSprite(win, dude, shown, flags + ((state == CHOPPED) ? FLAG_DRAW_SKIP_SPACES : FLAG_DRAW_NONE));
}

void drawGame(const window_t win, const game_t g, const stage_t stage)
{
	flag_t flags = (stage == GAME && g.state != DEAD) ? FLAG_DRAW_BOLD : FLAG_DRAW_NONE;

	drawTree(win, g.tree, g.shown, flags);
	drawTree(win, g.chopped, g.shown, flags);
	drawDude(win, g.dudeSide, g.state, g.shown, (stage == GAME) ? FLAG_DRAW_BOLD : flags);

	/* Do not draw additional items if player is dead */
	if (stage != GAME || g.state == DEAD)
		return;

	if (g.state == CHOPPED)
		drawString(win, TEXT_CHOP_STR, TEXT_CHOP_LEN, WIN_ROWS - 3 - g.chopTextOffsetY, X_POS_CENTER(TEXT_CHOP_LEN) + 1, g.shown, flags);

	if (g.showChopDir) {
		drawString(win, TEXT_CHOP_L_STR, TEXT_CHOP_L_LEN, TEXT_CHOP_DIR_Y, roundToInt((float)(WIN_COLS - TREE_CHUNK_COLS) / 2.0f) - TEXT_CHOP_DIR_PADDING - TEXT_CHOP_L_LEN, g.shown, flags);
		drawString(win, TEXT_CHOP_R_STR, TEXT_CHOP_R_LEN, TEXT_CHOP_DIR_Y, roundToInt((float)(WIN_COLS + TREE_CHUNK_COLS) / 2.0f) + TEXT_CHOP_DIR_PADDING, g.shown, flags);
	}

	drawSprite(win, g.levelBigText, g.levelBigTextShown, flags + FLAG_DRAW_SKIP_SPACES + FLAG_DRAW_ACS);

	/* Do not draw score and timer windows if the level up text is being shown */
	if (g.levelBigTextShown > 10)
		return;

	drawGameWindow(g.winTimer, FLAG_DRAW_ACS + ((g.timer == TIMER_MAX) ? FLAG_DRAW_BOLD : FLAG_DRAW_NONE));
	drawGameWindow(g.winScore, flags);

	if (g.showWatchTime)
		drawString(win, TEXT_WATCHTIME_STR, TEXT_WATCHTIME_LEN, WIN_TIMER_Y, X_POS_CENTER(TEXT_WATCHTIME_LEN) + 1, g.shown, FLAG_DRAW_BOLD);
}

void freeGame(game_t *g)
{
	freeWindow(&g->winTimer.win);
	freeWindow(&g->winScore.win);
	freeTree(&g->tree);
	freeTree(&g->chopped);
}
