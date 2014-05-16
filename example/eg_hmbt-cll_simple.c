/**
 * @file eg_hmbt_simple.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Hash Mod Balanced Tree CLL Simple Example
 *
 * Date: 10-10-2012
 * 
 * Copyright 2012 Pedro A. Hortas (pah@ucodev.org)
 *
 * This file is part of libpall.
 *
 * libpall is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libpall is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libpall.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "hmbt_cll.h"

#define elem_val(val) ((struct elem [1]) { { val, } })

struct elem {
	unsigned long id;
	char buf[24];
};

/**
 * compare
 */
int compare(const void *d1, const void *d2) {
	const struct elem *pd1 = (struct elem *) d1, *pd2 = (struct elem *) d2;

	if (pd1->id > pd2->id)
		return 1;

	if (pd1->id < pd2->id)
		return -1;

	return 0;
}

/**
 * destroy
 */
void destroy(void *data) {
	free(data);
}

/**
 * hash
 */
uint32_t hash(void *data) {
	return ((struct elem *) data)->id;
}

int main(void) {
	struct elem *e1 = NULL, *ptr = NULL;
	struct hmbt_cll_handler *hhc = NULL;

	/* Alloc memory for element e1 */
	if (!(e1 = malloc(sizeof(struct elem)))) {
		fprintf(stderr, "malloc() failed: %s\n", strerror(errno));
		return 1;
	}

	/* Populate element data */
	e1->id = 0xdeadbeef;
	strcpy(e1->buf, "HMBT-CLL Example");

	/* Initialize handler
	 * Default configuration is (CONFIG_SEARCH_LRU | CONFIG_INSERT_HEAD)
	 */
	if (!(hhc = pall_hmbt_cll_init(&compare, &destroy, &hash, NULL, NULL, 11))) {
		fprintf(stderr, "pall_hmbt_init() error: %s\n", strerror(errno));
		return 1;
	}

	/* Insert element.
	 *
	 * This is the same as calling:
	 * pall_hmbt_cll_insert(hhc, e1);
	 *
	 */
	hhc->insert(hhc, e1);

	/* Search for element.
	 *
	 * This is the same as calling:
	 * ptr = pall_hmbt_cll_search(hhc, elem_val(0xdeadbeef));
	 *
	 */
	if ((ptr = hhc->search(hhc, elem_val(0xdeadbeef))))
		printf("Item found:\n * id: 0x%.8lx, buf: %s\n", ptr->id, ptr->buf);
	else
		fprintf(stderr, "Item not found.\n");

	/* Delete element.
	 *
	 * This is the same as calling:
	 * pall_hmbt_delete(hh, elem_val(0xdeadbeef));
	 *
	 */
	hhc->del(hhc, elem_val(0xdeadbeef));
	  /* Element e1 is free()'d on delete() through destroy() function. */

	/* Destroy handler */
	pall_hmbt_cll_destroy(hhc);

	return 0;
}

