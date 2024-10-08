/*PGR-GNU*****************************************************************
File: matrix_cell_t.hpp

Copyright (c) 2017 Celia Virginia Vergara Castillo
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

#ifndef INCLUDE_CPP_COMMON_MATRIX_CELL_T_HPP_
#define INCLUDE_CPP_COMMON_MATRIX_CELL_T_HPP_
#pragma once

#include "c_types/typedefs.h"

namespace vrprouting {

/** @brief traveling costs

  @note C/C++/postgreSQL connecting structure for input
  name | description
  :----- | :-------
  from_vid | Departure node's identifier
  to_vid | Arrival node's identifier
  cost | Travel cost from departure to arrival
  */
class Matrix_cell_t {
 public:
     Id from_vid; /** @b departure node's identifier */
     Id to_vid;   /** @b arrival node's identifier */
     TInterval cost;      /** Travel Interval from departure to arrival */
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_MATRIX_CELL_T_HPP_
