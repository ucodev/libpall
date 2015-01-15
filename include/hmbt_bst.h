/**
 * @file hmbt_bst.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Hash Mod Balanced Tree BST interface header
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

#ifndef LIBPALL_HMBT_BST_H
#define LIBPALL_HMBT_BST_H

#include "pall.h"
#include "bst.h"

/* Constants */
#define HMBT_BST_DEFAULT_ARR_SIZE	127

/* Structures */

/**
 * @struct hmbt_bst_stat
 *
 * @brief
 *   Statistical counters for tree operations and tree elements.
 *
 * @see pall_hmbt_bst_stat()
 * @see pall_hmbt_bst_stat_reset()
 *
 * @var hmbt_bst_stat::insert
 *   Number of successful inserts
 *
 * @var hmbt_bst_stat::insert_err
 *   Number of failed inserts
 *
 * @var hmbt_bst_stat::del
 *   Number of successful deletes
 *
 * @var hmbt_bst_stat::del_nf
 *   Number of not found deletes
 *
 * @var hmbt_bst_stat::search
 *   Number of successful searches
 *
 * @var hmbt_bst_stat::search_nf
 *   Number of not found searches
 *
 * @var hmbt_bst_stat::serialize
 *   Number of successful serializations
 *
 * @var hmbt_bst_stat::serialize_err
 *   Number of failed serializations
 *
 * @var hmbt_bst_stat::unserialize
 *   Number of successful unserializations
 *
 * @var hmbt_bst_stat::unserialize_err
 *   Number of failed unserializations
 *
 * @var hmbt_bst_stat::stat
 *   Number of stat calls
 *
 * @var hmbt_bst_stat::count
 *   Number of count calls
 *
 * @var hmbt_bst_stat::collapse
 *   Number of collapse calls
 *
 * @var hmbt_bst_stat::iterate
 *   Number of full iterations
 *
 * @var hmbt_bst_stat::rewind
 *   Number of rewind calls
 *
 * @var hmbt_bst_stat::elem_count_cur
 *   Current number of elements present on the tree
 *
 * @var hmbt_bst_stat::elem_count_max
 *   Maximum elements since initialization or last stat_reset call.
 *
 * @var hmbt_bst_stat::node_elem_count_min
 *   The element count of the node with less elements.
 *
 * @var hmbt_bst_stat::node_elem_count_avg
 *   The average number of elements per node.
 *
 * @var hmbt_bst_stat::node_elem_count_max
 *   The element count of the node with more elements.
 *
 * @var hmbt_bst_stat::node_elem_count
 *   Array containing the element count of all nodes.
 *   The array index matches the indexes of the HMBT-BST array.
 *
 */
struct hmbt_bst_stat {
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
	unsigned long collapse;
	unsigned long iterate;
	unsigned long rewind;

	/* Element statistics */
	unsigned long elem_count_cur;
	unsigned long elem_count_max;

	/* Node statistics */
	unsigned long node_elem_count_min;
	float node_elem_count_avg;
	unsigned long node_elem_count_max;
	unsigned long *node_elem_count;
};

/**
 * @struct hmbt_bst_handler
 *
 * @brief
 *   Data structure defining the Hash Mod Balanced Tree BST handler.
 *
 * @var hmbt_bst_handler::array
 *   The HMBT-BST array of size 'arr_size'. Each array element points to a
 *   Binary Search Tree handler. This array shall not be directly modified.
 *
 * @var hmbt_bst_handler::arr_size
 *   The size of the HMBT-BST array. This value shall not be directly modified.
 *
 * @var hmbt_bst_handler::insert
 *   Function pointer performing the same operation of pall_hmbt_bst_insert()
 *
 * @var hmbt_bst_handler::del
 *   Function pointer performing the same operation of pall_hmbt_bst_delete()
 *
 * @var hmbt_bst_handler::search
 *   Function pointer performing the same operation of pall_hmbt_bst_search()
 *
 * @var hmbt_bst_handler::serialize
 *   Function pointer performing the same operation of pall_hmbt_bst_serialize()
 *
 * @var hmbt_bst_handler::unserialize
 *   Function pointer performing the same operation of
 *   pall_hmbt_bst_unserialize()
 *
 * @var hmbt_bst_handler::stat
 *   Function pointer performing the same operation of pall_hmbt_bst_stat()
 *
 * @var hmbt_bst_handler::stat_reset
 *   Function pointer performing the same operation of
 *   pall_hmbt_bst_stat_reset()
 *
 * @var hmbt_bst_handler::count
 *   Function pointer performing the same operation of pall_hmbt_bst_count()
 *
 * @var hmbt_bst_handler::pope
 *   Function pointer performing the same operation of pall_hmbt_bst_pope()
 *
 * @var hmbt_bst_handler::poph_elem
 *   Function pointer performing the same operation of pall_hmbt_bst_poph_elem()
 *
 * @var hmbt_bst_handler::poph_index
 *   Function pointer performing the same operation of
 *   pall_hmbt_bst_poph_index()
 * 
 * @var hmbt_bst_handler::collapse
 *   Function pointer performing the same operation of pall_hmbt_bst_collapse()
 *
 * @var hmbt_bst_handler::iterate
 *   Function pointer performing the same operation of pall_hmbt_bst_iterate()
 *
 * @var hmbt_bst_handler::rewind
 *   Function pointer performing the same operation of pall_hmbt_bst_rewind()
 *
 * @var hmbt_bst_handler::set_config
 *   Function pointer performing the same operation of
 *   pall_hmbt_bst_set_config()
 *
 * @var hmbt_bst_handler::get_config
 *   Function pointer performing the same operation of
 *   pall_hmbt_bst_get_config()
 *
 */
struct hmbt_bst_handler {
	struct bst_handler **array;
	unsigned int arr_size;
	ui32_t _iterate_arr_pos;
	int _iterate_reverse;

	struct hmbt_bst_stat _stat;
	int (*compare) (const void *d1, const void *d2);
	ui32_t (*hash) (void *data);
	void (*destroy) (void *data);
	int (*ser_data) (pall_fd_t fd, void *data);
	void *(*unser_data) (pall_fd_t fd);

	int (*insert) (struct hmbt_bst_handler *handler, void *data);
	int (*del) (struct hmbt_bst_handler *handler, void *data);
	void *(*search) (struct hmbt_bst_handler *handler, void *data);
	int (*serialize) (struct hmbt_bst_handler *handler, pall_fd_t fd);
	int (*unserialize) (struct hmbt_bst_handler *handler, pall_fd_t fd);
	struct hmbt_bst_stat *(*stat) (struct hmbt_bst_handler *handler);
	void (*stat_reset) (struct hmbt_bst_handler *handler);
	ui32_t (*count) (struct hmbt_bst_handler *handler);
	void (*collapse) (struct hmbt_bst_handler *handler);
	void *(*iterate) (struct hmbt_bst_handler *handler);
	void (*rewind) (struct hmbt_bst_handler *handler, int to);
};


/* Prototypes / Interface */

/**
 * @brief
 *   Initializes a Hash Mod Balanced Tree BST handler.
 *
 * @param compare
 *   Internally used function for element comparision by pall_hmbt_bst_insert(),
 *   pall_hmbt_bst_search() and pall_hmbt_bst_delete().
 *   It receives two elements as parameters of type const void *.
 *   It shall return an integer less than, equal to, or greater than zero if
 *   d1 is found, respectively, to be less than, to match, or to be greater
 *   than d2.
 *
 * @param destroy
 *   Internally used function for memory deallocation, on pall_hmbt_bst_delete()
 *   and pall_hmbt_bst_collapse(), of the element pointed by its parameter of
 *   type void *.
 *
 * @param hash
 *   Internally used function for element hashing. This function shall receive
 *   a pointer to the element to be hashed and returns a 32-bit integer. This
 *   function is invoked each time a call to a hmbt-bst function that requires
 *   to determine the location of an element on the tree. The analogous
 *   operation performed to determine the array index of the HMBT-BST for a
 *   given element is:
 *
 *     index = hash(element) % arr_size
 *
 * @param ser_data
 *   Internally used function for element serialization.
 *   This is an optional argument and NULL shall be used to disable
 *   serialization support, causing serialization calls
 *   (pall_hmbt_bst_serialize()) to fail, setting errno to ENOSYS.
 *
 * @param unser_data
 *   Internally used function for element unserialization.
 *   This is an optional argument and NULL shall be used to disable
 *   unserialization support, causing unserialization calls
 *   (pall_hmbt_bst_unserialize()) to fail, setting errno to ENOSYS.
 *
 * @param array_size
 *   The size of the array of the HMBT-BST. Each array element is a pointer to a
 *   Binary Search Tree handler. If this parameter is set to zero, the default
 *   value defined by HMBT_BST_DEFAULT_ARR_SIZE is used.
 *
 * @return 
 *   On success, a pointer to a valid Hash Mod Balanced Tree BST handler is
 *   returned.
 *   On error, NULL is returned, and errno is set appropriately.
 *   \n\n
 *   Errors: EINVAL, ENOMEM
 *
 * @see pall_hmbt_bst_insert()
 * @see pall_hmbt_bst_search()
 * @see pall_hmbt_bst_delete()
 * @see pall_hmbt_bst_serialize()
 * @see pall_hmbt_bst_unserialize()
 * @see pall_hmbt_bst_destroy()
 *
 */
struct hmbt_bst_handler *pall_hmbt_bst_init(
		int (*compare) (const void *d1, const void *d2),
		void (*destroy) (void *data),
		ui32_t (*hash) (void *data),
		int (*ser_data) (pall_fd_t fd, void *data),
		void *(*unser_data) (pall_fd_t fd),
		unsigned int array_size);

/**
 * @brief
 *   Unitializes and release all resources of a Hash Mod Balanced Tree BST handler
 *   pointed by parameter 'h'.
 *
 * @see pall_hmbt_bst_init()
 *
 */
void pall_hmbt_bst_destroy(struct hmbt_bst_handler *h);

/**
 * @brief
 *   Inserts an element pointed by 'data' into the Hash Mod Balanced Tree BST
 *   pointed by 'h'.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
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
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_delete()
 * @see pall_hmbt_bst_search()
 * @see hmbt_bst_stat
 *
 */
int pall_hmbt_bst_insert(struct hmbt_bst_handler *h, void *data);

/**
 * @brief
 *   Deletes an element that matches the contents of the element pointed by
 *   'data' from the Hash Mod Balanced Tree BST pointed by 'h'. The comparision
 *   of the elements is performed by the compare() function passed to
 *   pall_hmbt_bst_init() function.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched and deleted.
 *
 * @return
 *   On success, zero is returned and statistical counter 'del' is incremented.
 *   If the element is not found, -1 is returned and statistical counter
 *   'del_nf' is incremented.
 *
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_insert()
 * @see hmbt_bst_stat
 *
 */
int pall_hmbt_bst_delete(struct hmbt_bst_handler *h, void *data);

/**
 * @brief
 *   Searches an element that matches the contents of the element pointed by
 *   'data' on the Hash Mod Balanced Tree BST pointed by 'h'. The comparision
 *   of the elements is performed by the compare() function passed to
 *   pall_hmbt_bst_init() function.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @param data
 *   A pointer to the partially filled element to be searched.
 *
 * @return
 *   On success, a pointer to the found element is returned and statistical
 *   counter 'search' is incremented. If the element is not found, NULL is
 *   returned and statistical counter 'search_nf' is incremented.
 *
 * @see pall_hmbt_bst_init()
 * @see hmbt_bst_stat
 *
 */
void *pall_hmbt_bst_search(struct hmbt_bst_handler *h, void *data);

/**
 * @brief
 *   Serializes the contents of the Hash Mod Balanced Tree BST pointed by 'h',
 *   to the file descriptor 'fd'.
 *   The metadata of the tree is serialized in network byte order.
 *   Each element is serialized through the ser_data() function passed to
 *   pall_hmbt_bst_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
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
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_unserialize()
 * @see hmbt_bst_stat
 *
 */
int pall_hmbt_bst_serialize(struct hmbt_bst_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Unserializes the contents from the file descriptor 'fd' into the Hash Mod
 *   Balanced Tree BST pointed by 'h'.
 *   The unserialized data is converted from network byte order to host byte
 *   order.
 *   Each element is serialized through the unser_data() function passed to
 *   pall_hmbt_bst_init(). If this parameter was passed as NULL, this function
 *   will return error with errno set to ENOSYS.
 *   The pall_fd_t type is a compatible type to the file descriptor type of the
 *   system.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
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
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_serialize()
 * @see hmbt_bst_stat
 *
 */
int pall_hmbt_bst_unserialize(struct hmbt_bst_handler *h, pall_fd_t fd);

/**
 * @brief
 *   Returns statistical information for operations and content of the Hash Mod
 *   Balanced Tree BST pointed by handler 'h'. This function shall be called
 *   for each time updated statistical counters are required.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @return
 *   Returns a pointer to a valid struct hmbt_bst_stat and the statistical
 *   counter 'stat' is incremented. This function always succeeds.
 *
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_stat_reset()
 * @see hmbt_bst_stat
 *
 */
struct hmbt_bst_stat *pall_hmbt_bst_stat(struct hmbt_bst_handler *h);

/**
 * @brief
 *   Resets the statistical counters of the Hash Mod Balanced Tree BST pointed
 *   by handler 'h'.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_stat()
 * @see hmbt_bst_stat
 *
 */
void pall_hmbt_bst_stat_reset(struct hmbt_bst_handler *h);

/**
 * @brief
 *   Returns the number of elements of the Hash Mod Balanced Tree BST pointed
 *   by handler 'h'.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @return
 *   Returns a positive integer of type ui32_t (32 bit unsigned) if the tree
 *   isn't empty. If it's empty, zero is returned. Statistical counter 'count'
 *   is always incremented when this function retruns.
 *
 * @see pall_hmbt_bst_init()
 * @see hmbt_bst_stat
 *
 */
ui32_t pall_hmbt_bst_count(struct hmbt_bst_handler *h);

/**
 * @brief
 *   Removes all the elements from the Hash Mod Balanced Tree BST pointed by
 *   handler 'h'.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @return
 *   This function doesn't return any value. The statistical counter 'collapse'
 *   is incremented on return.
 *
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_delete()
 * @see hmbt_bst_stat
 *
 */
void pall_hmbt_bst_collapse(struct hmbt_bst_handler *h);

/**
 * @brief
 *   Iterates through the elements of the Hash Mod Balanced Tree BST pointed by
 *   handler 'h'. Each call to this function returns a pointer to the next
 *   element present on the tree. Iteration may be performed from array index
 *   zero to arr_size, depth-first, left-to-right for each indexed BST, or from
 *   arr_size to zero, depth-first, right-to-left for each indexed BST,
 *   depending on the parameters used on the pall_hmbt_bst_rewind() function.
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @return
 *   Returns a pointer to the next element present on the HMBT-BST. If the end
 *   of the HMBT-BST is reached, NULL is returned and statistical counter
 *   'iterate' is incremented.
 *
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_rewind()
 * @see hmbt_bst_stat
 *
 */
void *pall_hmbt_bst_iterate(struct hmbt_bst_handler *h);

/**
 * @brief
 *   Rewinds the Hash Mod Balanced Tree BST pointed by handler 'h'. The
 *   parameter 'to' tells to where the rewind should be perfomed and configures
 *   the behavior of pall_hmbt_bst_iterate().
 *   Rewind and iterate do not affect the behavior of any other operation.
 *
 * @param h
 *   An initialized Hash Mod Balanced Tree BST handler.
 *
 * @param to
 *   If set to 0, the tree is rewinded to array index zero, and each BST is
 *   rewinded to root. pall_hmbt_bst_iterate() will iterate the tree from array
 *   index zero to arr_size and each indexed BST from depth-first,
 *   left-to-right.
 *   If set to 1, the tree is rewinded to arr_size index, and each BST is
 *   rewinded to root. pall_hmbt_bst_iterate() will iterate the tree from array
 *   index arr_size to zero and each indexed BST from depth-first,
 *   right-to-left.
 *
 * @return
 *   No value is returned and statistical counter 'rewind' is incremented for
 *   each time this function returns.
 *
 * @see pall_hmbt_bst_init()
 * @see pall_hmbt_bst_iterate()
 * @see hmbt_bst_stat
 *
 */
void pall_hmbt_bst_rewind(struct hmbt_bst_handler *h, int to);

#endif

