/*PGR-GNU*****************************************************************
File: optimize_driver.h

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail:

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

/*! @file pickDeliver_driver.h */

#ifndef INCLUDE_DRIVERS_OPTIMIZE_DRIVER_H_
#define INCLUDE_DRIVERS_OPTIMIZE_DRIVER_H_
#pragma once

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
using Orders_t = struct Orders_t;
using Vehicle_t = struct Vehicle_t;
using Matrix_cell_t = struct Matrix_cell_t;
using Time_multipliers_t = struct Time_multipliers_t;
using Short_vehicle_rt = struct Short_vehicle_rt;
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct Orders_t Orders_t;
typedef struct Vehicle_t Vehicle_t;
typedef struct Matrix_cell_t Matrix_cell_t;
typedef struct Time_multipliers_t Time_multipliers_t;
typedef struct Short_vehicle_rt Short_vehicle_rt;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void vrp_do_optimize(
        Orders_t customers_arr[], size_t,
        Vehicle_t *vehicles_arr, size_t,
        Matrix_cell_t *, size_t,
        Time_multipliers_t *, size_t,
        double, int, int64_t, bool, bool, bool,

        Short_vehicle_rt**, size_t*,
        char**, char**, char**);


#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_DRIVERS_OPTIMIZE_DRIVER_H_
