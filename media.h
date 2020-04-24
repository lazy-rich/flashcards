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
#ifndef MEDIA_H
#define MEDIA_H

#include <stdlib.h>

#include <SDL.h>

/* an idea of an image */
struct image {
	int width;
	int height;
	int channels;
	unsigned char *data;
	size_t size;
};

/* an idea of a window */
struct window {
	int width;
	int height;
	int flags;
	SDL_Window *w;
	SDL_Renderer *r;
};

struct image *make_image_from_memory(unsigned char *, size_t);
struct window *make_window(int, int, int);
void destroy_window(struct window *);

#endif /* MEDIA_H */
