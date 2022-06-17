#include "common.h"

#define HIGH_SCORE_PATH_DIR "/treedude"
#define HIGH_SCORE_PATH_FILE "/score"
#define HIGH_SCORE_PATH "/treedude/score"
#define HIGH_SCORE_PATH_DIR_PERMISSIONS S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
#define HIGH_SCORE_FILE_FORMAT "%lx"

#define FADE_RAND_CHANCE 15
#define FADE_RAND_CHARS "`#-"
#define FADE_RAND_CHARS_LEN strlen(FADE_RAND_CHARS)
#define COLS_EXCEEDED (x + cols - winSize.cols)

#define TEXT_SCREEN_VERSION_LEN strlen(VERSION)
#define TEXT_SCREEN_TOP_1_STR "treedude v"
#define TEXT_SCREEN_TOP_1_LEN strlen(TEXT_SCREEN_TOP_1_STR)
#define TEXT_SCREEN_TOP_2_STR " by nivkovic"
#define TEXT_SCREEN_TOP_2_LEN strlen(TEXT_SCREEN_TOP_2_STR)
#define TEXT_SCREEN_BOTTOM_STR "press Q to quit"
#define TEXT_SCREEN_BOTTOM_LEN strlen(TEXT_SCREEN_BOTTOM_STR)
#define TEXT_SCREEN_X_OFFSET 3

static struct timespec getSysTime(void)
{
	struct timespec sysTime;

#ifdef CLOCK_MONOTONIC
	clock_gettime(CLOCK_MONOTONIC, &sysTime);
#else
	clock_gettime(CLOCK_REALTIME, &sysTime);
#endif

	return sysTime;
}

long getEpochMs(void)
{
	struct timespec time;

	time = getSysTime();
	return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

void initRandSeed(void)
{
	srand(getSysTime().tv_nsec / 1000000);
}

int randInt(int min, int max)
{
	return min + rand() % ((max + 1) - min);
}

int roundToInt(float val)
{
	return (val >= floor(val) + 0.5f) ? (int)ceil(val) : (int)floor(val);
}

static attr_t attrFromFlags(flag_t flags)
{
	if (flags & FLAG_DRAW_DIM)
		return A_DIM;
	else if (flags & FLAG_DRAW_BOLD)
		return A_BOLD;
	else
		return A_NORMAL;
}

void initDisplay(void)
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(0);
}

void freeDisplay(void)
{
	endwin();
}

void initWindow(window_t *win, const window_t *parWin, const coord_t y, const coord_t x, const dimention_t rows, const dimention_t cols)
{
	window_t newWin;
	newWin.win = (parWin == NULL) ? newwin(rows, cols, y, x) : derwin(parWin->win, rows, cols, y, x);
	newWin.size.rows = rows;
	newWin.size.cols = cols;
	newWin.pos.y = y;
	newWin.pos.x = x;
	*win = newWin;
}

void updateWindow(const window_t win, const flag_t flags)
{
	attr_t attr = attrFromFlags(flags);

	wattron(win.win, attr);
	box(win.win, 0, 0);
	wattroff(win.win, attr);
	wrefresh(win.win);
}

void clearWindow(const window_t win)
{
	wclear(win.win);
}

void freeWindow(window_t *win)
{
	clearWindow(*win);
	delwin(win->win);
}

static void drawObject(WINDOW *win, const dimentions_t winSize, const char *str, const dimention_t rows, const dimention_t cols, const coord_t y, const coord_t x, const shown_t shown, const flag_t flags)
{
	char strChar;
	dimention_t strRow, strCol;
	attr_t attr;

	/* Return if will not be visible if drawn */
	if (shown <= SHOWN_MIN || cols == 0 || y + (coord_t)rows < 0 || x + (coord_t)cols < 0 || x > (coord_t)winSize.cols)
		return;

	attr = attrFromFlags(flags);
	wattron(win, attr);

	/* Draw each row within window */
	for (strRow = 0; strRow < rows && y + strRow <= winSize.rows; strRow++) {
		/* Draw whole row */
		if (flags & FLAG_DRAW_NONE && shown >= SHOWN_MAX) {
			mvwaddnstr(win, y + strRow, x, &str[cols * strRow], cols - ((COLS_EXCEEDED > 0) ? COLS_EXCEEDED : 0));
		/* Draw each char of row */
		} else {
			for (strCol = 0; strCol < cols && x + strCol <= winSize.cols; strCol++) {
				strChar = str[cols * strRow + strCol];
				/* Next char if cannot draw char */
				if ((flags & FLAG_DRAW_SKIP_SPACES && strChar == ' ') || shown < randInt(SHOWN_MIN, SHOWN_MAX))
					continue;
				/* Change char to random fade char */
				if (shown < SHOWN_MAX && strChar != ' ' && randInt(0, 100) < FADE_RAND_CHANCE)
					strChar = FADE_RAND_CHARS[randInt(0, FADE_RAND_CHARS_LEN - 1)];
				mvwaddch(win, y + strRow, x + strCol, (flags & FLAG_DRAW_ACS && strChar == '`') ? ACS_BLOCK : (chtype)strChar);
			}
		}
	}

	wattroff(win, attr);
}

void drawString(const window_t win, const char* str, const dimention_t len, const coord_t y, const coord_t x, const shown_t shown, const flag_t flags)
{
	drawObject(win.win, win.size, str, 1, len, y, x, shown, flags);
}

void drawText(const window_t win, const text_t text, const shown_t shown, const flag_t flags)
{
	drawString(win, text.str, text.len, text.pos.y, text.pos.x, shown, flags);
}

void drawSprite(const window_t win, const sprite_t spr, const shown_t shown, const flag_t flags)
{
	drawObject(win.win, win.size, spr.str, spr.size.rows, spr.size.cols, spr.pos.y, spr.pos.x, shown, flags);
}

void initScreen(screen_t *screen, const dimention_t rows, const dimention_t cols)
{
	screen->size.rows = rows;
	screen->size.cols = cols;
	initWindow(&screen->win, NULL, roundToInt((float)(rows - WIN_ROWS) / 2.0f), roundToInt((float)(cols - WIN_COLS) / 2.0f), WIN_ROWS, WIN_COLS);
}

void updateScreen(screen_t *screen)
{
	updateWindow(screen->win, FLAG_DRAW_DIM);
	drawObject(stdscr, screen->size, TEXT_SCREEN_TOP_1_STR, 1, TEXT_SCREEN_TOP_1_LEN, screen->win.pos.y, screen->win.pos.x + TEXT_SCREEN_X_OFFSET, SHOWN_MAX, FLAG_DRAW_BOLD);
	drawObject(stdscr, screen->size, VERSION, 1, TEXT_SCREEN_VERSION_LEN, screen->win.pos.y, screen->win.pos.x + TEXT_SCREEN_X_OFFSET + TEXT_SCREEN_TOP_1_LEN, SHOWN_MAX, FLAG_DRAW_BOLD);
	drawObject(stdscr, screen->size, TEXT_SCREEN_TOP_2_STR, 1, TEXT_SCREEN_TOP_2_LEN, screen->win.pos.y, screen->win.pos.x + TEXT_SCREEN_X_OFFSET + TEXT_SCREEN_TOP_1_LEN + TEXT_SCREEN_VERSION_LEN, SHOWN_MAX, FLAG_DRAW_BOLD);
	drawObject(stdscr, screen->size, TEXT_SCREEN_BOTTOM_STR, 1, TEXT_SCREEN_BOTTOM_LEN, screen->win.pos.y + WIN_ROWS - 1, screen->win.pos.x + WIN_COLS - TEXT_SCREEN_BOTTOM_LEN - TEXT_SCREEN_X_OFFSET, SHOWN_MAX, FLAG_DRAW_NONE);
	refresh();
}

void freeScreen(screen_t *screen)
{
	clear();
	freeWindow(&screen->win);
}

bool_t updateFadeMulti(fade_t *fade, shown_t *shown, const float multi)
{
	/* Exit if done */
	if ((*fade == IN_DONE && *shown >= SHOWN_MAX) || (*fade == OUT_DONE && *shown <= SHOWN_MIN))
		return FALSE_E;

	/* Update shown */
	*shown += FADE_SPEED * (shown_t)(((*fade == OUT) ? -1.0f : 1.0f) * multi);

	/* Done */
	if (*fade == IN && *shown >= SHOWN_MAX) {
		*fade = IN_DONE;
		*shown = SHOWN_MAX;
	} else if (*fade == OUT && *shown <= SHOWN_MIN) {
		*fade = OUT_DONE;
		*shown = SHOWN_MIN;
	}

	return TRUE_E;
}

bool_t updateFade(fade_t *fade, shown_t *shown)
{
	return updateFadeMulti(fade, shown, 1.0f);
}

static bool_t dirExists(const char *path)
{
	struct stat stats;

	if (stat(path, &stats) != 0)
		return FALSE_E;

	if (S_ISDIR(stats.st_mode) == 0)
		return FALSE_E;

	return TRUE_E;
}

static char *getXdgDataHomePath(void)
{
	static char path[PATH_MAX];
	char *envVar = NULL;

	/* Get $XDG_DATA_HOME */
	NULL_STRING(path);
	envVar = getenv("XDG_DATA_HOME");
	if (envVar) {
		strcpy(path, envVar);
	/* Fall back to $HOME/.local/share */
	} else {
		envVar = getenv("HOME");
		if (!envVar)
			return path;
		strcpy(path, envVar);
		strcat(path, "/.local/share");
	}

	return path;
}

score_t loadHighScore(void)
{
	char *path;
	FILE *fileStream;
	score_t highScore = 0;

	/* Get file path */
	path = getXdgDataHomePath();
	if (!dirExists(path))
		return highScore;
	strcat(path, HIGH_SCORE_PATH);

	/* Read file */
	fileStream = fopen(path, "r");
	if (fileStream) {
		fscanf(fileStream, HIGH_SCORE_FILE_FORMAT, &highScore);
		fclose(fileStream);
	}

	return highScore;
}

void saveHighScore(score_t highScore)
{
	char *path;
	FILE *fileStream;

	/* Get high score file dir */
	path = getXdgDataHomePath();
	strcat(path, HIGH_SCORE_PATH_DIR);

	/* Ensure dir exist, ensure dir is created if it does not exist */
	if (!dirExists(path) && mkdir(path, HIGH_SCORE_PATH_DIR_PERMISSIONS) != 0)
		return;

	/* Write high score to file */
	strcat(path, HIGH_SCORE_PATH_FILE);
	fileStream = fopen(path, "w");
	if (fileStream) {
		fprintf(fileStream, HIGH_SCORE_FILE_FORMAT, highScore);
		fclose(fileStream);
	}
}
