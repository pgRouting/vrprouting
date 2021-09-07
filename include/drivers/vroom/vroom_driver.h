/*PGR-GNU*****************************************************************
File: vroom_driver.h

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

#ifndef INCLUDE_DRIVERS_VROOM_VROOM_DRIVER_H_
#define INCLUDE_DRIVERS_VROOM_VROOM_DRIVER_H_
#pragma once

#include "c_types/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

  void do_vrp_vroom(
      Vroom_job_t *jobs, size_t total_jobs,
      Vroom_time_window_t *jobs_tws, size_t total_jobs_tws,
      Vroom_shipment_t *shipments, size_t total_shipments,
      Vroom_time_window_t *shipments_tws, size_t total_shipments_tws,
      Vroom_vehicle_t *vehicles, size_t total_vehicles,
      Vroom_break_t *breaks, size_t total_breaks,
      Vroom_time_window_t *breaks_tws, size_t total_breaks_tws,
      Matrix_cell_t *matrix_cells_arr, size_t total_cells,

      Vroom_rt **return_tuples,
      size_t *return_count,

      char ** log_msg,
      char ** notice_msg,
      char ** err_msg);

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_DRIVERS_VROOM_VROOM_DRIVER_H_
