/*PGR-GNU*****************************************************************
File: vroom_rt.h

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2021 Ashish Kumar
Mail: ashishkr23438@gmail.com

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

#ifndef INCLUDE_C_TYPES_VROOM_RT_H_
#define INCLUDE_C_TYPES_VROOM_RT_H_
#pragma once

#include "c_types/typedefs.h"

/** @brief Solution's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
vehicle_seq | Sequence for ordering a single vehicle
vehicle_id | The vehicle's identifier
vehicle_data | The vehicle's metadata information
step_seq | Step sequence of the vehicle
step_type | Step sequence of the vehicle
task_id | The task's identifier
location_id | Location id of task in matrix
task_data | The task's metadata information
arrival_time | Estimated time of arrival
travel_time | Travel time from previous step_seq to current step_seq
setup_time | Setup time at this step
service_time | Service time at this step
waiting_time | Waiting time upon arrival at this step
departure_time | Estimated time of departure
load | Vehicle load after step completion
*/
struct Vroom_rt {
  Idx vehicle_seq;         /** Sequence for ordering a single vehicle */
  Idx vehicle_id;          /** The vehicle's identifier */
  char *vehicle_data;      /** The vehicle's metadata information */

  Idx step_seq;            /** Step sequence of the vehicle */
  StepType step_type;      /** Type of the step */
  Idx task_id;             /** The task's identifier */
  MatrixIndex location_id; /** Location id of task in matrix */
  char *task_data;         /** The task's metadata information */

  Duration arrival_time;   /** Estimated time of arrival */
  Duration travel_time;    /** Travel time from previous step_seq to current step_seq */
  Duration setup_time;     /** Setup time at this step */
  Duration service_time;   /** Service time at this step */
  Duration waiting_time;   /** Waiting time upon arrival at this step */
  Duration departure_time; /** Estimated time of departure */

  Amount *load;            /** Vehicle's load after step completion array */
  size_t load_size;        /** Vehicle's load array size */
};

#endif  // INCLUDE_C_TYPES_VROOM_RT_H_
