/*PGR-GNU*****************************************************************
File: vroom_shipment_t.h

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

#ifndef INCLUDE_C_TYPES_VROOM_VROOM_SHIPMENT_T_H_
#define INCLUDE_C_TYPES_VROOM_VROOM_SHIPMENT_T_H_
#pragma once

#include "c_types/typedefs.h"

/** @brief Vehicles's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | Shipment identifier
p_location_index | Pickup location index in matrix
p_service | Pickup service time
d_location_index | Delivery location index in matrix
d_service | Delivery service time
amount | Quantities for shipment
amount_size | Number of quantities
skills | Mandatory skills
skills_size | Number of skills
priority | Priority level of shipment
*/
struct Vroom_shipment_t {
  Idx id; /** Shipment identifier */

  /** pickup shipment */
  MatrixIndex p_location_index; /** Pickup location index in matrix */
  Duration p_service; /** Pickup service time */

  /** delivery shipment */
  MatrixIndex d_location_index; /** Delivery location index in matrix */
  Duration d_service; /** Delivery service time */

  Amount *amount; /** Quantities for shipment */
  size_t amount_size; /** Number of quantities */

  Skill *skills; /** Mandatory skills */
  size_t skills_size; /** Number of skills */

  Priority priority; /** Priority level of shipment */
};

#endif  // INCLUDE_C_TYPES_VROOM_VROOM_SHIPMENT_T_H_
