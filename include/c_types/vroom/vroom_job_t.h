/*PGR-GNU*****************************************************************
File: vroom_job_t.h

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

#ifndef INCLUDE_C_TYPES_VROOM_VROOM_JOB_T_H_
#define INCLUDE_C_TYPES_VROOM_VROOM_JOB_T_H_
#pragma once

#include "c_types/typedefs.h"

/** @brief Job's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | The job's identifier
location_id | Location index of job in matrix
setup | Job setup duration
service | Job service duration
delivery | Quantities for delivery
delivery_size | Number of delivery quantities
pickup | Quantities for pickup
pickup_size | Number of pickup quantities
skills | Mandatory skills
skills_size | Number of mandatory skills
priority | Priority level of job
data | Metadata information of job
*/
struct Vroom_job_t {
  Idx id; /** The job's identifier */
  MatrixIndex location_id; /** Location index of job in matrix */

  Duration setup; /** Job setup duration */
  Duration service; /** Job service duration */

  Amount *delivery; /** Quantities for delivery */
  size_t delivery_size; /** Number of delivery quantities */

  Amount *pickup; /** Quantities for pickup */
  size_t pickup_size; /** Number of pickup quantities */

  Skill *skills; /** Mandatory skills */
  size_t skills_size; /** Number of mandatory skills */

  Priority priority; /** Priority level of job */

  char *data; /** Metadata information of job */
};

#endif  // INCLUDE_C_TYPES_VROOM_VROOM_JOB_T_H_
