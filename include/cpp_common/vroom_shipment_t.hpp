/*PGR-GNU*****************************************************************
File: vroom_shipment_t.hpp

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

#ifndef INCLUDE_CPP_COMMON_VROOM_SHIPMENT_T_HPP_
#define INCLUDE_CPP_COMMON_VROOM_SHIPMENT_T_HPP_
#pragma once

#include <structures/vroom/input/input.h>

#include <string>

#include "c_types/typedefs.h"

namespace vrprouting {


/** @brief Vroom's shipment's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | Shipment identifier
p_location_id | Pickup location index in matrix
p_setup | Pickup setup time
p_service | Pickup service time
d_location_id | Delivery location index in matrix
d_setup | Delivery setup time
d_service | Delivery service time
amount | Quantities for shipment
skills | Mandatory skills
priority | Priority level of shipment
p_data | Metadata information of pickup shipment
d_data | Metadata information of delivery shipment
*/
class Vroom_shipment_t {
 public:
     Idx id; /** Shipment identifier */

     /** pickup shipment */
     MatrixIndex p_location_id; /** Pickup location index in matrix */
     Duration p_setup; /** Pickup setup time */
     Duration p_service; /** Pickup service time */

     /** delivery shipment */
     MatrixIndex d_location_id; /** Delivery location index in matrix */
     Duration d_setup; /** Delivery setup time */
     Duration d_service; /** Delivery service time */

     ::vroom::Amount amount; /** Quantities for shipment */
     ::vroom::Skills skills; /** Mandatory skills */
     Priority priority; /** Priority level of shipment */

     std::string p_data; /** Metadata information of pickup shipment */
     std::string d_data; /** Metadata information of delivery shipment */
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VROOM_SHIPMENT_T_HPP_
