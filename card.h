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
#ifndef CARD_H
#define CARD_H

#include <stdlib.h>

#include <SDL.h>

#include "media.h"
#include "set.h"

/* representation of all of the cards */
struct cardset {
	struct cardrep **cards;
	size_t n_cards;
	size_t current;
};

/* representation of a card */
struct cardrep {
	SDL_Texture *texture;
	int height;
	int width;
	int x;
	int y;
};

struct cardset *make_cardset_from_set(struct set *, SDL_Renderer *);
void destroy_cardset(struct cardset *);

#endif /* CARD_H */