#define _XOPEN_SOURCE 500 /* C89 + POSIX.1-1990 + POSIX.2-1992 + SUSv2 */

#include "intro.h"
#include "game.h"
#include "end.h"
#include <signal.h>

/* Data that needs to be freed on exit */
screen_t screen;
game_t game;

static void cleanExit(int signal)
{
	freeGame(&game);
	freeScreen(&screen);
	freeDisplay();

	exit(signal);
}

int main(int argc, char *argv[])
{
	char argOpt;
	struct timespec sysTime;
	long epochStartMs = 0, epochEndMs = 0, timerMs = 0;
	flag_t flags = FLAG_MAIN_NONE;
	input_t input;
	stage_t stage = INTRO;
	score_t score = 0, highScore = 0;
	intro_t intro;
	end_t end;

	/* Signal handlers */
	signal(SIGQUIT, cleanExit);
	signal(SIGINT, cleanExit);
	signal(SIGTERM, cleanExit);

	/* Handle args */
	while ((argOpt = getopt(argc, argv, ":nvV")) != -1)
		switch (argOpt) {
			case 'v':
			case 'V':
				printf("treedude %s%s", VERSION, NEW_LINE);
				cleanExit(EXIT_SUCCESS);
				break;
			case 'n':
				flags += FLAG_MAIN_NOHIGHSCOREFILE;
				break;
			case '?':
			default:
				fprintf(stderr, "Invalid option '%c'%s", optopt, NEW_LINE);
				fprintf(stderr, "Usage: treedude [-nnV]%s", NEW_LINE);
				cleanExit(EXIT_FAILURE);
				break;
		}

	/* Initialise */
	initRandSeed();
	initDisplay();
	initScreen(&screen, LINES, COLS);
	initIntro(&intro);
	initGame(&game, screen);
	initEnd(&end);

	if (!(flags & FLAG_MAIN_NOHIGHSCOREFILE))
		highScore = loadHighScore();

	while (stage != QUIT) { 
		/* Start loop timer */
		clock_gettime(CLOCK_MONOTONIC, &sysTime);
		epochStartMs = sysTime.tv_sec * 1000 + sysTime.tv_nsec / 1000000;

		input = getch();

		/* Catch quit */
		switch (input) {
			case 'q':
			case 'Q':
			case 27: /* Esc */
				stage = QUIT;
				break;
		}
		if (stage == QUIT)
			break;

		/* Term resize */
		if (LINES != (int)screen.size.rows || COLS != (int)screen.size.cols) {
			freeScreen(&screen);
			initScreen(&screen, LINES, COLS);
			resizeGame(&game, screen);
		}

		/* Process stages */
		switch (stage) {
			case INTRO:
				updateIntro(&intro, &stage, input);
				clearWindow(screen.win);
				drawIntro(screen.win, intro);
				break;
			case GAME:
				updateGame(&game, &stage, &score, screen, input);
				clearWindow(screen.win);
				drawGame(screen.win, game, stage);
				break;
			case END:
				updateEnd(&end, &stage, &game.shown, &highScore, score, input, flags);
				clearWindow(screen.win);
				drawGame(screen.win, game, stage);
				drawEnd(screen.win, end);
				break;
			case RESET:
				freeGame(&game);
				clearWindow(screen.win);
				score = 0;
				initRandSeed();
				initGame(&game, screen);
				initEnd(&end);
				stage = GAME;
				break;
			default: break;
		}

		/* Update main screen */
		updateScreen(&screen);

		/* End loop timer and sleep */
		clock_gettime(CLOCK_MONOTONIC, &sysTime);
		epochEndMs = sysTime.tv_sec * 1000 + sysTime.tv_nsec / 1000000;
		timerMs = epochEndMs - epochStartMs;
		if (timerMs < MS_PER_LOOP)
			usleep(1000 * (MS_PER_LOOP - timerMs));
	}

	cleanExit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
