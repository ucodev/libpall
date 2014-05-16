/**
 * @file cll.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Circular Linked List interface header
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

#ifndef LIBPALL_CLL_H
#define LIBPALL_CLL_H

#include "pall.h"

/* Configuration Options */

/**
 * @def CONFIG_SEARCH_FORWARD
 *
 * @brief
 *   Configures the handler to perform forward searches (->next pointer).
 */
#define CONFIG_SEARCH_FORWARD	0x01
/**
 * @def CONFIG_SEARCH_BACKWARD
 *
 * @brief
 *   Configures the handler to perform backward searches (->prev pointer).
 */
#define CONFIG_SEARCH_BACKWARD	0x02
/**
 * @def CONFIG_SEARCH_AUTO
 *
 * @brief
 *   Configures the handler to perform automatic searches. This will use a
 *   the compare() function passed to handler initializer in order to identify
 *   the shortest path to the element (automatically choose ->next or ->prev
 *   pointer).
 */
#define CONFIG_SEARCH_AUTO	0x04
/**
 * @def CONFIG_SEARCH_LRU
 *
 * @brief
 *   Configures the handler to perform Least Recently Used searches. This will
 *   cause the search routine to start searching from the head of the list
 *   towards the tail. When the element is found, it is moved to the head of
 *   the list. This is the default configuration.
 */
#define CONFIG_SEARCH_LRU	0x08
/**
 * @def CONFIG_INSERT_SORTED
 *
 * @brief
 *   Configures the handler to perform sorted inserts in the list. The
 *   compare() function passed to the handler initializer is used to locate
 *   the insert position of the element being inserted.
 */
#define CONFIG_INSERT_SORTED	0x10
/**
 * @def CONFIG_INSERT_NEXT
 *
 * @brief
 *   Configures the handler to insert new elements on the next position after
 *   the current list pointer.
 *   The current list pointer is located at the position of the last successful
 *   insert or search, or at the next position of the last successful pop or
 *   delete operation.
 */
#define CONFIG_INSERT_NEXT	0x20
/**
 * @def CONFIG_INSERT_PREV
 *
 * @brief
 *   Configures the handler to insert new elements on the previous position
 *   after the current list pointer.
 *   The current list pointer is located at the position of the last successful
 *   insert or search, or at the next position of the last successful pop or
 *   delete operation.
 */
#define CONFIG_INSERT_PREV	0x40
/**
 * @def CONFIG_INSERT_TAIL
 *
 * @brief
 *   Configures the handler to insert new elements on the tail of the list.
 */
#define CONFIG_INSERT_TAIL	0x80
/**
 * @def CONFIG_INSERT_HEAD
 *
 * @brief
 *   Configures the handler to insert new elements at the head of the list.
 *   This is the default configuration.
 */
#define CONFIG_INSERT_HEAD	0x100

/* Structures */
struct cll_elem {
	void *data;
	struct cll_elem *next;
	struct cll_elem *prev;
};

/**
 * @struct cll_stat
 *
 * @brief
 *   Statistical counters for list operations and list elements.
 *
 * @see pall_cll_stat()
 * @see pall_cll_stat_reset()
 *
 * @var cll_stat::insert
 *   Number of successful inserts
 *
 * @var cll_stat::insert_err
 *   Number of failed inserts
 *
 * @var cll_stat::del
 *   Number of successful deletes
 *
 * @var cll_stat::del_nf
 *   Number of not found deletes
 *
 * @var cll_stat::search
 *   Number of successful searches
 *
 * @var cll_stat::search_nf
 *   Number of not found searches
 *
 * @var cll_stat::serialize
 *   Number of successful serializations
 *
 * @var cll_stat::serialize_err
 *   Number of failed serializations
 *
 * @var cll_stat::unserialize
 *   Number of successful unserializations
 *
 * @var cll_stat::unserialize_err
 *   Number of failed unserializations
 *
 * @var cll_stat::stat
 *   Number of stat calls
 *
 * @var cll_stat::count
 *   Number of count calls
 *
 * @var cll_stat::pope
 *   Number of successful element pops
 *
 * @var cll_stat::pope_nf
 *   Number of not found elem pops
 *
 * @var cll_stat::poph
 *   Number of successful head pops
 *
 * @var cll_stat::poph_nf
 *   Number of not found head pops
 *
 * @var cll_stat::collapse
 *   Number of collapse calls
 *
 * @var cll_stat::iterate
 *   Number of full iterations
 *
 * @var cll_stat::rewind
 *   Number of rewind calls
 *
 * @var cll_stat::set_config
 *   Number of set_config calls
 *
 * @var cll_stat::get_config
 *   Number of get_config calls
 *
 * @var cll_stat::elem_count_cur
 *   Current number of elements present on the list
 *
 * @var cll_stat::elem_count_max
 *   Maximum elements since initialization or last stat_reset call.
 */
struct cll_stat {
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
	unsigned long pope_nf;
	unsigned long poph;
	unsigned long poph_nf;
	unsigned long collapse;
	unsigned long iterate;
	unsigned long rewind;
	unsigned long set_config;
	unsigned long get_config;

	/* Element statistics */
	unsigned long elem_count_cur;
	unsigned long elem_count_max;
};

/**
 * @struct cll_handler
 *
 * @brief
 *   Data structure defining the Circular Linked List handler.
 *
 * @var cll_handler::cll
 *   Pointer to a Circular Linked List element that always refer to the current
 *   position on the list. This pointer is modified by pall_cll_insert(),
 *   pall_cll_delete(), pall_cll_pope(), pall_cll_poph() and pall_cll_search().
 *
 * @var cll_handler::cll_head
 *   Pointer to the element at the head of the Circular Linked List.
 *
 * @var cll_handler::insert
 *   Function pointer performing the same operation of pall_cll_insert()
 *
 * @var cll_handler::del
 *   Function pointer performing the same operation of pall_cll_delete()
 *
 * @var cll_handler::search
 *   Function pointer performing the same operation of pall_cll_search()
 *
 * @var cll_handler::serialize
 *   Function pointer performing the same operation of pall_cll_serialize()
 *
 * @var cll_handler::unserialize
 *   Function pointer performing the same operation of pall_cll_unserialize()
 *
 * @var cll_handler::stat
 *   Function pointer performing the same operation of pall_cll_stat()
 *
 * @var cll_handler::stat_reset
 *   Function pointer performing the same operation of pall_cll_stat_reset()
 *
 * @var cll_handler::count
 *   Function pointer performing the same operation of pall_cll_count()
 *
 * @var cll_handler::pope
 *   Function pointer performing the same operation of pall_cll_pope()
 *
 * @var cll_handler::poph
 *   Function pointer performing the same operation of pall_cll_poph()
 *
 * @var cll_handler::collapse
 *   Function pointer performing the same operation of pall_cll_collapse()
 *
 * @var cll_handler::iterate
 *   Function pointer performing the same operation of pall_cll_iterate()
 *
 * @var cll_handler::rewind
 *   Function pointer performing the same operation of pall_cll_rewind()
 *
 * @var cll_handler::set_config
 *   Function pointer performing the same operation of pall_cll_set_config()
 *
 * @var cll_handler::get_config
 *   Function pointer performing the same operation of pall_cll_get_config()
 *
 */
struct cll_handler {
	struct cll_elem *cll;
	struct cll_elem *cll_head;
	struct cll_elem *_iterate_start;
	struct cll_elem *_iterate_cur;
	int _iterate_reverse;

	struct cll_stat _stat;
	ui32_t _config_flags;
	ui32_t _count;

	int (*compare) (const void *d1, const void *d2);
	void (*destroy) (void *data);
	int (*ser_data) (pall_fd_t fd, void *data);
	void *(*unser_data) (pall_fd_t fd);

	int (*insert) (struct cll_handler *handler, void *data);
	int (*del) (struct cll_handler *handler, void *data);
	void *(*search) (struct cll_handler *handler, void *data);
	int (*serialize) (struct cll_handler *handler, pall_fd_t fd);
	int (*unserialize) (struct cll_handler *handler, pall_fd_t fd);
	struct cll_stat *(*stat) (struct cll_handler *handler);
	void (*stat_reset) (struct cll_handler *handler);
	ui32_t (*count) (struct cll_handler *handler);
	void *(*pope) (struct cll_handler *handler, void *data);
	void *(*poph) (struct cll_handler *handler);
	void (*collapse) (struct cll_handler *handler);
	void *(*iterate) (struct cll_handler *handler);
	void (*rewind) (struct cll_handler *handler, int to);
	ui32_t (*set_config) (struct cll_handler *handler, ui32_t flags);
	ui32_t (*get_config) (struct cll_handler *handler);
};


/* Prototypes / Interface */

/**
 * @brief
 *   Initializes a Circular Linked List handler.
 *
 * @param compare
 *   Internally used function for element comparision by pall_cll_insert(),
 *   pall_cll_search() and pall_cll_delete().
 *   It receives two elements as parameters of type const void *.
 *   It shall return an integer less than, equal to, or greater than zero if
 *   d1 is found, respectively, to be less than, to match, or to be greater
 *   than d2.
 *
 * @param destroy
 *   Internally used function for memory deallocation, on pall_cll_delete() and
 *   pall_cll_collapse(), of the element pointed by its parameter of type
 *   void *.
 *
 * @param ser_data
 *   Internally used function for element serialization.
 *   This is an optional argument and NULL shall be used to disable
 *   serialization support, causing serialization calls (pall_cll_serialize())
 *   to fail, setting errno to ENOSYS.
 *
 * @param unser_data
 *   Internally used function for element unserialization.
 *   This is an optional argument and NULL shall be used to disable
 *   unserialization support, causing unserialization calls
 *   (pall_cll_unserialize()) to fail, setting errno to ENOSYS.
 *
 * @return 
 *   On success, a pointer to a valid Circular Linked List handler is returned.
 *   On error, NULL is returned, and errno is set appropriately.
 *   \n\n
 *   Errors: EINVAL, ENOMEM
 *
 * @see pall_cll_insert()
 * @see pall_cll_search()
 * @see pall_cll_delete()
 * @see pall_cll_serialize()
 * @see pall_cll_unserialize()
 * @see pall_cll_destroy()
 *
 */
struct cll_handler *pall_cll_init(
		int (*compare) (const void *d1, const void *d2),
		void (*destroy) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd));

/**
 * @brief
 *   Unitializes and release all resources of a Circular Linked List handler
 *   pointed by parameter 'h'.
 *
 * @see pall_cll_init()
 *
 */
void pall_cll_destroy(struct cll_handler *h);

/**
 * @brief
 *   Inserts an element pointed by 'data' into the Circular Linked List pointed
 *   by 'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
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
 * @see pall_cll_init()
 * @see pall_cll_delete()
 * @see pall_cll_search()
 * @see cll_stat
 *
 */
int pall_cll_insert(struct cll_handler *h, void *data);

/**
 * @brief
 *   Deletes an element that matches the contents of the element pointed by
 *   'data' from the Circular Linked List pointed by 'h'. The comparision
 *   of the elements is performed by the compare() function passed to
 *   pall_cll_init() function.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched and deleted.
 *
 * @return
 *   On success, zero is returned and statistical counter 'del' is incremented.
 *   If the element is not found, -1 is returned and statistical counter
 *   'del_nf' is incremented.
 *
 * @see pall_cll_init()
 * @see pall_cll_insert()
 * @see cll_stat
 *
 */
int pall_cll_delete(struct cll_handler *h, void *data);

/**
 * @brief
 *   Searches an element that matches the contents of the element pointed by
 *   'data' on the Circular Linked List pointed by 'h'. The comparision
 *   of the elements is performed by the compare() function passed to
 *   pall_cll_init() function.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched.
 *
 * @return
 *   On success, a pointer to the found element is returned and statistical
 *   counter 'search' is incremented. If the element is not found, NULL is
 *   returned and statistical counter 'search_nf' is incremented.
 *
 * @see pall_cll_init()
 * @see cll_stat
 *
 */
void *pall_cll_search(struct cll_handler *h, void *data);

/**
 * @brief
 *   Serializes the contents of the Circular Linked List pointed by 'h', to
 *   the file descriptor 'fd'.
 *   The metadata of the list is serialized in network byte order.
 *   Each element is serialized through the ser_data() function passed to
 *   pall_cll_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Circular Linked List handler.
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
 * @see pall_cll_init()
 * @see pall_cll_unserialize()
 * @see cll_stat
 *
 */
int pall_cll_serialize(struct cll_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Unserializes the contents from the file descriptor 'fd' into the Circular
 *   Linked List pointed by 'h'.
 *   The unserialized data is converted from network byte order to host byte
 *   order.
 *   Each element is serialized through the unser_data() function passed to
 *   pall_cll_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Circular Linked List handler.
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
 * @see pall_cll_init()
 * @see pall_cll_serialize()
 * @see cll_stat
 *
 */
int pall_cll_unserialize(struct cll_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Returns statistical information for operations and content of the Circular
 *   Linked List pointed by handler 'h'. This function shall be called for
 *   each time updated statistical counters are required.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @return
 *   Returns a pointer to a valid struct cll_stat and the statistical counter
 *   'stat' is incremented. This function always succeeds.
 *
 * @see pall_cll_init()
 * @see pall_cll_stat_reset()
 * @see cll_stat
 *
 */
struct cll_stat *pall_cll_stat(struct cll_handler *h);

/**
 * @brief
 *   Resets the statistical counters of the Circular Linked List pointed by
 *   handler 'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @see pall_cll_init()
 * @see pall_cll_stat()
 * @see cll_stat
 *
 */
void pall_cll_stat_reset(struct cll_handler *h);

/**
 * @brief
 *   Returns the number of elements of the Circular Linked List pointed by
 *   handler 'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @return
 *   Returns a positive integer of type ui32_t (32 bit unsigned) if the list
 *   isn't empty. If it's empty, zero is returned. Statistical counter 'count'
 *   is always incremented when this function retruns.
 *
 * @see pall_cll_init()
 * @see cll_stat
 *
 */
ui32_t pall_cll_count(struct cll_handler *h);

/**
 * @brief
 *   Pops an element that matches the contents of the element pointed by 'data'
 *   from the Circular Linked List pointed by handler 'h'. The comparision of
 *   the elements is performed by the compare() function passed to
 *   pall_cll_init() function.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched and popped.
 *
 * @return
 *   On success, a pointer to the popped element is returned and the
 *   statistical counter 'pope' is incremented. If the element is not found,
 *   NULL is returned and the statistical counter 'pope_nf' is incremented.
 *
 * @see pall_cll_init()
 * @see cll_stat
 *
 */
void *pall_cll_pope(struct cll_handler *h, void *data);

/**
 * @brief
 *   Pops an element from the head of the Circular Linked List pointed by
 *   handler 'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @return
 *   On success, a pointer to the popped element is returned and the
 *   statistical counter 'poph' is incremented. If the list is empty, NULL is
 *   returned and the statistical counter 'poph_nf' is incremented.
 *
 * @see pall_cll_init()
 * @see cll_stat
 *
 */
void *pall_cll_poph(struct cll_handler *h);

/**
 * @brief
 *   Removes all the elements from the Circular Linked List pointed by handler
 *   'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @return
 *   This function doesn't return any value. The statistical counter 'collapse'
 *   is incremented on return.
 *
 * @see pall_cll_init()
 * @see pall_cll_delete()
 * @see cll_stat
 *
 */
void pall_cll_collapse(struct cll_handler *h);

/**
 * @brief
 *   Iterates through the elements of the Circular Linked List pointed by
 *   handler 'h'. Each call to this function returns a pointer to the next
 *   element present on the list. Iteration may be performed from head to tail
 *   or from tail to head, depending on the parameters used on the
 *   pall_cll_rewind() function.
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @return
 *   Returns a pointer to the next element present on the list. If the end of
 *   the list is reached, NULL is returned and statistical counter 'iterate'
 *   is incremented.
 *
 * @see pall_cll_init()
 * @see pall_cll_rewind()
 * @see cll_stat
 *
 */
void *pall_cll_iterate(struct cll_handler *h);

/**
 * @brief
 *   Rewinds the Circular Linked List pointed by handler 'h'. The parameter
 *   'to' tells to where the rewind should be perfomed and configures the
 *   the behavior of pall_cll_iterate().
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @param to
 *   If set to 0, the list is rewinded to head and pall_cll_iterate() will
 *   iterate the list from head to tail.
 *   If set to 1, the list is rewinded to tail and pall_cll_iterate() will
 *   iterate the list from tail to head.
 *
 * @return
 *   No value is returned and statistical counter 'rewind' is incremented for
 *   each time this function returns.
 *
 * @see pall_cll_init()
 * @see pall_cll_iterate()
 * @see cll_stat
 *
 */
void pall_cll_rewind(struct cll_handler *h, int to);

/**
 * @brief
 *   Sets a new configuration based on 'flags' parameter to the Circular Linked
 *   List pointed by handler 'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @param flags
 *   The configuration flags that will affect the behavior of the list.
 *   Available flags are: CONFIG_SEARCH_FORWARD, CONFIG_SEARCH_BACKWARD,
 *   CONFIG_SEARCH_AUTO, CONFIG_SEARCH_LRU (default), CONFIG_INSERT_SORTED,
 *   CONFIG_INSERT_NEXT, CONFIG_INSERT_PREV, CONFIG_INSERT_TAIL,
 *   CONFIG_INSERT_HEAD (default).
 *
 * @return
 *   Returns the previously configuration flags before the call to this
 *   function and statistical counter 'set_config' is incremented.
 *
 * @see pall_cll_init()
 * @see pall_cll_get_config()
 * @see cll_stat
 * @see CONFIG_SEARCH_FORWARD
 * @see CONFIG_SEARCH_BACKWARD
 * @see CONFIG_SEARCH_AUTO
 * @see CONFIG_SEARCH_LRU
 * @see CONFIG_INSERT_SORTED
 * @see CONFIG_INSERT_NEXT
 * @see CONFIG_INSERT_PREV
 * @see CONFIG_INSERT_TAIL
 * @see CONFIG_INSERT_HEAD
 *
 */
ui32_t pall_cll_set_config(struct cll_handler *h, ui32_t flags);

/**
 * @brief
 *   Gets the current configuration flags from the Circular Linked List
 *   pointed by handler 'h'.
 *
 * @param h
 *   An initialized Circular Linked List handler.
 *
 * @return
 *   Returns the current configuration flags set to the list and statistical
 *   counter 'set_config' is incremented.
 *
 * @see pall_cll_init()
 * @see pall_cll_set_config()
 * @see cll_stat
 * @see CONFIG_SEARCH_FORWARD
 * @see CONFIG_SEARCH_BACKWARD
 * @see CONFIG_SEARCH_AUTO
 * @see CONFIG_SEARCH_LRU
 * @see CONFIG_INSERT_SORTED
 * @see CONFIG_INSERT_NEXT
 * @see CONFIG_INSERT_PREV
 * @see CONFIG_INSERT_TAIL
 * @see CONFIG_INSERT_HEAD
 *
 */ 
ui32_t pall_cll_get_config(struct cll_handler *h);

#endif

