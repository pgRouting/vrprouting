/*PGR-GNU*****************************************************************
File: pickDeliver_driver.h

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

#ifndef INCLUDE_DRIVERS_PICKDELIVER_DRIVER_H_
#define INCLUDE_DRIVERS_PICKDELIVER_DRIVER_H_
#pragma once

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
using Solution_rt = struct Solution_rt;
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct Solution_rt Solution_rt;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /** @brief Driver for processing a pickupDeliver problem */
void vrp_do_pickDeliver(
        char*, char*, char*, char*,
        double, int, int64_t, bool, bool, bool, bool, bool,

        Solution_rt**, size_t*,
        char**, char**, char**);


#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_DRIVERS_PICKDELIVER_DRIVER_H_
