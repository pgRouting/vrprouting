/*PGR-GNU*****************************************************************
File: time_multipliers_input.h

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

#ifndef INCLUDE_C_COMMON_TIME_MULTIPLIERS_INPUT_H_
#define INCLUDE_C_COMMON_TIME_MULTIPLIERS_INPUT_H_
#pragma once

#include <stddef.h>

typedef struct Time_multipliers_t Time_multipliers_t;

/** @brief Get the time multipliers using interval*/
void get_timeMultipliers(
    char *sql,
    Time_multipliers_t **row,
    size_t *total_rows);

/** @brief Get the time multipliers using bigint*/
void get_timeMultipliers_raw(
    char *sql,
    Time_multipliers_t **row,
    size_t *total_rows);

#endif  // INCLUDE_C_COMMON_TIME_MULTIPLIERS_INPUT_H_
