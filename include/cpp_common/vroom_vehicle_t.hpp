/*PGR-GNU*****************************************************************
File: vroom_vehicle_t.hpp

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

#ifndef INCLUDE_CPP_COMMON_VROOM_VEHICLE_T_HPP_
#define INCLUDE_CPP_COMMON_VROOM_VEHICLE_T_HPP_
#pragma once

#include <structures/vroom/vehicle.h>

#include <string>

#include "c_types/typedefs.h"

namespace vrprouting {

/** @brief Vroom's Vehicles's attributes

  @note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | The vehicle's identifier
start_id | Start location index in matrix
end_id | End location index in matrix
capacity | Vehicle's capacity array
skills | Vehicle's skills
tw | The time windows
speed_factor | Vehicle travel time multiplier
max_tasks | Max number of tasks in a route for the vehicle
data | Metadata information of vehicle
*/
class Vroom_vehicle_t {
 public:
     Idx id; /** The vehicle's identifier */
     MatrixIndex start_id; /** Start location index in matrix */
     MatrixIndex end_id; /** End location index in matrix */

     ::vroom::Amount capacity; /** Vehicle's capacity array */
     ::vroom::Skills skills; /** Mandatory skills */

     ::vroom::TimeWindow tw; /** The Vroom time windows */

     Multiplier speed_factor; /** Vehicle travel time multiplier */
     int32_t max_tasks; /** Max number of tasks in a route for the vehicle */
     std::string data; /** Metadata information of vehicle */
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VROOM_VEHICLE_T_HPP_
