/*PGR-GNU*****************************************************************
File: vroom_vehicle_t.h

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

#ifndef INCLUDE_C_TYPES_VROOM_VROOM_VEHICLE_T_H_
#define INCLUDE_C_TYPES_VROOM_VROOM_VEHICLE_T_H_
#pragma once

#include "c_types/typedefs.h"

/** @brief Vehicles's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | The vehicle's identifier
start_id | Start location index in matrix
end_id | End location index in matrix
capacity | Vehicle's capacity array
capacity_size | Vehicle's capacity array size
skills | Vehicle's skills
skills_size | Number of vehicle's skills
tw_open | Time window start time
tw_close | Time window end time
speed_factor | Vehicle travel time multiplier
max_tasks | Max number of tasks in a route for the vehicle
data | Metadata information of vehicle
*/
struct Vroom_vehicle_t {
  Idx id; /** The vehicle's identifier */
  MatrixIndex start_id; /** Start location index in matrix */
  MatrixIndex end_id; /** End location index in matrix */

  Amount *capacity; /** Vehicle's capacity array */
  size_t capacity_size; /** Vehicle's capacity array size */

  Skill *skills; /** Vehicle's skills */
  size_t skills_size; /** Number of vehicle's skills */

  Duration tw_open; /** Time window start time */
  Duration tw_close; /** Time window end time */

  double speed_factor; /** Vehicle travel time multiplier */

  int32_t max_tasks; /** Max number of tasks in a route for the vehicle */

  char *data; /** Metadata information of vehicle */
};


#endif  // INCLUDE_C_TYPES_VROOM_VROOM_VEHICLE_T_H_
