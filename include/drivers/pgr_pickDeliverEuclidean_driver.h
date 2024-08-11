/*PGR-GNU*****************************************************************
File: pgr_pickDeliverEuclidean_driver.h

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

#ifndef INCLUDE_DRIVERS_PGR_PICKDELIVEREUCLIDEAN_DRIVER_H_
#define INCLUDE_DRIVERS_PGR_PICKDELIVEREUCLIDEAN_DRIVER_H_
#pragma once

/* for size-t */
#ifdef __cplusplus
#   include <cstddef>
#else
#   include <stddef.h>
#endif

typedef struct Orders_t Orders_t;
typedef struct Solution_rt Solution_rt;
typedef struct Vehicle_t Vehicle_t;

#ifdef __cplusplus
extern "C" {
#endif

void vrp_do_pgr_pickDeliverEuclidean(
        Orders_t *pd_orders_arr, size_t total_pd_orders,
        Vehicle_t *vehicles_arr, size_t total_vehicles,
        double, int, int,

        Solution_rt**, size_t*,
        char**, char**, char**);


#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_DRIVERS_PGR_PICKDELIVEREUCLIDEAN_DRIVER_H_
