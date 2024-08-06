/*PGR-GNU*****************************************************************
File: orders_t.hpp

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

#ifndef INCLUDE_CPP_COMMON_ORDERS_T_HPP_
#define INCLUDE_CPP_COMMON_ORDERS_T_HPP_

#include "c_types/typedefs.h"

/** @brief order's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | Order's identifier
demand | Number of demand
pick_node_id | Pickup node identifier
pick_open_t | Pickup open time
pick_close_t | Pickup close time
pick_service_t | Pickup service duration
deliver_node_id | Deliver node identifier
deliver_open_t | Deliver open time
deliver_close_t | Deliver close time
deliver_service_t | Deliver service duration
*/


/**************************************************************************
 * pickDelivery types
 * ***********************************************************************/
/*
 * its with either (foo_x, foo_y) pairs (for euclidean or with foo_node_id (for matrix)
 */
struct Orders_t{
  Id      id;     /** Order's identifier */
  PAmount  demand; /** Number of demand */

  Coordinate pick_x; /** Pick x coordinate: used in stand alone program for benchmarks */
  Coordinate pick_y; /** Pick y coordinate: used in stand alone program for benchmarks */
  Id  pick_node_id;  /** Pickup node identifier */

  TTimestamp pick_open_t;     /** Pickup open time*/
  TTimestamp pick_close_t;    /** Pickup close time*/
  TInterval  pick_service_t;  /** Pickup service duration */

  Coordinate deliver_x;  /** Deliver x coordinate: used in stand alone program for benchmarks */
  Coordinate deliver_y;  /** Deliver y coordinate: used in stand alone program for benchmarks */
  Id deliver_node_id;    /** Deliver node identifier */

  TTimestamp deliver_open_t;   /** Deliver open time */
  TTimestamp deliver_close_t;  /** Deliver close time */
  TInterval deliver_service_t; /** Deliver service duration */
};

#endif  // INCLUDE_CPP_COMMON_ORDERS_T_HPP_
