/**
 * @file lifo.c
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Last In First Out interface
 *
 * Date: 13-05-2014
 * 
 * Copyright 2012-2014 Pedro A. Hortas (pah@ucodev.org)
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

#include "mm.h"
#include "pall.h"
#include "lifo.h"
#include "cll.h"

static int _lifo_push(struct lifo_handler *handler, void *data) {
	return handler->lifo->insert(handler->lifo, data);
}

static void *_lifo_pop(struct lifo_handler *handler) {
	return handler->lifo->poph(handler->lifo);
}

static int _lifo_serialize(struct lifo_handler *handler, pall_fd_t fd) {
	return handler->lifo->serialize(handler->lifo, fd);
}

static int _lifo_unserialize(struct lifo_handler *handler, pall_fd_t fd) {
	return handler->lifo->unserialize(handler->lifo, fd);
}

static struct lifo_stat *_lifo_stat(struct lifo_handler *handler) {
	handler->_stat.push = handler->lifo->stat(handler->lifo)->insert;
	handler->_stat.push_err = handler->lifo->stat(handler->lifo)->insert_err;
	handler->_stat.pop = handler->lifo->stat(handler->lifo)->poph;
	handler->_stat.pop_nf = handler->lifo->stat(handler->lifo)->poph_nf;
	handler->_stat.serialize = handler->lifo->stat(handler->lifo)->serialize;
	handler->_stat.serialize_err = handler->lifo->stat(handler->lifo)->serialize_err;
	handler->_stat.unserialize = handler->lifo->stat(handler->lifo)->unserialize;
	handler->_stat.unserialize_err = handler->lifo->stat(handler->lifo)->unserialize_err;
	handler->_stat.count = handler->lifo->stat(handler->lifo)->count;
	handler->_stat.collapse = handler->lifo->stat(handler->lifo)->collapse;
	handler->_stat.iterate = handler->lifo->stat(handler->lifo)->iterate;
	handler->_stat.rewind = handler->lifo->stat(handler->lifo)->rewind;
	handler->_stat.elem_count_cur = handler->lifo->stat(handler->lifo)->elem_count_cur;
	handler->_stat.elem_count_max = handler->lifo->stat(handler->lifo)->elem_count_max;

	handler->_stat.stat ++;

	return &handler->_stat;
}

static void _lifo_stat_reset(struct lifo_handler *handler) {
	memset(&handler->_stat, 0, sizeof(struct lifo_stat));
}

static ui32_t _lifo_count(struct lifo_handler *handler) {
	return handler->lifo->count(handler->lifo);
}

static void _lifo_collapse(struct lifo_handler *handler) {
	handler->lifo->collapse(handler->lifo);
}

static void *_lifo_iterate(struct lifo_handler *handler) {
	return handler->lifo->iterate(handler->lifo);
}

static void _lifo_rewind(struct lifo_handler *handler, int to) {
	handler->lifo->rewind(handler->lifo, to);
}

struct lifo_handler *pall_lifo_init(
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd))
{
	struct lifo_handler *handler = NULL;

	if (!destroy) {
		errno = EINVAL;
		return NULL;
	}

	if (!(handler = (struct lifo_handler *) mm_alloc(sizeof(struct lifo_handler))))
		return NULL;

	memset(handler, 0, sizeof(struct lifo_handler));

	if (!(handler->lifo = pall_cll_init(NULL, destroy, ser_data, unser_data)))
		return NULL;

	handler->lifo->set_config(handler->lifo, CONFIG_INSERT_HEAD);

	handler->destroy = destroy;
	handler->ser_data = ser_data;
	handler->unser_data = unser_data;

	handler->push = &_lifo_push;
	handler->pop = &_lifo_pop;
	handler->serialize = &_lifo_serialize;
	handler->unserialize = &_lifo_unserialize;
	handler->stat = &_lifo_stat;
	handler->stat_reset = &_lifo_stat_reset;
	handler->count = &_lifo_count;
	handler->collapse = &_lifo_collapse;
	handler->iterate = &_lifo_iterate;
	handler->rewind = &_lifo_rewind;

	return handler;
}

void pall_lifo_destroy(struct lifo_handler *handler) {
	handler->collapse(handler);

	pall_cll_destroy(handler->lifo);

	mm_free(handler);
}

int pall_lifo_push(struct lifo_handler *h, void *data) {
	return h->push(h, data);
}

void *pall_lifo_pop(struct lifo_handler *h) {
	return h->pop(h);
}

int pall_lifo_serialize(struct lifo_handler *h, pall_fd_t fd) {
	return h->serialize(h, fd);
}

int pall_lifo_unserialize(struct lifo_handler *h, pall_fd_t fd) {
	return h->unserialize(h, fd);
}

struct lifo_stat *pall_lifo_stat(struct lifo_handler *h) {
	return h->stat(h);
}

void pall_lifo_stat_reset(struct lifo_handler *h) {
	h->stat_reset(h);
}

ui32_t pall_lifo_count(struct lifo_handler *h) {
	return h->count(h);
}

void pall_lifo_collapse(struct lifo_handler *h) {
	h->collapse(h);
}

void *pall_lifo_iterate(struct lifo_handler *h) {
	return h->iterate(h);
}

void pall_lifo_rewind(struct lifo_handler *h, int to) {
	h->rewind(h, to);
}

