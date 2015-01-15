/**
 * @file lifo.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Last In First Out interface header
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

#ifndef LIBPALL_LIFO_H
#define LIBPALL_LIFO_H

#include "config.h"
#include "pall.h"
#include "cll.h"

/* Structures */

/**
 * @struct lifo_stat
 *
 * @brief
 *   Statistical counters for stack operations and stack elements.
 *
 * @see pall_lifo_stat()
 * @see pall_lifo_stat_reset()
 *
 * @var lifo_stat::push
 *   Number of successful pushes
 *
 * @var lifo_stat::push_err
 *   Number of failed pushes
 *
 * @var lifo_stat::pop
 *   Number of successful pops
 *
 * @var lifo_stat::pop_nf
 *   Number of not found pops
 *
 * @var lifo_stat::serialize
 *   Number of successful serializations
 *
 * @var lifo_stat::serialize_err
 *   Number of failed serializations
 *
 * @var lifo_stat::unserialize
 *   Number of successful unserializations
 *
 * @var lifo_stat::unserialize_err
 *   Number of failed unserializations
 *
 * @var lifo_stat::stat
 *   Number of stat calls
 *
 * @var lifo_stat::count
 *   Number of count calls
 *
 * @var lifo_stat::collapse
 *   Number of collapse calls
 *
 * @var lifo_stat::iterate
 *   Number of full iterations
 *
 * @var lifo_stat::rewind
 *   Number of rewind calls
 *
 * @var lifo_stat::elem_count_cur
 *   Current number of elements present on the list
 *
 * @var lifo_stat::elem_count_max
 *   Maximum elements since initialization or last stat_reset call.
 *
 */
struct lifo_stat {
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
 * @struct lifo_handler
 *
 * @brief
 *   Data structure defining the Last In First Out stack handler.
 *
 * @var lifo_handler::push
 *   Function pointer performing the same operation of pall_lifo_push()
 *
 * @var lifo_handler::pop
 *   Function pointer performing the same operation of pall_lifo_pop()
 *
 * @var lifo_handler::serialize
 *   Function pointer performing the same operation of pall_lifo_serialize()
 *
 * @var lifo_handler::unserialize
 *   Function pointer performing the same operation of pall_lifo_unserialize()
 *
 * @var lifo_handler::stat
 *   Function pointer performing the same operation of pall_lifo_stat()
 *
 * @var lifo_handler::stat_reset
 *   Function pointer performing the same operation of pall_lifo_stat_reset()
 *
 * @var lifo_handler::count
 *   Function pointer performing the same operation of pall_lifo_count()
 *
 * @var lifo_handler::collapse
 *   Function pointer performing the same operation of pall_lifo_collapse()
 *
 * @var lifo_handler::iterate
 *   Function pointer performing the same operation of pall_lifo_iterate()
 *
 * @var lifo_handler::rewind
 *   Function pointer performing the same operation of pall_lifo_rewind()
 *
 */
struct lifo_handler {
	struct cll_handler *lifo;
	struct lifo_stat _stat;

	void (*destroy) (void *data);
	int (*ser_data) (pall_fd_t fd, void *data);
	void *(*unser_data) (pall_fd_t fd);

	int (*push) (struct lifo_handler *handler, void *data);
	void *(*pop) (struct lifo_handler *handler);
	int (*serialize) (struct lifo_handler *handler, pall_fd_t fd);
	int (*unserialize) (struct lifo_handler *handler, pall_fd_t fd);
	struct lifo_stat *(*stat) (struct lifo_handler *handler);
	void (*stat_reset) (struct lifo_handler *handler);
	ui32_t (*count) (struct lifo_handler *handler);
	void (*collapse) (struct lifo_handler *handler);
	void *(*iterate) (struct lifo_handler *handler);
	void (*rewind) (struct lifo_handler *handler, int to);
};


/* Prototypes / Interface */

/**
 * @brief
 *   Initializes a Last In First Out stack handler.
 *
 * @param destroy
 *   Internally used function for memory deallocation, on pall_lifo_collapse(),
 *   of the element pointed by its parameter of type void *.
 *
 * @param ser_data
 *   Internally used function for element serialization.
 *   This is an optional argument and NULL shall be used to disable
 *   serialization support, causing serialization calls (pall_lifo_serialize())
 *   to fail, setting errno to ENOSYS.
 *
 * @param unser_data
 *   Internally used function for element unserialization.
 *   This is an optional argument and NULL shall be used to disable
 *   unserialization support, causing unserialization calls
 *   (pall_lifo_unserialize()) to fail, setting errno to ENOSYS.
 *
 * @return 
 *   On success, a pointer to a valid Last In First Out stack handler is
 *   returned.
 *   On error, NULL is returned, and errno is set appropriately.
 *   \n\n
 *   Errors: EINVAL, ENOMEM
 *
 * @see pall_lifo_push()
 * @see pall_lifo_pop()
 * @see pall_lifo_serialize()
 * @see pall_lifo_unserialize()
 * @see pall_lifo_destroy()
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct lifo_handler *pall_lifo_init(
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd));

/**
 * @brief
 *   Unitializes and release all resources of a Last In First Out stack handler
 *   pointed by parameter 'h'.
 *
 * @see pall_lifo_init()
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_lifo_destroy(struct lifo_handler *h);

/**
 * @brief
 *   Pushes an element pointed by 'data' into the Last In First Out stack
 *   pointed by 'h'.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
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
 * @see pall_lifo_init()
 * @see pall_lifo_pop()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_lifo_push(struct lifo_handler *h, void *data);

/**
 * @brief
 *   Pops an element from the Last In First Out stack pointed by 'h'.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @return
 *   On success, a pointer to the popped element is returned and the
 *   statistical counter 'pop' is incremented. If the stack is empty, NULL is
 *   returned and the statistical counter 'pop_nf' is incremented.
 *
 * @see pall_lifo_init()
 * @see pall_lifo_push()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_lifo_pop(struct lifo_handler *h);

/**
 * @brief
 *   Serializes the contents of the Last In First Out stack pointed by 'h', to
 *   the file descriptor 'fd'.
 *   The metadata of the stack is serialized in network byte order.
 *   Each element is serialized through the ser_data() function passed to
 *   pall_lifo_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
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
 * @see pall_lifo_init()
 * @see pall_lifo_unserialize()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_lifo_serialize(struct lifo_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Unserializes the contents from the file descriptor 'fd' into the Last In
 *   First Out stack pointed by 'h'.
 *   The unserialized data is converted from network byte order to host byte
 *   order.
 *   Each element is serialized through the unser_data() function passed to
 *   pall_lifo_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
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
 * @see pall_lifo_init()
 * @see pall_lifo_serialize()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_lifo_unserialize(struct lifo_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Returns statistical information for operations and content of the Last In
 *   First Out stack pointed by 'h'. This function shall be called for each
 *   time updated statistical counters are required.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @return
 *   Returns a pointer to a valid struct lifo_stat and the statistical counter
 *   'stat' is incremented. This function always succeeds.
 *
 * @see pall_lifo_init()
 * @see pall_lifo_stat_reset()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct lifo_stat *pall_lifo_stat(struct lifo_handler *h);

/**
 * @brief
 *   Resets the statistical counters of the Last In First Out stack pointed by
 *   'h'.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @see pall_lifo_init()
 * @see pall_lifo_stat()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_lifo_stat_reset(struct lifo_handler *h);

/**
 * @brief
 *   Returns the number of elements of the Last In First Out stack pointed by
 *   'h'.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @return
 *   Returns a positive integer of type ui32_t (32 bit unsigned) if the list
 *   isn't empty. If it's empty, zero is returned. Statistical counter 'count'
 *   is always incremented when this function retruns.
 *
 * @see pall_lifo_init()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_lifo_count(struct lifo_handler *h);

/**
 * @brief
 *   Removes all the elements from the Last In First Out stack pointed by 'h'.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @return
 *   This function doesn't return any value. The statistical counter 'collapse'
 *   is incremented on return.
 *
 * @see pall_lifo_init()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_lifo_collapse(struct lifo_handler *h);

/**
 * @brief
 *   Iterates through the elements of the Last In First Out stack pointed by
 *   'h'. Each call to this function returns a pointer to the next element
 *   present on the stack. Iteration may be performed from head to tail or from
 *   tail to head, depending on the parameters used on the pall_lifo_rewind()
 *   function.
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @return
 *   Returns a pointer to the next element present on the stack. If the bottom
 *   of the stack is reached, NULL is returned and statistical counter 'iterate'
 *   is incremented.
 *
 * @see pall_lifo_init()
 * @see pall_lifo_rewind()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_lifo_iterate(struct lifo_handler *h);

/**
 * @brief
 *   Rewinds the Last In First Out stack pointed by 'h'. The parameter 'to'
 *   tells to where the rewind should be perfomed and configures the the
 *   behavior of pall_lifo_iterate().
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Last In First Out stack handler.
 *
 * @param to
 *   If set to 0, the stack is rewinded to head and pall_lifo_iterate() will
 *   iterate the stack from head to tail.
 *   If set to 1, the stack is rewinded to tail and pall_lifo_iterate() will
 *   iterate the list from tail to head.
 *
 * @return
 *   No value is returned and statistical counter 'rewind' is incremented for
 *   each time this function returns.
 *
 * @see pall_lifo_init()
 * @see pall_lifo_iterate()
 * @see lifo_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_lifo_rewind(struct lifo_handler *h, int to);

#endif

