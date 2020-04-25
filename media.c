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
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "media.h"

#define COLOURDEPTH	24
#define RMASK		0x000000ff
#define GMASK		0x0000ff00
#define BMASK		0x00ff0000

static struct image *make_image(void);

static struct image *
make_image()
{
	struct image *n;

	n = malloc(sizeof(struct image));
	if (n == NULL)
		return NULL;
	n->width = 0;
	n->height = 0;
	n->channels = 0;
	n->data = NULL;
	n->size = 0;

	return n;
}

struct image *
make_image_from_memory(unsigned char *data, size_t size)
{
	struct image *n;

	n = make_image();
	if (n == NULL)
		return NULL;
	n->data = stbi_load_from_memory(data, size, &n->width, &n->height,
			&n->channels, 0);
	if (n->data == NULL) {
		free(n);
		return NULL;
	}
	n->size = size;

	return n;
}

struct window *
make_window(int width, int height, int flags)
{
	struct window *n;

	n = malloc(sizeof(struct window));
	if (n == NULL)
		return NULL;
	n->width = width;
	n->height = height;
	n->flags = flags;
	const int rc = SDL_CreateWindowAndRenderer(n->width, n->height,
			n->flags, &n->w, &n->r);
	if (rc == -1) {
		free(n);
		return NULL;
	}

	return n;
}

void
destroy_window(struct window *d)
{
	SDL_DestroyRenderer(d->r);
	SDL_DestroyWindow(d->w);
	free(d);
}

SDL_Texture *
make_sdl_texture_from_image(struct image *img, SDL_Renderer *r)
{
	SDL_Surface *surface;
	SDL_Texture *texture;

	surface = SDL_CreateRGBSurfaceFrom(img->data, img->width, img->height,
			COLOURDEPTH, 3 * img->width, RMASK, GMASK, BMASK, 0);
	if (surface == NULL)
		return NULL;

	texture = SDL_CreateTextureFromSurface(r, surface);
	if (texture == NULL) {
		SDL_FreeSurface(surface);
		return NULL;
	}
	SDL_FreeSurface(surface);

	return texture;
}
