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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include <SDL.h>

#include "media.h"
#include "set.h"
#include "card.h"

/* range for user supplied interval in seconds */
#define INTERVALMIN	3
#define INTERVALMAX	60
/* default interval in seconds*/
#define DEFAULTINTERVAL	5

static void usage(void);
static Uint32 next_card_timer(Uint32, void *);
static unsigned long make_interval(char *);
static SDL_Rect make_dest_rect(struct window *, struct cardrep *);

static void
usage()
{
	fprintf(stderr, "flashcards -s <setname> -i <delay interval>\n");
	exit(EXIT_FAILURE);
}

static Uint32
next_card_timer(Uint32 interval, void *ud)
{
	SDL_Event ev;
	SDL_UserEvent uev;

	uev.type = SDL_USEREVENT;
	uev.code = 1;
	uev.data1 = NULL;
	uev.data2 = NULL;

	ev.type = SDL_USEREVENT;
	ev.user = uev;

	SDL_PushEvent(&ev);

	return interval;
}

static unsigned long
make_interval(char *s)
{
	char *ep;
	unsigned long res;

	errno = 0;
	res = strtoul(s, &ep, 10);
	if (s[0] == '\0' || *ep != '\0')
		errx(1, "make_interval: %s\n", s);
	if (errno == ERANGE && res == ULONG_MAX)
		errx(1, "make_interval: value out of range");
	if (res < INTERVALMIN || res > INTERVALMAX)
		errx(1, "make_interval: value out of range");

	return res;
}

static SDL_Rect
make_dest_rect(struct window *w, struct cardrep *c)
{
	SDL_Rect dest;

	dest.x = (w->width / 2) - (c->width / 2);
	dest.y = (w->height / 2) - (c->height / 2);
	dest.w = c->width;
	dest.h = c->height;

	return dest;
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
	struct cardrep *cur;
	SDL_TimerID scroll_timer;
	unsigned long interval;

	setpath = NULL;
	s = NULL;
	interval = DEFAULTINTERVAL;

	while ((c = getopt (argc, argv, "s:i:")) != -1) {
		switch (c) {
		case 'i':
			interval = make_interval(optarg);
			break;
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

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
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
	SDL_SetWindowTitle(display->w, "flashcards");
	cs = make_cardset_from_set(s, display->r);
	if (cs == NULL) {
		warnx("make_cardset_from_set failed");
		destroy_window(display);
		destroy_set(s);
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	destroy_set(s);

	scroll_timer = SDL_AddTimer(interval * 1000, next_card_timer, NULL);
	unsigned int want_exit = 0;
	cur = cs->cards[0];
	do {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				want_exit = 1;
			if (ev.type == SDL_USEREVENT)
				cur = next_card(cs);
		}
		if (want_exit == 1)
			break;
		SDL_SetRenderDrawColor(display->r, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(display->r);
		const SDL_Rect dest = make_dest_rect(display, cur);
		SDL_RenderCopy(display->r, cur->texture, NULL, &dest);
		SDL_RenderPresent(display->r);
		SDL_Delay(100);
	} while (1);

	destroy_cardset(cs);
	destroy_window(display);
	SDL_Quit();

	return 0;
}
