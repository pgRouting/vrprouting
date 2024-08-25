/*PGR-GNU*****************************************************************
File: time_multipliers_t.h

Copyright (c) 2020 Celia Virginia Vergara Castillo
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

#ifndef INCLUDE_CPP_COMMON_TIME_MULTIPLIERS_T_HPP_
#define INCLUDE_CPP_COMMON_TIME_MULTIPLIERS_T_HPP_
#pragma once


#include "c_types/typedefs.h"

namespace vrprouting {

/** @brief Time Dependant Multipliers

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
start_time | with respect to 0
multiplier | takes effect starting from the @b start_time

@note
- Evrything between 0 and the smallest @b start_time found will have 1 as multiplier
- Evrything between after the largest start time will have the multiplier of the largest start_time

*/
class Time_multipliers_t {
 public:
     /** Time of day where the multiplier starts to be valid */
     TTimestamp start_time;
     /** multiplier at hour */
     Multiplier multiplier;
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_TIME_MULTIPLIERS_T_HPP_
