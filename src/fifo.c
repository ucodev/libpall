/**
 * @file fifo.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        First In First Out interface
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
#include "fifo.h"
#include "cll.h"

static int _fifo_push(struct fifo_handler *handler, void *data) {
	return handler->fifo->insert(handler->fifo, data);
}

static void *_fifo_pop(struct fifo_handler *handler) {
	return handler->fifo->poph(handler->fifo);
}

static int _fifo_serialize(struct fifo_handler *handler, pall_fd_t fd) {
	return handler->fifo->serialize(handler->fifo, fd);
}

static int _fifo_unserialize(struct fifo_handler *handler, pall_fd_t fd) {
	return handler->fifo->unserialize(handler->fifo, fd);
}

static struct fifo_stat *_fifo_stat(struct fifo_handler *handler) {
	handler->_stat.push = handler->fifo->stat(handler->fifo)->insert;
	handler->_stat.push_err = handler->fifo->stat(handler->fifo)->insert_err;
	handler->_stat.pop = handler->fifo->stat(handler->fifo)->poph;
	handler->_stat.pop_nf = handler->fifo->stat(handler->fifo)->poph_nf;
	handler->_stat.serialize = handler->fifo->stat(handler->fifo)->serialize;
	handler->_stat.serialize_err = handler->fifo->stat(handler->fifo)->serialize_err;
	handler->_stat.unserialize = handler->fifo->stat(handler->fifo)->unserialize;
	handler->_stat.unserialize_err = handler->fifo->stat(handler->fifo)->unserialize_err;
	handler->_stat.count = handler->fifo->stat(handler->fifo)->count;
	handler->_stat.collapse = handler->fifo->stat(handler->fifo)->collapse;
	handler->_stat.iterate = handler->fifo->stat(handler->fifo)->iterate;
	handler->_stat.rewind = handler->fifo->stat(handler->fifo)->rewind;
	handler->_stat.elem_count_cur = handler->fifo->stat(handler->fifo)->elem_count_cur;
	handler->_stat.elem_count_max = handler->fifo->stat(handler->fifo)->elem_count_max;

	handler->_stat.stat ++;

	return &handler->_stat;
}

static void _fifo_stat_reset(struct fifo_handler *handler) {
	memset(&handler->_stat, 0, sizeof(struct fifo_stat));
}

static ui32_t _fifo_count(struct fifo_handler *handler) {
	return handler->fifo->count(handler->fifo);
}

static void _fifo_collapse(struct fifo_handler *handler) {
	handler->fifo->collapse(handler->fifo);
}

static void *_fifo_iterate(struct fifo_handler *handler) {
	return handler->fifo->iterate(handler->fifo);
}

static void _fifo_rewind(struct fifo_handler *handler, int to) {
	handler->fifo->rewind(handler->fifo, to);
}

struct fifo_handler *pall_fifo_init(
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd))
{
	struct fifo_handler *handler = NULL;

	if (!destroy) {
		errno = EINVAL;
		return NULL;
	}

	if (!(handler = (struct fifo_handler *) mm_alloc(sizeof(struct fifo_handler))))
		return NULL;

	memset(handler, 0, sizeof(struct fifo_handler));

	if (!(handler->fifo = pall_cll_init(NULL, destroy, ser_data, unser_data)))
		return NULL;

	handler->fifo->set_config(handler->fifo, CONFIG_INSERT_TAIL);

	handler->destroy = destroy;
	handler->ser_data = ser_data;
	handler->unser_data = unser_data;

	handler->push = &_fifo_push;
	handler->pop = &_fifo_pop;
	handler->serialize = &_fifo_serialize;
	handler->unserialize = &_fifo_unserialize;
	handler->stat = &_fifo_stat;
	handler->stat_reset = &_fifo_stat_reset;
	handler->count = &_fifo_count;
	handler->collapse = &_fifo_collapse;
	handler->iterate = &_fifo_iterate;
	handler->rewind = &_fifo_rewind;

	return handler;
}

void pall_fifo_destroy(struct fifo_handler *h) {
	h->collapse(h);

	pall_cll_destroy(h->fifo);

	mm_free(h);
}

int pall_fifo_push(struct fifo_handler *h, void *data) {
	return h->push(h, data);
}

void *pall_fifo_pop(struct fifo_handler *h) {
	return h->pop(h);
}

int pall_fifo_serialize(struct fifo_handler *h, pall_fd_t fd) {
	return h->serialize(h, fd);
}

int pall_fifo_unserialize(struct fifo_handler *h, pall_fd_t fd) {
	return h->unserialize(h, fd);
}

struct fifo_stat *pall_fifo_stat(struct fifo_handler *h) {
	return h->stat(h);
}

void pall_fifo_stat_reset(struct fifo_handler *h) {
	h->stat_reset(h);
}

ui32_t pall_fifo_count(struct fifo_handler *h) {
	return h->count(h);
}

void pall_fifo_collapse(struct fifo_handler *h) {
	h->collapse(h);
}

void *pall_fifo_iterate(struct fifo_handler *h) {
	return h->iterate(h);
}

void pall_fifo_rewind(struct fifo_handler *h, int to) {
	h->rewind(h, to);
}

