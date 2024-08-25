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

#include <cstdint>
#include <string>

namespace vrprouting {

enum expectType {
    INTEGER,
    ANY_INTEGER,
    ANY_NUMERICAL,
    TEXT,
    JSONB,
    CHAR1,
    INTEGER_ARRAY,
    ANY_INTEGER_ARRAY,
    TIMESTAMP,
    INTERVAL,

    POSITIVE_INTEGER,
    ANY_POSITIVE_INTEGER,
    ANY_UINT,
    ANY_POSITIVE_ARRAY,
    UINT_ARRAY,
    ANY_UINT_ARRAY,
    /* similar types */
    STEP_TYPE  = INTEGER,
    AMOUNT     = ANY_INTEGER,
    ID         = ANY_INTEGER,
    TTIMESTAMP = ANY_INTEGER,
    COORDINATE = ANY_NUMERICAL,
    SPEED      = ANY_NUMERICAL,
    MULTIPLIER = ANY_NUMERICAL,
    TINTERVAL    = ANY_POSITIVE_INTEGER,
    MATRIX_INDEX = ANY_POSITIVE_INTEGER,
    IDX     = ANY_UINT,
    PAMOUNT = ANY_UINT
};


class Info {
 public:
     int colNumber;
     uint64_t type;
     bool strict;
     std::string name;
     expectType eType;
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_INFO_HPP_
