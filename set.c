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
#include "set.h"

static struct item *make_item(void);
static void destroy_item(struct item *);
static struct set *make_set(void);
static int archive_block_to_item(struct archive *, struct item *);
static void destroy_reader(struct archive *);

static struct item *
make_item()
{
	struct item *n;

	n = malloc(sizeof(struct item));
	if (n == NULL)
		return NULL;
	n->name = NULL;
	n->data = NULL;
	n->size = 0;

	return n;
}

static void
destroy_item(struct item *d)
{
	free(d->name);
	free(d->data);
	free(d);
}

static struct set *
make_set()
{
	struct set *n;

	n = malloc(sizeof(struct set));
	if (n == NULL)
		return NULL;
	n->name = NULL;
	n->items = NULL;
	n->n_items = 0;

	return n;
}

struct set *
load_set(const char *path)
{
	struct archive *reader;
	struct archive_entry *entry;
	struct item **tmpi;
	struct set *s;
	int rc;

	rc = ARCHIVE_FAILED;

	reader = archive_read_new();
	if (reader == NULL)
		return NULL;
	rc = archive_read_support_format_tar(reader);
	if (rc != ARCHIVE_OK) {
		destroy_reader(reader);
		return NULL;
	}
	rc = archive_read_open_filename(reader, path, 10240);
	if (rc != ARCHIVE_OK) {
		destroy_reader(reader);
		return NULL;
	}
	s = make_set();
	if (s == NULL) {
		destroy_reader(reader);
		return NULL;
	}
	s->name = strdup(path);
	if (s->name == NULL) {
		free(s);
		destroy_reader(reader);
		return NULL;
	}

	/* iterate through each entry */
	while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
		struct item *tmp = make_item();
		if (tmp == NULL) {
			destroy_set(s);
			destroy_reader(reader);
			return NULL;
		}
		tmp->name = strdup(archive_entry_pathname(entry));
		if (tmp->name == NULL) {
			free(tmp);
			destroy_set(s);
			destroy_reader(reader);
			return NULL;
		}
		if (archive_block_to_item(reader, tmp) != 0) {
			free(tmp->name);
			free(tmp);	
			destroy_set(s);
			destroy_reader(reader);
			return NULL;
		}
		tmpi = realloc(s->items,
				(s->n_items + 1) * sizeof (struct item *));
		if (tmpi == NULL) {
			free(tmp->name);
			free(tmp);	
			destroy_set(s);
			destroy_reader(reader);
			return NULL;
		}
		s->items = tmpi;
		s->items[s->n_items] = tmp;
		++s->n_items;
	}
	destroy_reader(reader);

	return s;
}

void
destroy_set(struct set *d)
{
	free(d->name);
	for (size_t i = 0; i < d->n_items; ++i)
		destroy_item(d->items[i]);
	free(d->items);
	free(d);
}

static int
archive_block_to_item(struct archive *reader, struct item *n)
{
	const void *bp;
	size_t len;
	off_t offset;
	int rc;
	unsigned char *tmp;

	do {
		rc = archive_read_data_block(reader, &bp, &len, &offset);
		if (rc == ARCHIVE_EOF)
			return 0;
		if (rc != ARCHIVE_OK)
			return -1;
		tmp = realloc(n->data, n->size + len);
		if (tmp == NULL)
			return -1;
		n->data = tmp;
		memcpy(n->data + n->size, bp, len);
		n->size += len;
	} while(1);

	/* not reached */
	return -1;
}

static void
destroy_reader(struct archive *reader)
{
	int rc;

	rc = archive_read_close(reader);
	if (rc != ARCHIVE_OK) {
		warnx("destroy_reader: %s", archive_error_string(reader));
	}
	rc = archive_read_free(reader);
	if (rc != ARCHIVE_OK) {
		warnx("destroy_reader: %s", archive_error_string(reader));
	}
}
