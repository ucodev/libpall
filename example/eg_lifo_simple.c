/**
 * @file eg_lifo_simple.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Last In First Out Simple Example
 *
 * Date: 31-08-2012
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

#include "lifo.h"

#define elem_val(val) ((struct elem [1]) { { val, } })

struct elem {
	unsigned long id;
	char buf[16];
};

/**
 * destroy
 */
void destroy(void *data) {
	free(data);
}

int main(void) {
	struct elem *e1 = NULL, *ptr = NULL;
	struct lifo_handler *hl = NULL;

	/* Alloc memory for element e1 */
	if (!(e1 = malloc(sizeof(struct elem)))) {
		fprintf(stderr, "malloc() failed: %s\n", strerror(errno));
		return 1;
	}

	/* Populate element data */
	e1->id = 0xdeadbeef;
	strcpy(e1->buf, "LIFO Example");

	/* Initialize handler */
	if (!(hl = pall_lifo_init(&destroy, NULL, NULL))) {
		fprintf(stderr, "pall_lifo_init() error: %s\n", strerror(errno));
		return 1;
	}

	/* Push element into queue.
	 *
	 * This is the same as calling:
	 * pall_lifo_push(hl, e1);
	 *
	 */
	hl->push(hl, e1);

	/* Pop element from queue
	 *
	 * This is the same as calling:
	 * pall_lifo_pop(hl)
	 *
	 */
	if ((ptr = hl->pop(hl))) {
		printf("Item popped:\n * id: 0x%.8lx, buf: %s\n", ptr->id, ptr->buf);
		free(ptr); /* free() element after processing */
	} else
		fprintf(stderr, "LIFO pop failed.\n");
		

	/* Destroy handler */
	pall_lifo_destroy(hl);

	return 0;
}

