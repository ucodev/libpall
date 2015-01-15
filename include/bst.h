/**
 * @file bst.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Binary Search Tree interface header
 *
 * Date: 11-10-2012
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

#ifndef LIBPALL_BST_H
#define LIBPALL_BST_H

#include "config.h"
#include "pall.h"
#include "fifo.h"
#include "lifo.h"


/* Structures */

/**
 * @struct bst_stat
 *
 * @brief
 *   Statistical counters for tree operations and tree elements.
 *
 * @see pall_bst_stat()
 * @see pall_bst_stat_reset()
 *
 * @var bst_stat::insert
 *   Number of successful inserts
 *
 * @var bst_stat::insert_err
 *   Number of failed inserts
 *
 * @var bst_stat::del
 *   Number of successful deletes
 *
 * @var bst_stat::del_nf
 *   Number of not found deletes
 *
 * @var bst_stat::search
 *   Number of successful searches
 *
 * @var bst_stat::search_nf
 *   Number of not found searches
 *
 * @var bst_stat::serialize
 *   Number of successful serializations
 *
 * @var bst_stat::serialize_err
 *   Number of failed serializations
 *
 * @var bst_stat::unserialize
 *   Number of successful unserializations
 *
 * @var bst_stat::unserialize_err
 *   Number of failed unserializations
 *
 * @var bst_stat::stat
 *   Number of stat calls
 *
 * @var bst_stat::count
 *   Number of count calls
 *
 * @var bst_stat::collapse
 *   Number of collapse calls
 *
 * @var bst_stat::iterate
 *   Number of full iterations
 *
 * @var bst_stat::rewind
 *   Number of rewind calls
 *
 * @var bst_stat::elem_count_cur
 *   Current number of elements present on the tree
 *
 * @var bst_stat::elem_count_max
 *   Maximum elements since initialization or last stat_reset call.
 *
 */
struct bst_stat {
	/* Operation statistics */
	unsigned long insert;
	unsigned long insert_err;
	unsigned long del;
	unsigned long del_nf;
	unsigned long search;
	unsigned long search_nf;
	unsigned long serialize;
	unsigned long serialize_err;
	unsigned long unserialize;
	unsigned long unserialize_err;
	unsigned long stat;
	unsigned long count;
	unsigned long pope;
	unsigned long collapse;
	unsigned long iterate;
	unsigned long rewind;

	/* Element statistics */
	unsigned long elem_count_cur;
	unsigned long elem_count_max;
};

/**
 * @struct bst_handler
 *
 * @brief
 *   Data structure defining the Binary Search Tree handler.
 *
 * @var bst_handler::root
 *   The BST root pointer.
 *
 * @var bst_handler::insert
 *   Function pointer performing the same operation of pall_bst_insert()
 *
 * @var bst_handler::del
 *   Function pointer performing the same operation of pall_bst_delete()
 *
 * @var bst_handler::search
 *   Function pointer performing the same operation of pall_bst_search()
 *
 * @var bst_handler::serialize
 *   Function pointer performing the same operation of pall_bst_serialize()
 *
 * @var bst_handler::unserialize
 *   Function pointer performing the same operation of pall_bst_unserialize()
 *
 * @var bst_handler::stat
 *   Function pointer performing the same operation of pall_bst_stat()
 *
 * @var bst_handler::stat_reset
 *   Function pointer performing the same operation of pall_bst_stat_reset()
 *
 * @var bst_handler::count
 *   Function pointer performing the same operation of pall_bst_count()
 *
 * @var bst_handler::collapse
 *   Function pointer performing the same operation of pall_bst_collapse()
 *
 * @var bst_handler::iterate
 *   Function pointer performing the same operation of pall_bst_iterate()
 *
 * @var bst_handler::rewind
 *   Function pointer performing the same operation of pall_bst_rewind()
 *
 */
struct bst_handler {
	void *root;
	struct fifo_handler *_iterate_forward;
	struct lifo_handler *_iterate_backward;
	int _iterate_reverse;
	pall_fd_t _ser_cur_fd;
	int _ser_ret;
	ui32_t _count;

	struct bst_stat _stat;
	int (*compare) (const void *d1, const void *d2);
	void (*destroy) (void *data);
	int (*ser_data) (pall_fd_t fd, void *data);
	void *(*unser_data) (pall_fd_t fd);

	int (*insert) (struct bst_handler *handler, void *data);
	int (*del) (struct bst_handler *handler, void *data);
	void *(*search) (struct bst_handler *handler, void *data);
	int (*serialize) (struct bst_handler *handler, pall_fd_t fd);
	int (*unserialize) (struct bst_handler *handler, pall_fd_t fd);
	struct bst_stat *(*stat) (struct bst_handler *handler);
	void (*stat_reset) (struct bst_handler *handler);
	ui32_t (*count) (struct bst_handler *handler);
	void (*collapse) (struct bst_handler *handler);
	void *(*iterate) (struct bst_handler *handler);
	void (*rewind) (struct bst_handler *handler, int to);
};

struct bst_node {
	void *data;
	struct bst_handler *h;
};


/* Prototypes / Interface */

/**
 * @brief
 *   Initializes a Binary Search Tree handler.
 *
 * @param compare
 *   Internally used function for element comparision by pall_bst_insert(),
 *   pall_bst_search() and pall_bst_delete().
 *   It receives two elements as parameters of type const void *.
 *   It shall return an integer less than, equal to, or greater than zero if
 *   d1 is found, respectively, to be less than, to match, or to be greater
 *   than d2.
 *
 * @param destroy
 *   Internally used function for memory deallocation, on pall_bst_delete() and
 *   pall_bst_collapse(), of the element pointed by its parameter of type
 *   void *.
 *
 * @param ser_data
 *   Internally used function for element serialization.
 *   This is an optional argument and NULL shall be used to disable
 *   serialization support, causing serialization calls (pall_bst_serialize())
 *   to fail, setting errno to ENOSYS.
 *
 * @param unser_data
 *   Internally used function for element unserialization.
 *   This is an optional argument and NULL shall be used to disable
 *   unserialization support, causing unserialization calls
 *   (pall_bst_unserialize()) to fail, setting errno to ENOSYS.
 *
 * @return 
 *   On success, a pointer to a valid Binary Search Tree handler is returned.
 *   On error, NULL is returned, and errno is set appropriately.
 *   \n\n
 *   Errors: EINVAL, ENOMEM
 *
 * @see pall_bst_insert()
 * @see pall_bst_search()
 * @see pall_bst_delete()
 * @see pall_bst_serialize()
 * @see pall_bst_unserialize()
 * @see pall_bst_destroy()
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct bst_handler *pall_bst_init(
		int (*compare) (const void *d1, const void *d2),
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd));

/**
 * @brief
 *   Unitializes and release all resources of a Binary Search Tree handler
 *   pointed by parameter 'h'.
 *
 * @see pall_bst_init()
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_destroy(struct bst_handler *h);

/**
 * @brief
 *   Inserts an element pointed by 'data' into the Binary Search Tree pointed
 *   by 'h'.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @param data
 *   A pointer to the element to be inserted.
 *
 * @return
 *   On success, zero is returned and statistical counter 'insert' is
 *   incremented. On error, -1 is returned, statistical counter 'insert_err' is
 *   incremented, and errno is set appropriately.
 *   \n\n
 *   Errors: ENOMEM
 *
 * @see pall_bst_init()
 * @see pall_bst_delete()
 * @see pall_bst_search()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_insert(struct bst_handler *h, void *data);

/**
 * @brief
 *   Deletes an element that matches the contents of the element pointed by
 *   'data' from the Binary Search Tree pointed by 'h'. The comparision
 *   of the elements is performed by the compare() function passed to
 *   pall_bst_init() function.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched and deleted.
 *
 * @return
 *   On success, zero is returned and statistical counter 'del' is incremented.
 *   If the element is not found, -1 is returned and statistical counter
 *   'del_nf' is incremented.
 *
 * @see pall_bst_init()
 * @see pall_bst_insert()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_delete(struct bst_handler *h, void *data);

/**
 * @brief
 *   Searches an element that matches the contents of the element pointed by
 *   'data' on the Binary Search Tree pointed by 'h'. The comparision
 *   of the elements is performed by the compare() function passed to
 *   pall_bst_init() function.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched.
 *
 * @return
 *   On success, a pointer to the found element is returned and statistical
 *   counter 'search' is incremented. If the element is not found, NULL is
 *   returned and statistical counter 'search_nf' is incremented.
 *
 * @see pall_bst_init()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_bst_search(struct bst_handler *h, void *data);

/**
 * @brief
 *   Serializes the contents of the Binary Search Tree pointed by 'h', to
 *   the file descriptor 'fd'.
 *   The metadata of the tree is serialized in network byte order.
 *   Each element is serialized through the ser_data() function passed to
 *   pall_bst_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
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
 * @see pall_bst_init()
 * @see pall_bst_unserialize()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_serialize(struct bst_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Unserializes the contents from the file descriptor 'fd' into the Binary
 *   Search Tree pointed by 'h'.
 *   The unserialized data is converted from network byte order to host byte
 *   order.
 *   Each element is serialized through the unser_data() function passed to
 *   pall_bst_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
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
 * @see pall_bst_init()
 * @see pall_bst_serialize()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
int pall_bst_unserialize(struct bst_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Returns statistical information for operations and content of the Binary
 *   Search Tree pointed by handler 'h'. This function shall be called for
 *   each time updated statistical counters are required.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @return
 *   Returns a pointer to a valid struct bst_stat and the statistical counter
 *   'stat' is incremented. This function always succeeds.
 *
 * @see pall_bst_init()
 * @see pall_bst_stat_reset()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
struct bst_stat *pall_bst_stat(struct bst_handler *h);

/**
 * @brief
 *   Resets the statistical counters of the Binary Search Tree pointed by
 *   handler 'h'.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @see pall_bst_init()
 * @see pall_bst_stat()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_stat_reset(struct bst_handler *h);

/**
 * @brief
 *   Returns the number of elements of the Binary Search Tree pointed
 *   by handler 'h'.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @return
 *   Returns a positive integer of type ui32_t (32 bit unsigned) if the tree
 *   isn't empty. If it's empty, zero is returned. Statistical counter 'count'
 *   is always incremented when this function retruns.
 *
 * @see pall_bst_init()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
ui32_t pall_bst_count(struct bst_handler *h);

/**
 * @brief
 *   Removes all the elements from the Binary Search Tree pointed by handler
 *   'h'.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @return
 *   This function doesn't return any value. The statistical counter 'collapse'
 *   is incremented on return.
 *
 * @see pall_bst_init()
 * @see pall_bst_delete()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_collapse(struct bst_handler *h);

/**
 * @brief
 *   Iterates through the elements of the Binary Search Tree pointed by
 *   handler 'h'. Each call to this function returns a pointer to the next
 *   element present on the tree. Iteration may be performed from array index
 *   zero to arr_size, head to tail for each indexed list, or from arr_size to
 *   zero, tail to head for each indexed list, depending on the parameters used
 *   on the pall_bst_rewind() function.
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @return
 *   Returns a pointer to the next element present on the list. If the end of
 *   the list is reached, NULL is returned and statistical counter 'iterate'
 *   is incremented.
 *
 * @see pall_bst_init()
 * @see pall_bst_rewind()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void *pall_bst_iterate(struct bst_handler *h);

/**
 * @brief
 *   Rewinds the Binary Search Tree pointed by handler 'h'. The parameter
 *   'to' tells to where the rewind should be perfomed and configures the
 *   the behavior of pall_bst_iterate().
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Binary Search Tree handler.
 *
 * @param to
 *   If set to 0, the tree is rewinded to array index zero, and each list is
 *   rewinded to head. pall_bst_iterate() will iterate the tree from array
 *   index zero to arr_size and each indexed list from head to tail.
 *   If set to 1, the tree is rewinded to arr_size index, and each list is
 *   rewinded to tail. pall_bst_iterate() will iterate the tree from array
 *   index arr_size to zero and each index list from tail to head.
 *
 * @return
 *   No value is returned and statistical counter 'rewind' is incremented for
 *   each time this function returns.
 *
 * @see pall_bst_init()
 * @see pall_bst_iterate()
 * @see bst_stat
 *
 */
#ifdef COMPILE_WIN32
DLLIMPORT
#endif
void pall_bst_rewind(struct bst_handler *h, int to);

#endif

