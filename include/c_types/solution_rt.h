/*PGR-GNU*****************************************************************
File: solution_rt.h

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

#ifndef INCLUDE_C_TYPES_SOLUTION_RT_H_
#define INCLUDE_C_TYPES_SOLUTION_RT_H_
#pragma once

/* for int64_t */
#ifdef __cplusplus
#   include <cstdint>
#else
#   include <stdint.h>
#endif

/** @brief Solution schedule when twv & cw are hard restrictions

@note C/C++/postgreSQL connecting structure for output
name | description
:----- | :-------
vehicle_seq | Sequence for ordering for a single vehicle
vehicle_id | Vehicle's identifier
stop_seq | Stop sequence of vehicle
order_id | Node identifier of the stop
stop_type | Kind of stop
cargo | Cargo when leaving the stop
travelDuration | Travel time from previous stop
arrivalTime | Vehicle's arrival time
waitDuration | Vehicle's wait time before stop opens
operationTime | Time at which picked up or droped off takes place
serviceDuration | Service time of the stop
departureTime | Vehicle's departure time from the stop
*/

struct Solution_rt {
    int vehicle_seq;         /** Sequence for ordering for a single vehicle */
    int64_t vehicle_id;      /** Vehicle's identifier */
    int stop_seq;            /** Stop sequence of vehicle */
    int64_t order_id;        /** Served order's identifier */
    int64_t stop_id;         /** Node identifier of the stop */
    int stop_type;           /** Kind of stop */
    int64_t cargo;            /** Cargo when leaving the stop */
    int64_t travelTime;       /** Travel time from previous stop */
    int64_t arrivalTime;      /** Vehicle's arrival time */
    int64_t waitDuration;     /** Vehicle's wait time before stop opens */
    int64_t operationTime;    /** Time at which the pickup or delivery takes place */
    int64_t serviceDuration;  /** Service time of the stop */
    int64_t departureTime;    /** Vehicle's departure time from the stop */
    int cvTot;               /** Total Capacity Violations */
    int twvTot;              /** Total Time Window Violations */
};

/*************************************************************************/

#endif  // INCLUDE_C_TYPES_SOLUTION_RT_H_
