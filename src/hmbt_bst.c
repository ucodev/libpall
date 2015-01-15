/**
 * @file hmbt_bst.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Hash Mod Balanced Tree BST interface
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

#include "config.h"
#include "mm.h"
#include "pall.h"
#include "hmbt_bst.h"
#include "bst.h"

static int _hmbt_bst_insert(struct hmbt_bst_handler *handler, void *data) {
	struct bst_handler *pbst = NULL;

	pbst = handler->array[handler->hash(data) % handler->arr_size];

	return pbst->insert(pbst, data);
}

static int _hmbt_bst_delete(struct hmbt_bst_handler *handler, void *data) {
	struct bst_handler *pbst = NULL;

	pbst = handler->array[handler->hash(data) % handler->arr_size];

	return pbst->del(pbst, data);
}

static void *_hmbt_bst_search(struct hmbt_bst_handler *handler, void *data) {
	struct bst_handler *pbst = NULL;

	pbst = handler->array[handler->hash(data) % handler->arr_size];

	return pbst->search(pbst, data);
}

static ui32_t _hmbt_bst_count(struct hmbt_bst_handler *handler) {
	unsigned long i = 0;
	ui32_t count = 0;
	struct bst_handler *pbst = NULL;

	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];

		count += pbst->count(pbst);
	}

	handler->_stat.count ++;

	return count;
}

static int _hmbt_bst_serialize(struct hmbt_bst_handler *handler, pall_fd_t fd) {
	unsigned long i = 0;
	struct bst_handler *pbst = NULL;
	ui32_t arr_size_nbo = pall_htonl(handler->arr_size);

	if (!handler->ser_data) {
		handler->_stat.serialize_err ++;
		errno = ENOSYS;
		return -1;
	}

	if (pall_write(fd, &arr_size_nbo, 4) != 4) {
		handler->_stat.serialize_err ++;
		return -1;
	}

	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];

		if (pbst->serialize(pbst, fd) < 0) {
			handler->_stat.serialize_err ++;
			return -1;
		}
	}

	handler->_stat.serialize ++;

	return 0;
}

static int _hmbt_bst_unserialize(
		struct hmbt_bst_handler *handler,
		pall_fd_t fd)
{
	unsigned long i = 0;
	struct bst_handler *pbst = NULL;

	if (!handler->unser_data) {
		handler->_stat.unserialize_err ++;
		errno = ENOSYS;
		return -1;
	}

	if (pall_read(fd, &handler->arr_size, 4) != 4) {
		handler->_stat.unserialize_err ++;
		return -1;
	}

	handler->arr_size = ntohl(handler->arr_size);

	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];

		if (pbst->unserialize(pbst, fd) < 0) {
			handler->_stat.unserialize_err ++;
			return -1;
		}
	}

	handler->_stat.unserialize ++;

	return 0;
}

static struct hmbt_bst_stat *_hmbt_bst_stat(struct hmbt_bst_handler *handler) {
	unsigned long i = 0;
	struct bst_handler *pbst = NULL;

	handler->_stat.insert = 0;
	handler->_stat.insert_err = 0;
	handler->_stat.del = 0;
	handler->_stat.del_nf = 0;
	handler->_stat.search = 0;
	handler->_stat.search_nf = 0;
	handler->_stat.elem_count_cur = 0;
	handler->_stat.elem_count_max = 0;
	handler->_stat.node_elem_count_min = ~0UL;
	handler->_stat.node_elem_count_avg = 0;
	handler->_stat.node_elem_count_max = 0;

	memset(handler->_stat.node_elem_count, 0, handler->arr_size * sizeof(unsigned long));
	
	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];

		handler->_stat.insert += pbst->stat(pbst)->insert;
		handler->_stat.insert_err += pbst->stat(pbst)->insert_err;
		handler->_stat.del += pbst->stat(pbst)->del;
		handler->_stat.del_nf += pbst->stat(pbst)->del_nf;
		handler->_stat.search += pbst->stat(pbst)->search;
		handler->_stat.search_nf += pbst->stat(pbst)->search_nf;
		handler->_stat.elem_count_cur += pbst->stat(pbst)->elem_count_cur;
		handler->_stat.elem_count_max += pbst->stat(pbst)->elem_count_max;
		handler->_stat.node_elem_count[i] = pbst->count(pbst);

		if (handler->_stat.node_elem_count[i] < handler->_stat.node_elem_count_min)
			handler->_stat.node_elem_count_min = handler->_stat.node_elem_count[i];

		if (handler->_stat.node_elem_count[i] > handler->_stat.node_elem_count_max)
			handler->_stat.node_elem_count_max = handler->_stat.node_elem_count[i];
	}

	handler->_stat.node_elem_count_avg = handler->_stat.elem_count_cur / (float) handler->arr_size;

	handler->_stat.stat ++;

	return &handler->_stat;
}

static void _hmbt_bst_stat_reset(struct hmbt_bst_handler *handler) {
	unsigned int i = 0;
	unsigned long *ptr = handler->_stat.node_elem_count;
	struct bst_handler *pbst = NULL;

	memset(&handler->_stat, 0, sizeof(struct hmbt_bst_stat));

	memset(ptr, 0, handler->arr_size * sizeof(unsigned long));

	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];

		pbst->stat_reset(pbst);
	}

	handler->_stat.node_elem_count = ptr;
}

static void _hmbt_bst_collapse(struct hmbt_bst_handler *handler) {
	ui32_t i = 0;
	struct bst_handler *pbst = NULL;

	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];

		pbst->collapse(pbst);
	}

	handler->_stat.collapse ++;
}

static void *_hmbt_bst_iterate(struct hmbt_bst_handler *handler) {
	void *data = NULL;
	struct bst_handler *pbst = NULL;

	for (;;) {
		pbst = handler->array[handler->_iterate_arr_pos];

		if ((data = pbst->iterate(pbst)))
			break;

		if (handler->_iterate_reverse) {
			if (!handler->_iterate_arr_pos)
				break;

			handler->_iterate_arr_pos --;
		} else {
			handler->_iterate_arr_pos ++;

			if (handler->_iterate_arr_pos >= handler->arr_size)
				break;
		}
	}

	if (!data)
		handler->_stat.iterate ++;

	return data;
}

static void _hmbt_bst_rewind(struct hmbt_bst_handler *handler, int to) {
	unsigned long i = 0;
	struct bst_handler *pbst = NULL;

	handler->_iterate_reverse = to;

	for (i = 0; i < handler->arr_size; i ++) {
		pbst = handler->array[i];
		pbst->rewind(pbst, to);
	}

	if (to) {
		handler->_iterate_arr_pos = handler->arr_size - 1;
	} else {
		handler->_iterate_arr_pos = 0;
	}

	handler->_stat.rewind ++;
}

struct hmbt_bst_handler *pall_hmbt_bst_init(
		int (*compare) (const void *d1, const void *d2),
		void (*destroy) (void *data),
		ui32_t (*hash) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd),
		unsigned int array_size)
{
	int errsv = 0;
	long i = 0;
	struct hmbt_bst_handler *handler = NULL;

	if (!hash || !destroy) {
		errno = EINVAL;
		return NULL;
	}

	if (!(handler = (struct hmbt_bst_handler *) mm_alloc(sizeof(struct hmbt_bst_handler))))
		return NULL;

	memset(handler, 0, sizeof(struct hmbt_bst_handler));

	handler->arr_size = array_size ? array_size : HMBT_BST_DEFAULT_ARR_SIZE;
	handler->_iterate_arr_pos = 0;

	if (!(handler->_stat.node_elem_count = (unsigned long *) mm_alloc(handler->arr_size * sizeof(unsigned long)))) {
		errsv = errno;
		mm_free(handler);
		errno = errsv;
		return NULL;
	}

	handler->compare = compare;
	handler->hash = hash;
	handler->destroy = destroy;
	handler->ser_data = ser_data;
	handler->unser_data = unser_data;

	handler->insert = &_hmbt_bst_insert;
	handler->del = &_hmbt_bst_delete;
	handler->search = &_hmbt_bst_search;
	handler->serialize = &_hmbt_bst_serialize;
	handler->unserialize = &_hmbt_bst_unserialize;
	handler->stat = &_hmbt_bst_stat;
	handler->stat_reset = &_hmbt_bst_stat_reset;
	handler->count = &_hmbt_bst_count;
	handler->collapse = &_hmbt_bst_collapse;
	handler->iterate = &_hmbt_bst_iterate;
	handler->rewind = &_hmbt_bst_rewind;

	if (!(handler->array = (struct bst_handler **) mm_alloc(sizeof(struct bst_handler *) * handler->arr_size))) {
		errsv = errno;
		mm_free(handler);
		errno = errsv;
		return NULL;
	}

	for (i = 0; ((unsigned) i) < handler->arr_size; i ++) {
		if (!(handler->array[i] = pall_bst_init(compare, destroy, ser_data, unser_data))) {
			errsv = errno;
			for (-- i; i >= 0; i --)
				pall_bst_destroy(handler->array[i]);
			mm_free(handler->array);
			mm_free(handler);
			errno = errsv;
			return NULL;
		}
	}

	return handler;
}

void pall_hmbt_bst_destroy(struct hmbt_bst_handler *h) {
	ui32_t i = 0;

	h->collapse(h);

	for (i = 0; i < h->arr_size; i ++)
		pall_bst_destroy(h->array[i]);

	mm_free(h->array);
	mm_free(h->_stat.node_elem_count);
	mm_free(h);
}

int pall_hmbt_bst_insert(struct hmbt_bst_handler *h, void *data) {
	return h->insert(h, data);
}

int pall_hmbt_bst_delete(struct hmbt_bst_handler *h, void *data) {
	return h->del(h, data);
}

void *pall_hmbt_bst_search(struct hmbt_bst_handler *h, void *data) {
	return h->search(h, data);
}

int pall_hmbt_bst_serialize(struct hmbt_bst_handler *h, pall_fd_t fd) {
	return h->serialize(h, fd);
}

int pall_hmbt_bst_unserialize(struct hmbt_bst_handler *h, pall_fd_t fd) {
	return h->unserialize(h, fd);
}

struct hmbt_bst_stat *pall_hmbt_bst_stat(struct hmbt_bst_handler *h) {
	return h->stat(h);
}

void pall_hmbt_bst_stat_reset(struct hmbt_bst_handler *h) {
	h->stat_reset(h);
}

ui32_t pall_hmbt_bst_count(struct hmbt_bst_handler *h) {
	return h->count(h);
}

void pall_hmbt_bst_collapse(struct hmbt_bst_handler *h) {
	h->collapse(h);
}

void *pall_hmbt_bst_iterate(struct hmbt_bst_handler *h) {
	return h->iterate(h);
}

void pall_hmbt_bst_rewind(struct hmbt_bst_handler *h, int to) {
	h->rewind(h, to);
}

