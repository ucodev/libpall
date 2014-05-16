/**
 * @file pall.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Configuration header for libpall
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

#ifndef LIBPALL_H
#define LIBPALL_H

/**
 * @typedef pall_fd_t
 *
 * @brief
 *   Compatible type for system file descriptor type.
 *
 */

/**
 * @typedef ui32_t
 *
 * @brief
 *   32-bit unsigned integer. The reason for this typedef is that uint32_t is
 *   an optional type of C99 and C11 standards and may not be available on
 *   all compilers.
 *
 */

 #if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
  #include <windows.h>
  #include <winsock2.h>

  typedef HANDLE pall_fd_t;
  typedef unsigned __int32 ui32_t;

  static inline SSIZE_T pall_write(HANDLE h, LPCVOID data, DWORD count) {
	DWORD wcount = 0;

	if (!WriteFile(h, data, count, &wcount, NULL))
		return -1;

	return (SSIZE_T) wcount;
  }

  static inline SSIZE_T pall_read(HANDLE h, LPVOID data, DWORD count) {
	DWORD rcount = 0;

	if (!ReadFile(h, data, count, &rcount, NULL))
		return -1;

	return (SSIZE_T) rcount;
  }

 #else /* !WIN32/64 */
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <stdint.h>

  typedef int pall_fd_t;
  typedef uint32_t ui32_t;

  #define pall_write(fd, data, count) write(fd, data, count)
  #define pall_read(fd, data, count) read(fd, data, count)

 #endif

#define pall_htonl(hl) htonl(hl)

#endif

