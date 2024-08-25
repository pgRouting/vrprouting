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
#endif

/*************************************************************************/

#endif  // INCLUDE_C_TYPES_TYPEDEFS_H_
