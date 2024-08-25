/*PGR-GNU*****************************************************************
File: vroom_matrix_t.hpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2021 Ashish Kumar
Mail: ashishkr23438@gmail.com

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

#ifndef INCLUDE_CPP_COMMON_VROOM_MATRIX_T_HPP_
#define INCLUDE_CPP_COMMON_VROOM_MATRIX_T_HPP_
#pragma once

#include "c_types/typedefs.h"

namespace vrprouting {

/** @brief Matrix's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
start_id | Start node identifier
end_id | End node identifier
duration | Duration to travel from start to end
cost | Cost to travel from start to end
*/
class Vroom_matrix_t {
 public:
     MatrixIndex start_id;  /** Start node identifier */
     MatrixIndex end_id;    /** End node identifier */

     Duration duration;     /** Duration to travel from start to end */
     TravelCost cost;       /** Cost to travel from start to end */
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VROOM_MATRIX_T_HPP_

