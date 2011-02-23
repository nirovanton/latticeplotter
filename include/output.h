/*
    Definitions for output macros to make life easier.
    Copyright (C) 2009  Alex Brandt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef OUTPUT_H
#define OUTPUT_H

#include <iostream> //!< Required for the following operations so we might as well include it.

#define GRAY   "\033[22;37m"
#define LIGHT_RED "\033[01;31m"
#define YELLOW "\033[01;33m"
#define LIGHT_BLUE      "\033[01;34m"
#define LIGHT_GREEN "\033[01;32m"

#ifndef NDEBUG
#define DEBUG(A) if (this->debug) \
    std::cerr << YELLOW << __FILE__ << ":" << __LINE__ << ": debug: " << #A << " -> " << (A) << GRAY << std::endl
#endif

#define VERBOSE(A) if (this->verbose) \
    std::cout << LIGHT_BLUE << __FILE__ << ":" << __LINE__ << ": verbose: " << (A) << GRAY << std::endl

#define WARNING(A) std::cerr << LIGHT_GREEN << (A) << GRAY << std::endl
#define ERROR(A) std::cerr << LIGHT_RED << (A) << GRAY << std::endl

#endif
