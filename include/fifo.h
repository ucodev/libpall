/**
 * @file fifo.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        First In First Out interface header
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

#ifndef LIBPALL_FIFO_H
#define LIBPALL_FIFO_H

#include "config.h"
#include "pall.h"
#include "cll.h"


/* Structures */

/**
 * @struct fifo_stat
 *
 * @brief
 *   Statistical counters for queue operations and queue elements.
 *
 * @see pall_fifo_stat()
 * @see pall_fifo_stat_reset()
 *
 * @var fifo_stat::push
 *   Number of successful pushes
 *
 * @var fifo_stat::push_err
 *   Number of failed pushes
 *
 * @var fifo_stat::pop
 *   Number of successful pops
 *
 * @var fifo_stat::pop_nf
 *   Number of not found pops
 *
 * @var fifo_stat::serialize
 *   Number of successful serializations
 *
 * @var fifo_stat::serialize_err
 *   Number of failed serializations
 *
 * @var fifo_stat::unserialize
 *   Number of successful unserializations
 *
 * @var fifo_stat::unserialize_err
 *   Number of failed unserializations
 *
 * @var fifo_stat::stat
 *   Number of stat calls
 *
 * @var fifo_stat::count
 *   Number of count calls
 *
 * @var fifo_stat::collapse
 *   Number of collapse calls
 *
 * @var fifo_stat::iterate
 *   Number of full iterations
 *
 * @var fifo_stat::rewind
 *   Number of rewind calls
 *
 * @var fifo_stat::elem_count_cur
 *   Current number of elements present on the list
 *
 * @var fifo_stat::elem_count_max
 *   Maximum elements since initialization or last stat_reset call.
 *
 */
struct fifo_stat {
	/* Operation statistics */
	unsigned long push;
	unsigned long push_err;
	unsigned long pop;
	unsigned long pop_nf;
	unsigned long serialize;
	unsigned long serialize_err;
	unsigned long unserialize;
	unsigned long unserialize_err;
	unsigned long stat;
	unsigned long count;
	unsigned long collapse;
	unsigned long iterate;
	unsigned long rewind;

	/* Element statistics */
	unsigned long elem_count_cur;
	unsigned long elem_count_max;
};

/**
 * @struct fifo_handler
 *
 * @brief
 *   Data structure defining the First In First Out queue handler.
 *
 * @var fifo_handler::push
 *   Function pointer performing the same operation of pall_fifo_push()
 *
 * @var fifo_handler::pop
 *   Function pointer performing the same operation of pall_fifo_pop()
 *
 * @var fifo_handler::serialize
 *   Function pointer performing the same operation of pall_fifo_serialize()
 *
 * @var fifo_handler::unserialize
 *   Function pointer performing the same operation of pall_fifo_unserialize()
 *
 * @var fifo_handler::stat
 *   Function pointer performing the same operation of pall_fifo_stat()
 *
 * @var fifo_handler::stat_reset
 *   Function pointer performing the same operation of pall_fifo_stat_reset()
 *
 * @var fifo_handler::count
 *   Function pointer performing the same operation of pall_fifo_count()
 *
 * @var fifo_handler::collapse
 *   Function pointer performing the same operation of pall_fifo_collapse()
 *
 * @var fifo_handler::iterate
 *   Function pointer performing the same operation of pall_fifo_iterate()
 *
 * @var fifo_handler::rewind
 *   Function pointer performing the same operation of pall_fifo_rewind()
 *
 */
struct fifo_handler {
	struct cll_handler *fifo;
	struct fifo_stat _stat;

	void (*destroy) (void *data);
	int (*ser_data) (pall_fd_t fd, void *data);
	void *(*unser_data) (pall_fd_t fd);

	int (*push) (struct fifo_handler *handler, void *data);
	void *(*pop) (struct fifo_handler *handler);
	int (*serialize) (struct fifo_handler *handler, pall_fd_t fd);
	int (*unserialize) (struct fifo_handler *handler, pall_fd_t fd);
	struct fifo_stat *(*stat) (struct fifo_handler *handler);
	void (*stat_reset) (struct fifo_handler *handler);
	ui32_t (*count) (struct fifo_handler *handler);
	void (*collapse) (struct fifo_handler *handler);
	void *(*iterate) (struct fifo_handler *handler);
	void (*rewind) (struct fifo_handler *handler, int to);
};


/* Prototypes / Interface */

/**
 * @brief
 *   Initializes a First In First Out queue handler.
 *
 * @param destroy
 *   Internally used function for memory deallocation, on pall_fifo_collapse(),
 *   of the element pointed by its parameter of type void *.
 *
 * @param ser_data
 *   Internally used function for element serialization.
 *   This is an optional argument and NULL shall be used to disable
 *   serialization support, causing serialization calls (pall_fifo_serialize())
 *   to fail, setting errno to ENOSYS.
 *
 * @param unser_data
 *   Internally used function for element unserialization.
 *   This is an optional argument and NULL shall be used to disable
 *   unserialization support, causing unserialization calls
 *   (pall_fifo_unserialize()) to fail, setting errno to ENOSYS.
 *
 * @return 
 *   On success, a pointer to a valid First In First Out queue handler is
 *   returned.
 *   On error, NULL is returned, and errno is set appropriately.
 *   \n\n
 *   Errors: EINVAL, ENOMEM
 *
 * @see pall_fifo_push()
 * @see pall_fifo_pop()
 * @see pall_fifo_serialize()
 * @see pall_fifo_unserialize()
 * @see pall_fifo_destroy()
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct fifo_handler *pall_fifo_init(
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd));

/**
 * @brief
 *   Unitializes and release all resources of a First In First Out queue handler
 *   pointed by parameter 'h'.
 *
 * @see pall_fifo_init()
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_fifo_destroy(struct fifo_handler *h);

/**
 * @brief
 *   Pushes an element pointed by 'data' into the First In First Out queue
 *   handler pointed by 'h'.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @param data
 *   A pointer to the element to be pushed.
 *
 * @return
 *   On success, zero is returned and statistical counter 'push' is incremented.
 *   On error, -1 is returned, statistical counter 'push_err' is incremented,
 *   and errno is set appropriately.
 *   \n\n
 *   Errors: ENOMEM
 *
 * @see pall_fifo_init()
 * @see pall_fifo_pop()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_fifo_push(struct fifo_handler *h, void *data);

/**
 * @brief
 *   Pops an element from the First In First Out queue pointed by 'h'.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @return
 *   On success, a pointer to the popped element is returned and the
 *   statistical counter 'pop' is incremented. If the queue is empty, NULL is
 *   returned and the statistical counter 'pop_nf' is incremented.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_push()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_fifo_pop(struct fifo_handler *h);

/**
 * @brief
 *   Serializes the contents of the First In First Out queue pointed by 'h', to
 *   the file descriptor 'fd'.
 *   The metadata of the queue is serialized in network byte order.
 *   Each element is serialized through the ser_data() function passed to
 *   pall_fifo_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @param fd
 *   A writable file descriptor.
 *
 * @return
 *   On success, zero is returned and statistical counter 'serialize' is
 *   incremented. On error, -1 is returned, statistical 'serialize_err' is
 *   incremented, and errno is set appropriately.
 *   \n\n
 *   Errors: Same as write() and ENOSYS.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_unserialize()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_fifo_serialize(struct fifo_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Unserializes the contents from the file descriptor 'fd' into the First In
 *   First Out queue pointed by 'h'.
 *   The unserialized data is converted from network byte order to host byte
 *   order.
 *   Each element is serialized through the unser_data() function passed to
 *   pall_fifo_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @param fd
 *   A readable file descriptor.
 *
 * @return
 *   On success, zero is returned and statistical counter 'unserialize' is
 *   incremented. On error, -1 is returned, statistical counter
 *   'unserialize_err' is incremented and, errno is set appropriately.
 *   \n\n
 *   Errors: Same as read() and ENOSYS.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_serialize()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_fifo_unserialize(struct fifo_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Returns statistical information for operations and content of the First In
 *   First Out queue pointed by 'h'. This function shall be called for each
 *   time updated statistical counters are required.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @return
 *   Returns a pointer to a valid struct fifo_stat and the statistical counter
 *   'stat' is incremented. This function always succeeds.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_stat_reset()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct fifo_stat *pall_fifo_stat(struct fifo_handler *h);

/**
 * @brief
 *   Resets the statistical counters of the First In First Out queue pointed by
 *   'h'.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_stat()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_fifo_stat_reset(struct fifo_handler *h);

/**
 * @brief
 *   Returns the number of elements of the First In First Out queue pointed by
 *   'h'.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @return
 *   Returns a positive integer of type ui32_t (32 bit unsigned) if the list
 *   isn't empty. If it's empty, zero is returned. Statistical counter 'count'
 *   is always incremented when this function retruns.
 *
 * @see pall_fifo_init()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_fifo_count(struct fifo_handler *h);

/**
 * @brief
 *   Removes all the elements from the First In First Out queue pointed by
 *   'h'.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @return
 *   This function doesn't return any value. The statistical counter 'collapse'
 *   is incremented on return.
 *
 * @see pall_fifo_init()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_fifo_collapse(struct fifo_handler *h);

/**
 * @brief
 *   Iterates through the elements of the First In First Out queue pointed by
 *   'h'. Each call to this function returns a pointer to the next element
 *   present on the queue. Iteration may be performed from head to tail or from
 *   tail to head, depending on the parameters used on the pall_fifo_rewind()
 *   function.
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @return
 *   Returns a pointer to the next element present on the queue. If the end of
 *   the queue is reached, NULL is returned and statistical counter 'iterate'
 *   is incremented.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_rewind()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_fifo_iterate(struct fifo_handler *h);

/**
 * @brief
 *   Rewinds the First In First Out queue pointed by 'h'. The parameter 'to'
 *   tells to where the rewind should be perfomed and configures the the
 *   behavior of pall_fifo_iterate().
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized First In First Out queue handler.
 *
 * @param to
 *   If set to 0, the queue is rewinded to head and pall_fifo_iterate() will
 *   iterate the queue from head to tail.
 *   If set to 1, the queue is rewinded to tail and pall_fifo_iterate() will
 *   iterate the list from tail to head.
 *
 * @return
 *   No value is returned and statistical counter 'rewind' is incremented for
 *   each time this function returns.
 *
 * @see pall_fifo_init()
 * @see pall_fifo_iterate()
 * @see fifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_fifo_rewind(struct fifo_handler *h, int to);

#endif

