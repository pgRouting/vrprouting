/*PGR-GNU*****************************************************************
File: vroom_job_t.hpp

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

#ifndef INCLUDE_CPP_COMMON_VROOM_JOB_T_HPP_
#define INCLUDE_CPP_COMMON_VROOM_JOB_T_HPP_
#pragma once

#include <structures/vroom/job.h>

#include <string>
#include <vector>
#include "c_types/typedefs.h"

namespace vrprouting {

/** @brief Job's attributes

@note C/C++/postgreSQL connecting structure for input
name | description
:----- | :-------
id | The job's identifier
location_id | Location index of job in matrix
setup | Job setup duration
service | Job service duration
delivery | Quantities for delivery
pickup | Quantities for pickup
skills | Mandatory skills
priority | Priority level of job
data | Metadata information of job
*/
class Vroom_job_t {
 public:
     Idx id; /** The job's identifier */
     MatrixIndex location_id;    /** Location index of job in matrix */

     ::vroom::Duration setup;   /** Job setup duration */
     ::vroom::Duration service; /** Job service duration */

     ::vroom::Amount pickup;    /** Quantities for pickup */
     ::vroom::Amount delivery;  /** Quantities for delivery */

     ::vroom::Skills skills;    /** Mandatory skills */

     Priority priority;         /** Priority level of job */

     std::string data;          /** Metadata information of job */
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VROOM_JOB_T_HPP_
