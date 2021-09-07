/*PGR-GNU*****************************************************************
File: compatibleVehicles_rt.h

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2021 Celia Virginia Vergara Castillo
Copyright (c) 2021 Joseph Emile Honour Percival

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

#ifndef INCLUDE_C_TYPES_COMPATIBLEVEHICLES_RT_H_
#define INCLUDE_C_TYPES_COMPATIBLEVEHICLES_RT_H_
#pragma once

/* for int64_t */
#ifdef __cplusplus
#   include <cstdint>
#else
#   include <stdint.h>
#endif

/** @brief order-vehicle compatability relationship

  @note C/C++/postgreSQL connecting structure for output
  name | description
  :----- | :-------
  order_id | Order's identifier
  vehicle_id | Vehicle's identifier
  */

struct CompatibleVehicles_rt {
  int64_t order_id;    /** order's identifier */
  int64_t vehicle_id;  /** vehicle's identifier */
};

#endif  // INCLUDE_C_TYPES_COMPATIBLEVEHICLES_RT_H_
