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
#include <cstring>
using TTimestamp = int64_t;
using TInterval = int64_t;
using Coordinate = double;
using Amount = int64_t;
using PAmount = uint32_t;
using Speed = double;
using Multiplier = double;
using Id = int64_t;
using Idx = uint64_t;
using MatrixIndex = int64_t;
using Duration = uint32_t;
using TravelCost = uint32_t;
using Priority = uint32_t;
using Distance = uint32_t;
using Skill = uint32_t;
using StepType = int32_t;
using PickDeliveryOrders_t = struct PickDeliveryOrders_t;
using Vehicle_t = struct Vehicle_t;
using Matrix_cell_t = struct Matrix_cell_t;
using Time_multipliers_t = struct Time_multipliers_t;
using Solution_rt = struct Solution_rt;
using Short_vehicle_rt = struct Short_vehicle_rt;
using Vroom_job_t = struct Vroom_job_t;
using Vroom_shipment_t = struct Vroom_shipment_t;
using Vroom_vehicle_t = struct Vroom_vehicle_t;
using Vroom_matrix_t = struct Vroom_matrix_t;
using Vroom_break_t = struct Vroom_break_t;
using Vroom_time_window_t = struct Vroom_time_window_t;
using Vroom_rt = struct Vroom_rt;
/*
 * Index on a container
 */
using Position = size_t;
#else
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef int64_t TTimestamp;
typedef int64_t TInterval;
typedef double Coordinate;
typedef int64_t Amount;
typedef uint32_t PAmount;
typedef double Speed;
typedef double Multiplier;
typedef int64_t Id;
typedef uint64_t Idx;
typedef int64_t MatrixIndex;
typedef uint32_t Duration;
typedef uint32_t TravelCost;
typedef uint32_t Priority;
typedef uint32_t Distance;
typedef uint32_t Skill;
typedef int32_t StepType;
typedef struct PickDeliveryOrders_t PickDeliveryOrders_t;
typedef struct Vehicle_t Vehicle_t;
typedef struct Matrix_cell_t Matrix_cell_t;
typedef struct Time_multipliers_t Time_multipliers_t;
typedef struct Solution_rt Solution_rt;
typedef struct Short_vehicle_rt Short_vehicle_rt;
typedef struct Vroom_job_t Vroom_job_t;
typedef struct Vroom_shipment_t Vroom_shipment_t;
typedef struct Vroom_vehicle_t Vroom_vehicle_t;
typedef struct Vroom_matrix_t Vroom_matrix_t;
typedef struct Vroom_break_t Vroom_break_t;
typedef struct Vroom_time_window_t Vroom_time_window_t;
typedef struct Vroom_rt Vroom_rt;
#endif

/*************************************************************************/

#endif  // INCLUDE_C_TYPES_TYPEDEFS_H_
