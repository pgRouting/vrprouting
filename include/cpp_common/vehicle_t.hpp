/*PGR-GNU*****************************************************************
File: vehicle_t.hpp

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
/** @file */

#ifndef INCLUDE_CPP_COMMON_VEHICLE_T_HPP_
#define INCLUDE_CPP_COMMON_VEHICLE_T_HPP_
#pragma once

#include <vector>
#include "c_types/typedefs.h"

namespace vrprouting {

/** @brief vehicles's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | Vehicle's identifier
capacity | Vehicle's capacity
start_node_id | Pickup open time
start_open_t | Start open time
start_close_t | Start close time
start_service_t | Start service duration
end_node_id | End node's identifier
end_open_t | End open time
end_close_t | End close time
end_service_t | End service time
stops | Vehicle's stops
*/
class Vehicle_t {
 public:
     Id id; /** Vehicle's identifier */
     PAmount capacity; /** Vehicle's capacity */
     Speed speed;
     PAmount cant_v;   /** Number of vehicles with same description **/
     std::vector<Id> stops; /** Stops */

     Id start_node_id; /** Start node's identifier */
     TTimestamp start_open_t; /** Start open time */
     TTimestamp start_close_t; /** Start close time */
     TInterval  start_service_t; /** Start service duration */
     Coordinate start_x;
     Coordinate start_y;

     Id end_node_id; /** End node's identifier */
     TTimestamp end_open_t; /** End open time */
     TTimestamp end_close_t; /** End close time */
     TInterval  end_service_t; /** End service time */
     Coordinate end_x;
     Coordinate end_y;
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VEHICLE_T_HPP_
