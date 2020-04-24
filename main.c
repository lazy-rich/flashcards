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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "set.h"

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
	if (s == NULL)
		exit(EXIT_FAILURE);

	printf("set name is: %s\n", s->name);
	for (size_t i = 0; i < s->n_items; ++i) {
		printf("item name is: %s\n", s->items[i]->name);
		printf("item data size is: %ld\n", s->items[i]->size);
	}
	destroy_set(s);

	return 0;
}
