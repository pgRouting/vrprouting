/*PGR-GNU*****************************************************************
File: info.hpp

Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/
/*! @file */

#ifndef INCLUDE_CPP_COMMON_INFO_HPP_
#define INCLUDE_CPP_COMMON_INFO_HPP_
#pragma once

/* for int64_t */
#ifndef __cplusplus
#   include <stdbool.h>
#   include <stdint.h>
#endif

// used for getting the data
typedef
enum {
    INTEGER,
    ANY_INTEGER,
    ANY_NUMERICAL,
    TEXT,
    JSONB,
    CHAR1,
    INTEGER_ARRAY,
    ANY_INTEGER_ARRAY,
    TIMESTAMP,
    INTERVAL
} expectType;


typedef
struct {
    int colNumber;
    uint64_t type;
    bool strict;
    char *name;
    expectType eType;
} Column_info_t;


#endif  // INCLUDE_CPP_COMMON_INFO_HPP_
