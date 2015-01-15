/**
 * @file hmbt_cll.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Hash Mod Balanced Tree CLL interface
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
#include "hmbt_cll.h"
#include "cll.h"

static int _hmbt_cll_insert(struct hmbt_cll_handler *handler, void *data) {
	struct cll_handler *pool = NULL;

	pool = handler->array[handler->hash(data) % handler->arr_size];

	return pool->insert(pool, data);
}

static int _hmbt_cll_delete(struct hmbt_cll_handler *handler, void *data) {
	struct cll_handler *pool = NULL;

	pool = handler->array[handler->hash(data) % handler->arr_size];

	return pool->del(pool, data);
}

static void *_hmbt_cll_search(struct hmbt_cll_handler *handler, void *data) {
	struct cll_handler *pool = NULL;

	pool = handler->array[handler->hash(data) % handler->arr_size];

	return pool->search(pool, data);
}

static ui32_t _hmbt_cll_count(struct hmbt_cll_handler *handler) {
	unsigned long i = 0;
	ui32_t count = 0;
	struct cll_handler *pool = NULL;

	for (i = 0; i < handler->arr_size; i ++) {
		pool = handler->array[i];

		count += pool->count(pool);
	}

	handler->_stat.count ++;

	return count;
}

static int _hmbt_cll_serialize(struct hmbt_cll_handler *handler, pall_fd_t fd) {
	unsigned long i = 0;
	struct cll_handler *pool = NULL;
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
		pool = handler->array[i];

		if (pool->serialize(pool, fd) < 0) {
			handler->_stat.serialize_err ++;
			return -1;
		}
	}

	handler->_stat.serialize ++;

	return 0;
}

static int _hmbt_cll_unserialize(
		struct hmbt_cll_handler *handler,
		pall_fd_t fd)
{
	unsigned long i = 0;
	struct cll_handler *pool = NULL;

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
		pool = handler->array[i];

		if (pool->unserialize(pool, fd) < 0) {
			handler->_stat.unserialize_err ++;
			return -1;
		}
	}

	handler->_stat.unserialize ++;

	return 0;
}

static struct hmbt_cll_stat *_hmbt_cll_stat(struct hmbt_cll_handler *handler) {
	unsigned long i = 0;
	struct cll_handler *pool = NULL;

	handler->_stat.insert = 0;
	handler->_stat.insert_err = 0;
	handler->_stat.del = 0;
	handler->_stat.del_nf = 0;
	handler->_stat.search = 0;
	handler->_stat.search_nf = 0;
	handler->_stat.pope = 0;
	handler->_stat.pope_nf = 0;
	handler->_stat.elem_count_cur = 0;
	handler->_stat.elem_count_max = 0;
	handler->_stat.node_elem_count_min = ~0UL;
	handler->_stat.node_elem_count_avg = 0;
	handler->_stat.node_elem_count_max = 0;

	memset(handler->_stat.node_elem_count, 0, handler->arr_size * sizeof(unsigned long));
	
	for (i = 0; i < handler->arr_size; i ++) {
		pool = handler->array[i];

		handler->_stat.insert += pool->stat(pool)->insert;
		handler->_stat.insert_err += pool->stat(pool)->insert_err;
		handler->_stat.del += pool->stat(pool)->del;
		handler->_stat.del_nf += pool->stat(pool)->del_nf;
		handler->_stat.search += pool->stat(pool)->search;
		handler->_stat.search_nf += pool->stat(pool)->search_nf;
		handler->_stat.pope += pool->stat(pool)->pope;
		handler->_stat.pope_nf += pool->stat(pool)->pope_nf;
		handler->_stat.elem_count_cur += pool->stat(pool)->elem_count_cur;
		handler->_stat.elem_count_max += pool->stat(pool)->elem_count_max;
		handler->_stat.node_elem_count[i] = pool->count(pool);

		if (handler->_stat.node_elem_count[i] < handler->_stat.node_elem_count_min)
			handler->_stat.node_elem_count_min = handler->_stat.node_elem_count[i];

		if (handler->_stat.node_elem_count[i] > handler->_stat.node_elem_count_max)
			handler->_stat.node_elem_count_max = handler->_stat.node_elem_count[i];
	}

	handler->_stat.node_elem_count_avg = handler->_stat.elem_count_cur / (float) handler->arr_size;

	handler->_stat.stat ++;

	return &handler->_stat;
}

static void _hmbt_cll_stat_reset(struct hmbt_cll_handler *handler) {
	unsigned int i = 0;
	unsigned long *ptr = handler->_stat.node_elem_count;
	struct cll_handler *pool = NULL;

	memset(&handler->_stat, 0, sizeof(struct hmbt_cll_stat));

	memset(ptr, 0, handler->arr_size * sizeof(unsigned long));

	for (i = 0; i < handler->arr_size; i ++) {
		pool = handler->array[i];

		pool->stat_reset(pool);
	}

	handler->_stat.node_elem_count = ptr;
}

static void *_hmbt_cll_pope(struct hmbt_cll_handler *handler, void *data) {
	struct cll_handler *pool = NULL;

	pool = handler->array[handler->hash(data) % handler->arr_size];

	return pool->pope(pool, data);
}

static void *_hmbt_cll_poph_elem(struct hmbt_cll_handler *handler, void *data) {
	struct cll_handler *pool = NULL;
	void *d = NULL;

	pool = handler->array[handler->hash(data) % handler->arr_size];

	if (!(d = pool->poph(pool)))
		handler->_stat.poph_elem_nf ++;
	else
		handler->_stat.poph_elem ++;

	return d;
}

static void *_hmbt_cll_poph_index(struct hmbt_cll_handler *handler, ui32_t index) {
	struct cll_handler *pool = NULL;
	void *d = NULL;

	pool = handler->array[index];

	if (!(d = pool->poph(pool)))
		handler->_stat.poph_index_nf ++;
	else
		handler->_stat.poph_index ++;

	return d;
}


static void _hmbt_cll_collapse(struct hmbt_cll_handler *handler) {
	ui32_t i = 0;
	struct cll_handler *pool = NULL;

	for (i = 0; i < handler->arr_size; i ++) {
		pool = handler->array[i];

		pool->collapse(pool);
	}

	handler->_stat.collapse ++;
}

static void *_hmbt_cll_iterate(struct hmbt_cll_handler *handler) {
	void *data = NULL;
	struct cll_handler *pool = NULL;

	for (;;) {
		pool = handler->array[handler->_iterate_arr_pos];

		if ((data = pool->iterate(pool)))
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

static void _hmbt_cll_rewind(struct hmbt_cll_handler *handler, int to) {
	unsigned long i = 0;
	struct cll_handler *pool = NULL;

	handler->_iterate_reverse = to;

	for (i = 0; i < handler->arr_size; i ++) {
		pool = handler->array[i];
		pool->rewind(pool, to);
	}

	if (to) {
		handler->_iterate_arr_pos = handler->arr_size - 1;
	} else {
		handler->_iterate_arr_pos = 0;
	}

	handler->_stat.rewind ++;
}

static ui32_t _hmbt_cll_set_config(struct hmbt_cll_handler *handler, ui32_t flags) {
	ui32_t old_flags = flags;
	unsigned long i = 0;
	struct cll_handler *pool = NULL;

	for (i = 0; i < handler->arr_size; i ++) {
		pool = handler->array[i];
		pool->set_config(pool, flags);
	}

	handler->_stat.set_config ++;

	return old_flags;
}

static ui32_t _hmbt_cll_get_config(struct hmbt_cll_handler *handler) {
	handler->_stat.get_config ++;

	return handler->array[0]->get_config(handler->array[0]);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct hmbt_cll_handler *pall_hmbt_cll_init(
		int (*compare) (const void *d1, const void *d2),
		void (*destroy) (void *data),
		ui32_t (*hash) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd),
		unsigned int array_size)
{
	int errsv = 0;
	long i = 0;
	struct hmbt_cll_handler *handler = NULL;

	if (!hash || !destroy) {
		errno = EINVAL;
		return NULL;
	}

	if (!(handler = (struct hmbt_cll_handler *) mm_alloc(sizeof(struct hmbt_cll_handler))))
		return NULL;

	memset(handler, 0, sizeof(struct hmbt_cll_handler));

	handler->arr_size = array_size ? array_size : HMBT_CLL_DEFAULT_ARR_SIZE;
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

	handler->insert = &_hmbt_cll_insert;
	handler->del = &_hmbt_cll_delete;
	handler->search = &_hmbt_cll_search;
	handler->serialize = &_hmbt_cll_serialize;
	handler->unserialize = &_hmbt_cll_unserialize;
	handler->stat = &_hmbt_cll_stat;
	handler->stat_reset = &_hmbt_cll_stat_reset;
	handler->count = &_hmbt_cll_count;
	handler->pope = &_hmbt_cll_pope;
	handler->poph_elem = &_hmbt_cll_poph_elem;
	handler->poph_index = &_hmbt_cll_poph_index;
	handler->collapse = &_hmbt_cll_collapse;
	handler->iterate = &_hmbt_cll_iterate;
	handler->rewind = &_hmbt_cll_rewind;
	handler->set_config = &_hmbt_cll_set_config;
	handler->get_config = &_hmbt_cll_get_config;

	if (!(handler->array = (struct cll_handler **) mm_alloc(sizeof(struct cll_handler *) * handler->arr_size))) {
		errsv = errno;
		mm_free(handler);
		errno = errsv;
		return NULL;
	}

	for (i = 0; ((unsigned) i) < handler->arr_size; i ++) {
		if (!(handler->array[i] = pall_cll_init(compare, destroy, ser_data, unser_data))) {
			errsv = errno;
			for (-- i; i >= 0; i --)
				pall_cll_destroy(handler->array[i]);
			mm_free(handler->array);
			mm_free(handler);
			errno = errsv;
			return NULL;
		}
	}

	return handler;
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_hmbt_cll_destroy(struct hmbt_cll_handler *h) {
	ui32_t i = 0;

	h->collapse(h);

	for (i = 0; i < h->arr_size; i ++)
		pall_cll_destroy(h->array[i]);

	mm_free(h->array);
	mm_free(h->_stat.node_elem_count);
	mm_free(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_hmbt_cll_insert(struct hmbt_cll_handler *h, void *data) {
	return h->insert(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_hmbt_cll_delete(struct hmbt_cll_handler *h, void *data) {
	return h->del(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_hmbt_cll_search(struct hmbt_cll_handler *h, void *data) {
	return h->search(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_hmbt_cll_serialize(struct hmbt_cll_handler *h, pall_fd_t fd) {
	return h->serialize(h, fd);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_hmbt_cll_unserialize(struct hmbt_cll_handler *h, pall_fd_t fd) {
	return h->unserialize(h, fd);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct hmbt_cll_stat *pall_hmbt_cll_stat(struct hmbt_cll_handler *h) {
	return h->stat(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_hmbt_cll_stat_reset(struct hmbt_cll_handler *h) {
	h->stat_reset(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_hmbt_cll_count(struct hmbt_cll_handler *h) {
	return h->count(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_hmbt_cll_pope(struct hmbt_cll_handler *h, void *data) {
	return h->pope(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_hmbt_cll_poph_elem(struct hmbt_cll_handler *h, void *data) {
	return h->poph_elem(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_hmbt_cll_poph_index(struct hmbt_cll_handler *h, ui32_t index) {
	return h->poph_index(h, index);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_hmbt_cll_collapse(struct hmbt_cll_handler *h) {
	h->collapse(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_hmbt_cll_iterate(struct hmbt_cll_handler *h) {
	return h->iterate(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_hmbt_cll_rewind(struct hmbt_cll_handler *h, int to) {
	h->rewind(h, to);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_hmbt_cll_set_config(struct hmbt_cll_handler *h, ui32_t flags) {
	return h->set_config(h, flags);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_hmbt_cll_get_config(struct hmbt_cll_handler *h) {
	return h->get_config(h);
}

