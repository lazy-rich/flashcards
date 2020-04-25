/*
 * Copyright (c) 2020 Richard Bradshaw
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>

#include "media.h"
#include "set.h"
#include "card.h"

static void usage(void);

static void
usage()
{
	fprintf(stderr, "flashcards -s <setname>");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	int c;
	char *setpath;
	struct set *s;
	struct window *display;
	SDL_Event ev;
	struct cardset *cs;

	setpath = NULL;
	s = NULL;

	while ((c = getopt (argc, argv, "s:")) != -1) {
		switch (c) {
		case 's':
			setpath = optarg;
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (setpath == NULL)
		usage();

	s = load_set(setpath);
	if (s == NULL) {
		warnx("load_set failed with path: %s", setpath);
		exit(EXIT_FAILURE);
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		/* 1. images */
		warnx("SDL_Init failed");
		destroy_set(s);
		exit(EXIT_FAILURE);
	}
	display = make_window(800, 600, SDL_WINDOW_RESIZABLE);
	if (display == NULL) {
		/* 1. images */
		warnx("make_window failed");
		SDL_Quit();
		destroy_set(s);
		exit(EXIT_FAILURE);
	}
	cs = make_cardset_from_set(s, display->r);
	if (cs == NULL) {
		warnx("make_cardset_from_set failed");
		destroy_window(display);
		destroy_set(s);
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	destroy_set(s);
	unsigned int want_exit = 0;
	do {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				want_exit = 1;
			if (ev.type == SDL_KEYDOWN) {
				if (ev.key.keysym.scancode ==
						SDL_SCANCODE_RIGHT) {
					if (cs->current == (cs->n_cards - 1))
						cs->current = 0;
					else
						++cs->current;
				}
				if (ev.key.keysym.scancode ==
						SDL_SCANCODE_LEFT) {
					if (cs->current == 0)
						cs->current = cs->n_cards - 1;
					else
						--cs->current;
				}
			}
		}
		if (want_exit == 1)
			break;
		SDL_SetRenderDrawColor(display->r, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(display->r);
		SDL_Rect dest;
		dest.x = (display->width / 2) -
			(cs->cards[cs->current]->width / 2);
		dest.y = (display->height / 2) -
			(cs->cards[cs->current]->height / 2);
		dest.w = cs->cards[cs->current]->width;
		dest.h = cs->cards[cs->current]->height;
		SDL_RenderCopy(display->r, cs->cards[cs->current]->texture,
				NULL, &dest);
		SDL_RenderPresent(display->r);
		SDL_Delay(100);
	} while (1);

	destroy_cardset(cs);
	destroy_window(display);
	SDL_Quit();

	return 0;
}
