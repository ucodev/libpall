/**
 * @file config.h
 * @brief Portable Abstracted Linked Lists Library (libpall)
 *        Configuration header
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

#ifndef LIBPALL_CONFIG_H
#define LIBPALL_CONFIG_H

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
 #ifndef COMPILE_WIN32
  #define COMPILE_WIN32 1
 #endif
#endif

#ifdef COMPILE_WIN32
 #include <winsock2.h>
 #include <windows.h>

 #if BUILDING_DLL
  #define DLLIMPORT __declspec(dllexport)
 #else
  #define DLLIMPORT __declspec(dllimport)
 #endif
#endif

#endif

