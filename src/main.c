#define _XOPEN_SOURCE 500 /* SUSv2 (inc. C89/90, POSIX.1-1990, POSIX.2-1992, POSIX.1b-1993, POSIX.1c-1995) */

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
	long startEpochMs = 0, timerMs = 0;
	flag_t mainFlags = FLAG_MAIN_NONE;
	input_t input;
	stage_t stage = INTRO;
	bool_t draw = TRUE_E;
	bool_t screenResized = FALSE_E;
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
				mainFlags |= FLAG_MAIN_NOHIGHSCOREFILE;
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

	if (!(mainFlags & FLAG_MAIN_NOHIGHSCOREFILE))
		highScore = loadHighScore();

	while (stage != QUIT) { 
		/* Start loop timer */
		startEpochMs = getEpochMs();

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
		screenResized = FALSE_E;
		if (LINES != screen.size.rows || COLS != screen.size.cols) {
			screenResized = TRUE_E;
			freeScreen(&screen);
			initScreen(&screen, LINES, COLS);
			resizeGame(&game, screen);
		}

		/* Process stages */
		switch (stage) {
			case INTRO:
				draw = updateIntro(&intro, &stage, input);
				if (draw || screenResized) {
					clearWindow(screen.win);
					drawIntro(screen.win, intro);
				}
				break;
			case GAME:
				draw = updateGame(&game, &stage, &score, screen, input);
				if (draw || screenResized) {
					clearWindow(screen.win);
					drawGame(screen.win, game, stage);
				}
				break;
			case END:
				draw = updateEnd(&end, &stage, &game.shown, &highScore, score, input, mainFlags);
				if (draw || screenResized) {
					clearWindow(screen.win);
					drawGame(screen.win, game, stage);
					drawEnd(screen.win, end);
				}
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
		if (draw || screenResized)
			updateScreen(&screen);

		/* End loop timer and sleep */
		timerMs = getEpochMs() - startEpochMs;
		if (timerMs < MS_PER_LOOP)
			sleepMs(MS_PER_LOOP - timerMs);
	}

	cleanExit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
