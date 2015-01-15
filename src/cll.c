/**
 * @file cll.c
 * @brief Portable Abstracted Linked Lists (libpall)
 *        Circular Linked List interface
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
#include "cll.h"

static void _cll_move_to_head(
		struct cll_handler *handler,
		struct cll_elem *elem)
{
	struct cll_elem *head_ptr = handler->cll_head;

	if (elem != head_ptr) {
		elem->prev->next = elem->next;
		elem->next->prev = elem->prev;

		elem->next = head_ptr;
		elem->prev = head_ptr->prev;
		head_ptr->prev->next = elem;
		head_ptr->prev = elem;

		handler->cll_head = elem;
	}
}

static int _cll_insert_head(struct cll_handler *handler, void *data) {
	struct cll_elem *n = NULL;

	if (!(n = (struct cll_elem *) mm_alloc(sizeof(struct cll_elem)))) {
		handler->_stat.insert_err ++;
		return -1;
	}

	n->data = data;

	if (!handler->cll) {
		n->next = n;
		n->prev = n;
		handler->cll = n;
		handler->cll_head = n;
	} else {
		n->next = handler->cll_head;
		n->prev = handler->cll_head->prev;
		handler->cll_head->prev->next = n;
		handler->cll_head->prev = n;
		handler->cll_head = n;
	}

	handler->_stat.insert ++;
	handler->_count ++;

	if (handler->_stat.elem_count_max < handler->_count)
		handler->_stat.elem_count_max = handler->_count;

	return 0;
}

static int _cll_insert_tail(struct cll_handler *handler, void *data) {
	if (_cll_insert_head(handler, data) < 0)
		return -1;

	handler->cll_head = handler->cll_head->next;

	return 0;
}

static int _cll_insert_sorted(struct cll_handler *handler, void *data) {
	struct cll_elem *pool = handler->cll, *n = NULL;

	if (!(n = (struct cll_elem *) mm_alloc(sizeof(struct cll_elem)))) {
		handler->_stat.insert_err ++;
		return -1;
	}

	n->data = data;

	if (!pool) {
		pool = n;
		pool->next = pool;
		pool->prev = pool;
		handler->cll_head = pool;
		handler->cll = pool;
	} else if (handler->compare(pool->data, data) > 0) {
		do {
			pool = pool->prev;
		} while ((pool != handler->cll_head->prev) && (handler->compare(pool->data, data) > 0));

		n->next = pool->next;
		n->prev = pool;
		pool->next->prev = n;
		pool->next = n;

		if (n->next == handler->cll_head)
			handler->cll_head = n;
	} else {
		do {
			pool = pool->next;
		} while ((pool != handler->cll_head) && (handler->compare(pool->data, data) < 0));

		n->next = pool;
		n->prev = pool->prev;
		pool->prev->next = n;
		pool->prev = n;
	}

	handler->cll = n;

	handler->_stat.insert ++;
	handler->_count ++;

	if (handler->_stat.elem_count_max < handler->_count)
		handler->_stat.elem_count_max = handler->_count;

	return 0;
}

static int _cll_insert(struct cll_handler *handler, void *data) {
	struct cll_elem *pool = handler->cll, *n = NULL;

	if (!(n = (struct cll_elem *) mm_alloc(sizeof(struct cll_elem)))) {
		handler->_stat.insert_err ++;
		return -1;
	}

	n->data = data;

	if (!pool) {
		pool = n;
		pool->next = pool;
		pool->prev = pool;
		handler->cll_head = pool;
		handler->cll = pool;
	} else {
		if (handler->_config_flags & CONFIG_INSERT_NEXT) {
			n->next = pool->next;
			n->prev = pool;
			pool->next->prev = n;
			pool->next = n;
		} else {
			n->next = pool;
			n->prev = pool->prev;
			pool->prev->next = n;
			pool->prev = n;
		}

		handler->cll = n;
	}

	handler->_stat.insert ++;
	handler->_count ++;

	if (handler->_stat.elem_count_max < handler->_count)
		handler->_stat.elem_count_max = handler->_count;

	return 0;
}

static void *_cll_unlink(struct cll_handler *handler, void *data) {
	struct cll_elem *pool = handler->cll, *start = pool, *ptr = NULL;
	void *elem_data = NULL;

	if (!pool)
		return NULL;

	if (handler->_config_flags & CONFIG_SEARCH_LRU) {
		start = pool = handler->cll_head;
	} else if (handler->_config_flags & CONFIG_SEARCH_AUTO) {
		if (handler->compare(pool->data, data) > 0) {
			handler->_config_flags |= CONFIG_SEARCH_BACKWARD;
		} else {
			handler->_config_flags &= ~CONFIG_SEARCH_BACKWARD;
		}
	}

	for (;;) {
		if (!handler->compare(data, pool->data)) {
			if (pool == pool->next) {
				elem_data = pool->data;
				mm_free(pool);
				handler->cll = NULL;
				handler->cll_head = NULL;
				handler->_iterate_cur = NULL;
				handler->_iterate_start = NULL;

				break;
			}

			if (handler->cll_head == pool)
				handler->cll_head = pool->next;

			ptr = pool;
			pool->prev->next = pool->next;
			pool->next->prev = pool->prev;
			handler->cll = pool->next;

			if (handler->cll_head == pool)
				handler->cll_head = pool->next;

			if (ptr == handler->_iterate_cur)
				handler->_iterate_cur = ptr->next;

			if (ptr == handler->_iterate_start)
				handler->_iterate_start = ptr->next;

			elem_data = ptr->data;

			mm_free(ptr);

			break;
		}

		pool = (handler->_config_flags & CONFIG_SEARCH_BACKWARD) ? pool->prev : pool->next;

		if (pool == start)
			return NULL;
	}

	handler->_count --;

	return elem_data;
}

static int _cll_delete(struct cll_handler *handler, void *data) {
	void *d = NULL;

	if ((d = _cll_unlink(handler, data))) {
		handler->destroy(d);
		handler->_stat.del ++;
		return 0;
	}

	handler->_stat.del_nf ++;

	return -1;
}

static void *_cll_search_nop(struct cll_handler *handler, void *data) {
	return NULL;
}

static void *_cll_search(struct cll_handler *handler, void *data) {
	struct cll_elem *pool = handler->cll, *start = pool;

	if (!pool) {
		handler->_stat.search_nf ++;
		return NULL;
	}

	if (handler->_config_flags & CONFIG_SEARCH_LRU) {
		start = pool = handler->cll_head;
	} else if (handler->_config_flags & CONFIG_SEARCH_AUTO) {
		if (handler->compare(pool->data, data) > 0) {
			handler->_config_flags |= CONFIG_SEARCH_BACKWARD;
		} else {
			handler->_config_flags &= ~CONFIG_SEARCH_BACKWARD;
		}
	}

	for (;;) {
		if (!handler->compare(data, pool->data)) {
			if (handler->_config_flags & CONFIG_SEARCH_LRU) {
				_cll_move_to_head(handler, pool);
			} else {
				handler->cll = pool;
			}

			handler->_stat.search ++;

			return pool->data;
		}

		pool = (handler->_config_flags & CONFIG_SEARCH_BACKWARD) ? pool->prev : pool->next;

		if (pool == start)
			break;
	}

	handler->_stat.search_nf ++;

	return NULL;
}

static ui32_t _cll_count(struct cll_handler *handler) {
	handler->_stat.count ++;

	return handler->_count;
}

static int _cll_serialize(struct cll_handler *handler, pall_fd_t fd) {
	struct cll_elem *start = NULL, *pool = NULL;
	ui32_t count_nbo = pall_htonl(handler->_count);

	if (!handler->ser_data) {
		handler->_stat.serialize_err ++;
		errno = ENOSYS;
		return -1;
	}

	if (pall_write(fd, &count_nbo, 4) != 4) {
		handler->_stat.serialize_err ++;
		return -1;
	}

	if (!handler->cll) {
		handler->_stat.serialize ++;
		return 0;
	}

	for (pool = handler->cll_head, start = handler->cll_head; ; ) {
		if (handler->ser_data(fd, pool->data) < 0) {
			handler->_stat.serialize_err ++;
			return -1;
		}

		pool = pool->next;

		if (pool == start)
			break;
	}

	handler->_stat.serialize ++;

	return 0;
}

static int _cll_unserialize(struct cll_handler *handler, pall_fd_t fd) {
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

static struct cll_stat *_cll_stat(struct cll_handler *handler) {
	handler->_stat.elem_count_cur = handler->_count;
	handler->_stat.stat ++;

	return &handler->_stat;
}

static void _cll_stat_reset(struct cll_handler *handler) {
	memset(&handler->_stat, 0, sizeof(struct cll_stat));
}

static void *_cll_pope(struct cll_handler *handler, void *data) {
	void *d = NULL;

	if ((d = _cll_unlink(handler, data))) {
		handler->_stat.pope ++;
		return d;
	}

	handler->_stat.pope_nf ++;

	return NULL;
}

static void *_cll_poph(struct cll_handler *handler) {
	struct cll_elem *ptr = NULL;
	void *pop_data = NULL;

	if (!handler->cll_head) {
		handler->_stat.poph_nf ++;
		return NULL;
	}

	if (handler->cll_head == handler->cll_head->next) {
		pop_data = handler->cll_head->data;
		mm_free(handler->cll_head);
		handler->cll = NULL;
		handler->cll_head = NULL;
		handler->_iterate_cur = NULL;
		handler->_iterate_start = NULL;
	} else {
		ptr = handler->cll_head;
		handler->cll_head->prev->next = handler->cll_head->next;
		handler->cll_head->next->prev = handler->cll_head->prev;
		handler->cll = handler->cll_head->next;
		handler->cll_head = handler->cll;

		if (ptr == handler->_iterate_cur)
			handler->_iterate_cur = ptr->next;

		if (ptr == handler->_iterate_start)
			handler->_iterate_start = ptr->next;

		pop_data = ptr->data;
		mm_free(ptr);
	}

	handler->_stat.poph ++;
	handler->_count --;

	return pop_data;
}

static void _cll_collapse(struct cll_handler *handler) {
	struct cll_elem *pool = handler->cll, *ptr = NULL;

	while (pool) {
		if (pool == pool->next) {
			handler->destroy(pool->data);
			mm_free(pool);
			handler->cll = NULL;
			handler->cll_head = NULL;
			handler->_iterate_start = NULL;
			handler->_iterate_cur = NULL;

			break;
		}

		ptr = pool;
		pool->next->prev = pool->prev;
		pool->prev->next = pool->next;
		handler->cll = pool->next;

		pool = pool->next;

		handler->destroy(ptr->data);
		mm_free(ptr);
	}

	handler->_stat.collapse ++;
	handler->_count = 0;
}

static void *_cll_iterate(struct cll_handler *handler) {
	struct cll_elem *ptr = NULL;

	if (!handler->cll) {
		handler->_stat.iterate ++;
		return NULL;
	}

	if (!handler->_iterate_start) {
		if (handler->_iterate_reverse) {
			handler->_iterate_start = handler->cll_head->prev;
			handler->_iterate_cur = handler->cll_head->prev->prev;
		} else {
			handler->_iterate_start = handler->cll_head;
			handler->_iterate_cur = handler->cll_head->next;
		}

		return handler->_iterate_start->data;
	}

	if (handler->_iterate_cur == handler->_iterate_start) {
		handler->_stat.iterate ++;
		return NULL;
	}

	ptr = handler->_iterate_cur;
	handler->cll = ptr;

	handler->_iterate_cur = handler->_iterate_reverse ? handler->_iterate_cur->prev : handler->_iterate_cur->next;

	return ptr->data;
}

static void _cll_rewind(struct cll_handler *handler, int to) {
	handler->_iterate_start = NULL;
	handler->_iterate_cur = NULL;
	handler->_iterate_reverse = to;

	handler->_stat.rewind ++;
}

static ui32_t _cll_set_config(struct cll_handler *handler, ui32_t flags) {
	ui32_t old_flags = flags;

	handler->_config_flags = flags;

	if (flags & CONFIG_INSERT_TAIL) {
		handler->insert = &_cll_insert_tail;
	} else if (flags & CONFIG_INSERT_HEAD) {
		handler->insert = &_cll_insert_head;
	} else if (flags & CONFIG_INSERT_SORTED) {
		handler->insert = &_cll_insert_sorted;
	} else {
		handler->insert = &_cll_insert;
	}

	handler->_stat.set_config ++;

	return old_flags;
}

static ui32_t _cll_get_config(struct cll_handler *handler) {
	handler->_stat.get_config ++;

	return handler->_config_flags;
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct cll_handler *pall_cll_init(
		int (*compare) (const void *src, const void *dst),
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd))
{
	struct cll_handler *handler = NULL;

	if (!destroy) {
		errno = EINVAL;
		return NULL;
	}

	if (!(handler = (struct cll_handler *) mm_alloc(sizeof(struct cll_handler))))
		return NULL;

	memset(handler, 0, sizeof(struct cll_handler));

	handler->cll = NULL;
	handler->cll_head = NULL;
	handler->_iterate_start = NULL;
	handler->_iterate_cur = NULL;
	handler->_config_flags = CONFIG_SEARCH_LRU | CONFIG_INSERT_HEAD;

	handler->compare = compare;
	handler->destroy = destroy;
	handler->ser_data = ser_data;
	handler->unser_data = unser_data;

	handler->insert = &_cll_insert;
	handler->del = &_cll_delete;
	handler->search = compare ? &_cll_search : &_cll_search_nop;
	handler->serialize = &_cll_serialize;
	handler->unserialize = &_cll_unserialize;
	handler->stat = &_cll_stat;
	handler->stat_reset = &_cll_stat_reset;
	handler->count = &_cll_count;
	handler->pope = &_cll_pope;
	handler->poph = &_cll_poph;
	handler->collapse = &_cll_collapse;
	handler->iterate = &_cll_iterate;
	handler->rewind = &_cll_rewind;
	handler->count = &_cll_count;
	handler->set_config = &_cll_set_config;
	handler->get_config = &_cll_get_config;

	return handler;
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_cll_destroy(struct cll_handler *h) {
	h->collapse(h);

	mm_free(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_cll_insert(struct cll_handler *h, void *data) {
	return h->insert(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_cll_delete(struct cll_handler *h, void *data) {
	return h->del(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_cll_search(struct cll_handler *h, void *data) {
	return h->search(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_cll_serialize(struct cll_handler *h, pall_fd_t fd) {
	return h->serialize(h, fd);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_cll_unserialize(struct cll_handler *h, pall_fd_t fd) {
	return h->unserialize(h, fd);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct cll_stat *pall_cll_stat(struct cll_handler *h) {
	return h->stat(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_cll_stat_reset(struct cll_handler *h) {
	h->stat_reset(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_cll_count(struct cll_handler *h) {
	return h->count(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_cll_pope(struct cll_handler *h, void *data) {
	return h->pope(h, data);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_cll_poph(struct cll_handler *h) {
	return h->poph(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_cll_collapse(struct cll_handler *h) {
	h->collapse(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_cll_iterate(struct cll_handler *h) {
	return h->iterate(h);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_cll_rewind(struct cll_handler *h, int to) {
	h->rewind(h, to);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_cll_set_config(struct cll_handler *h, ui32_t flags) {
	return h->set_config(h, flags);
}

#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_cll_get_config(struct cll_handler *h) {
	return h->get_config(h);
}

