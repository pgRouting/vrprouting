/*PGR-GNU*****************************************************************
File: typedefs.h

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

#ifndef INCLUDE_C_TYPES_TYPEDEFS_H_
#define INCLUDE_C_TYPES_TYPEDEFS_H_
#pragma once

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
using TTimestamp = int64_t;
using TInterval = int64_t;
using Coordinate = double;
using Amount = int64_t;
using PAmount = uint32_t;
using Speed = double;
using Multiplier = double;
using Id = int64_t;
using Idx = uint64_t;
using PickDeliveryOrders_t = struct PickDeliveryOrders_t;
using Vehicle_t = struct Vehicle_t;
using Matrix_cell_t = struct Matrix_cell_t;
using Time_multipliers_t = struct Time_multipliers_t;
using Solution_rt = struct Solution_rt;
using Short_vehicle_rt = struct Short_vehicle_rt;
/*
 * Index on a container
 */
using Position = size_t;
#else
#include <stdint.h>
#include <stddef.h>
typedef int64_t TTimestamp;
typedef int64_t TInterval;
typedef double Coordinate;
typedef int64_t Amount;
typedef uint32_t PAmount;
typedef double Speed;
typedef double Multiplier;
typedef int64_t Id;
typedef struct PickDeliveryOrders_t PickDeliveryOrders_t;
typedef struct Vehicle_t Vehicle_t;
typedef struct Matrix_cell_t Matrix_cell_t;
typedef struct Time_multipliers_t Time_multipliers_t;
typedef struct Solution_rt Solution_rt;
typedef struct Short_vehicle_rt Short_vehicle_rt;
#endif


/*************************************************************************/

#endif  // INCLUDE_C_TYPES_TYPEDEFS_H_
