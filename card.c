#include "card.h"

static struct cardrep *
make_cardrep(struct item *c, SDL_Renderer *r)
{
	struct cardrep *n;
	struct image *img;

	n = malloc(sizeof(struct cardrep));
	if (n == NULL)
		return NULL;
	img = make_image_from_memory(c->data, c->size);
	if (img == NULL) {
		free(n);
		return NULL;
	}
	n->width = img->width;
	n->height = img->height;
	n->x = 0;
	n->y = 0;
	n->texture = make_sdl_texture_from_image(img, r);
	if (n->texture == NULL) {
		free(img->data);
		free(img);
		free(n);
		return NULL;
	}
	free(img->data);
	free(img);

	return n;
}

struct cardset *
make_cardset_from_set(struct set *c, SDL_Renderer *r)
{
	struct cardset *n;

	n = malloc(sizeof(struct cardset));
	if (n == NULL)
		return NULL;
	n->cards = malloc(c->n_items * sizeof(struct cardrep *));
	if (n->cards == NULL) {
		free(n);
		return NULL;
	}
	n->n_cards = c->n_items;
	n->current = 0;

	for (size_t i = 0; i < n->n_cards; ++i) {
		n->cards[i] = make_cardrep(c->items[i], r);
		if (n->cards[i] == NULL) {
			/* ugh but dont see a nicer way */
			for (size_t j = 0; j < i; ++j) {
				SDL_DestroyTexture(n->cards[j]->texture);
				free(n->cards[j]);
			}
			free(n->cards);
			free(n);
			return NULL;
		}
	}

	return n;
}

void
destroy_cardset(struct cardset *d)
{
	for (size_t i = 0; i < d->n_cards; ++i) {
		SDL_DestroyTexture(d->cards[i]->texture);
		free(d->cards[i]);
	}
	free(d->cards);
	free(d);
}
