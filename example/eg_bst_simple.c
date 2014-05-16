/**
 * @file eg_bst_simple.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Binary Search Tree Simple Example
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

#include "bst.h"

#define elem_val(val) ((struct elem [1]) { { val, } })

struct elem {
	unsigned long id;
	char buf[16];
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

int main(void) {
	struct elem *e1 = NULL, *ptr = NULL;
	struct bst_handler *hb = NULL;

	/* Alloc memory for element e1 */
	if (!(e1 = malloc(sizeof(struct elem)))) {
		fprintf(stderr, "malloc() failed: %s\n", strerror(errno));
		return 1;
	}

	/* Populate element data */
	e1->id = 0xdeadbeef;
	strcpy(e1->buf, "BST Example");

	/* Initialize handler
	 * Default configuration is (CONFIG_SEARCH_LRU | CONFIG_INSERT_HEAD)
	 */
	if (!(hb = pall_bst_init(&compare, &destroy, NULL, NULL))) {
		fprintf(stderr, "pall_bst_init() error: %s\n", strerror(errno));
		return 1;
	}

	/* Insert element.
	 *
	 * This is the same as calling:
	 * pall_bst_insert(hb, e1);
	 *
	 */

	hb->insert(hb, e1);

	/* Search for element
	 *
	 * This is the same as calling:
	 * ptr = pall_bst_search(hb, elem_val(0xdeadbeef));
	 *
	 */
	if ((ptr = hb->search(hb, elem_val(0xdeadbeef))))
		printf("Item found:\n * id: 0x%.8lx, buf: %s\n", ptr->id, ptr->buf);
	else
		fprintf(stderr, "Item not found.\n");

	/* Delete element
	 *
	 * This is the same as calling:
	 * ptr = pall_bst_delete(hb, elem_val(0xdeadbeef));
	 *
	 */
	hb->del(hb, elem_val(0xdeadbeef));
	  /* Element e1 is free()'d on delete() through destroy() function. */

	/* Destroy handler */
	pall_bst_destroy(hb);

	return 0;
}

