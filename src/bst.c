/**
 * @file bst.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Binary Search Tree interface
 *
 * Date: 15-01-2015
 * 
 * Copyright 2012-2015 Pedro A. Hortas (pah@ucodev.org)
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
#include <search.h>
#include <assert.h>

#include "config.h"
#include "mm.h"
#include "pall.h"
#include "bst.h"
#include "fifo.h"
#include "lifo.h"

static void _bst_action_rewind_forward(
		const void *nodep,
		const VISIT which,
		const int depth)
{
	const struct bst_node *n = *(const struct bst_node **) nodep;

	switch (which) {
		case preorder: break;
		case endorder: break;
		case postorder:
		case leaf: assert(!n->h->_iterate_forward->push(n->h->_iterate_forward, n->data)); break;
	}
}

static void _bst_action_rewind_backward(
		const void *nodep,
		const VISIT which,
		const int depth)
{
	const struct bst_node *n = *(const struct bst_node **) nodep;

	switch (which) {
		case preorder: break;
		case endorder: break;
		case postorder:
		case leaf: assert(!n->h->_iterate_backward->push(n->h->_iterate_backward, n->data)); break;
	}
}

static void _bst_action_serialize(
		const void *nodep,
		const VISIT which,
		const int depth)
{
	const struct bst_node *n = *(const struct bst_node **) nodep;

	if (n->h->_ser_ret)
		return;

	switch (which) {
		case preorder: break;
		case endorder: break;
		case postorder:
		case leaf: n->h->_ser_ret = n->h->ser_data(n->h->_ser_cur_fd, n->data); break;
	}
}

static int _bst_compare(const void *d1, const void *d2) {
	const struct bst_node *b1 = (const struct bst_node *) d1;
	const struct bst_node *b2 = (const struct bst_node *) d2;

	return b1->h->compare(b1->data, b2->data);
}

static int _bst_insert(struct bst_handler *handler, void *data) {
	struct bst_node *n = NULL;
	void *val = NULL;

	if (!(n = (struct bst_node *) mm_alloc(sizeof(struct bst_node)))) {
		handler->_stat.insert_err ++;
		return -1;
	}

	n->data = data;
	n->h = handler;

	if (!(val = tsearch(n, &handler->root, &_bst_compare))) {
		handler->_stat.insert_err ++;
		mm_free(n);
		return -1;
	} else if (*(struct bst_node **) val != n) {
		mm_free(n);
	}

	handler->_stat.insert ++;
	handler->_count ++;

	if (handler->_stat.elem_count_max < handler->_count)
		handler->_stat.elem_count_max = handler->_count;

	return 0;
}

static int _bst_delete(struct bst_handler *handler, void *data) {
	struct bst_node n, *d = NULL;
	void *val;

	n.data = data;
	n.h = handler;

	if (!(val = tfind(&n, &handler->root, &_bst_compare))) {
		handler->_stat.del_nf ++;
		return -1;
	}

	d = *(struct bst_node **) val;

	if (!tdelete(&n, &handler->root, &_bst_compare)) {
		handler->_stat.del_nf ++;
		return -1;
	}

	handler->destroy(d->data);
	mm_free(d);

	handler->_stat.del ++;

	if (!-- handler->_count)
		handler->root = NULL;

	return 0;
}

static void *_bst_search(struct bst_handler *handler, void *data) {
	struct bst_node n;
	void *val;

	n.data = data;
	n.h = handler;

	if (!(val = tfind(&n, &handler->root, &_bst_compare))) {
		handler->_stat.search_nf ++;
		return NULL;
	}

	handler->_stat.search ++;

	return (*(struct bst_node **) val)->data;
}

static ui32_t _bst_count(struct bst_handler *handler) {
	handler->_stat.count ++;

	return handler->_count;
}

static int _bst_serialize(struct bst_handler *handler, pall_fd_t fd) {
	ui32_t count_nbo = pall_htonl(handler->_count);
	handler->_ser_cur_fd = fd;
	handler->_ser_ret = 0;

	if (!handler->ser_data) {
		handler->_stat.serialize_err ++;
		errno = ENOSYS;
		return -1;
	}

	if (pall_write(fd, &count_nbo, 4) != 4) {
		handler->_stat.serialize_err ++;
		return -1;
	}

	if (!handler->root) {
		handler->_stat.serialize ++;
		return 0;
	}

	twalk(handler->root, &_bst_action_serialize);

	if (handler->_ser_ret)
		handler->_stat.serialize_err ++;
	else
		handler->_stat.serialize ++;

	return handler->_ser_ret;
}

static int _bst_unserialize(struct bst_handler *handler, pall_fd_t fd) {
	ui32_t count = 0;
	void *data = NULL;

	if (!handler->unser_data) {
		handler->_stat.unserialize_err ++;
		errno = ENOSYS;
		return -1;
	}

	if (pall_read(fd, &count, 4) != 4) {
		handler->_stat.unserialize_err ++;
		return -1;
	}

	for (count = ntohl(count); count; count --) {
		if (!(data = handler->unser_data(fd))) {
			handler->_stat.unserialize_err ++;
			return -1;
		}

		if (handler->insert(handler, data) < 0) {
			handler->_stat.unserialize_err ++;
			return -1;
		}
	}

	handler->_stat.unserialize ++;

	return 0;
}

static struct bst_stat *_bst_stat(struct bst_handler *handler) {
	handler->_stat.elem_count_cur = handler->_count;
	handler->_stat.stat ++;

	return &handler->_stat;
}

static void _bst_stat_reset(struct bst_handler *handler) {
	memset(&handler->_stat, 0, sizeof(struct bst_stat));
}

static void _bst_collapse(struct bst_handler *handler) {
#ifdef _GNU_SOURCE
	tdestroy(handler->root, handler->destroy);
#else
	void *d = NULL;

	for (handler->rewind(handler, 0); (d = handler->iterate(handler)); ) {
		handler->del(handler, d);
		handler->_stat.del --;
	}
#endif

	handler->_stat.collapse ++;
	handler->_count = 0;
	handler->root = NULL;
}

static void *_bst_iterate(struct bst_handler *handler) {
	void *data = handler->_iterate_reverse ? handler->_iterate_backward->pop(handler->_iterate_backward) : handler->_iterate_forward->pop(handler->_iterate_forward);

	if (!data)
		handler->_stat.iterate ++;

	return data;
}

static void _bst_rewind(struct bst_handler *handler, int to) {
	handler->_iterate_reverse = to;

	while (handler->_iterate_forward->pop(handler->_iterate_forward)) ;
	while (handler->_iterate_backward->pop(handler->_iterate_backward)) ;

	twalk(handler->root, to ? &_bst_action_rewind_backward : &_bst_action_rewind_forward);

	handler->_stat.rewind ++;
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct bst_handler *pall_bst_init(
		int (*compare) (const void *d1, const void *d2),
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd))
{
	int errsv = 0;
	struct bst_handler *handler = NULL;

	if (!destroy) {
		errno = EINVAL;
		return NULL;
	}

	if (!(handler = (struct bst_handler *) mm_alloc(sizeof(struct bst_handler))))
		return NULL;

	memset(handler, 0, sizeof(struct bst_handler));

	if (!(handler->_iterate_forward = pall_fifo_init(destroy, NULL, NULL))) {
		errsv = errno;
		mm_free(handler);
		errno = errsv;
		return NULL;
	}

	if (!(handler->_iterate_backward = pall_lifo_init(destroy, NULL, NULL))) {
		errsv = errno;
		pall_fifo_destroy(handler->_iterate_forward);
		mm_free(handler);
		errno = errsv;
		return NULL;
	}

	handler->compare = compare;
	handler->destroy = destroy;
	handler->ser_data = ser_data;
	handler->unser_data = unser_data;

	handler->insert = &_bst_insert;
	handler->del = &_bst_delete;
	handler->search = &_bst_search;
	handler->serialize = &_bst_serialize;
	handler->unserialize = &_bst_unserialize;
	handler->stat = &_bst_stat;
	handler->stat_reset = &_bst_stat_reset;
	handler->count = &_bst_count;
	handler->collapse = &_bst_collapse;
	handler->iterate = &_bst_iterate;
	handler->rewind = &_bst_rewind;

	return handler;
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_destroy(struct bst_handler *h) {
	h->collapse(h);

	while (h->_iterate_forward->pop(h->_iterate_forward)) ;
	while (h->_iterate_backward->pop(h->_iterate_backward)) ;
	pall_fifo_destroy(h->_iterate_forward);
	pall_lifo_destroy(h->_iterate_backward);

	mm_free(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_insert(struct bst_handler *h, void *data) {
	return h->insert(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_delete(struct bst_handler *h, void *data) {
	return h->del(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_bst_search(struct bst_handler *h, void *data) {
	return h->search(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_serialize(struct bst_handler *h, pall_fd_t fd) {
	return h->serialize(h, fd);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_unserialize(struct bst_handler *h, pall_fd_t fd) {
	return h->unserialize(h, fd);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct bst_stat *pall_bst_stat(struct bst_handler *h) {
	return h->stat(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_stat_reset(struct bst_handler *h) {
	h->stat_reset(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_bst_count(struct bst_handler *h) {
	return h->count(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_collapse(struct bst_handler *h) {
	h->collapse(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_bst_iterate(struct bst_handler *h) {
	return h->iterate(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_rewind(struct bst_handler *h, int to) {
	h->rewind(h, to);
}

