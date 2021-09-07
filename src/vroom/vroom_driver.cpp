/*PGR-GNU*****************************************************************
File: vroom_driver.cpp

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

#include "drivers/vroom/vroom_driver.h"

#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>

#include "c_common/pgr_alloc.hpp"
#include "cpp_common/pgr_assert.h"
#include "cpp_common/vrp_vroom_problem.hpp"

/** @file vroom_driver.cpp
 * @brief Handles actual calling of function in the `vrp_vroom.hpp` file.
 *
 */

/** @brief Performs exception handling and converts the results to postgres.
 *
 * @pre log_msg is empty
 * @pre notice_msg is empty
 * @pre err_msg is empty
 * @pre return_tuples is empty
 * @pre return_count is 0
 *
 * It converts the C types to the C++ types, and passes these variables to the
 * function `vrp_vroom` which calls the main function defined in the
 * C++ Header file. It also does exception handling.
 *
 * @param jobs_sql          SQL query describing the jobs
 * @param jobs_tw_sql       SQL query describing the time window for jobs
 * @param shipments_sql     SQL query describing the shipments
 * @param shipments_tw_sql  SQL query describing the time windows for shipment
 * @param vehicles_sql      SQL query describing the vehicles
 * @param breaks_sql        SQL query describing the driver breaks.
 * @param breaks_tws_sql    SQL query describing the time windows for break start.
 * @param matrix_sql        SQL query describing the cells of the cost matrix
 * @param result_tuples     the rows in the result
 * @param result_count      the count of rows in the result
 * @param log_msg           stores the log message
 * @param notice_msg        stores the notice message
 * @param err_msg           stores the error message
 *
 * @returns void
 */
void
do_vrp_vroom(
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
    char ** err_msg) {
  std::ostringstream log;
  std::ostringstream err;
  std::ostringstream notice;
  try {
    pgassert(!(*log_msg));
    pgassert(!(*notice_msg));
    pgassert(!(*err_msg));
    pgassert(!(*return_tuples));
    pgassert(!(*return_count));
    pgassert(jobs || shipments);
    pgassert(vehicles);
    pgassert(matrix_cells_arr);
    pgassert(total_jobs || total_shipments);
    pgassert(total_vehicles);
    pgassert(total_cells);

    Identifiers<Id> location_ids;

    for (size_t i = 0; i < total_jobs; ++i) {
      location_ids += jobs[i].location_index;
    }

    for (size_t i = 0; i < total_shipments; ++i) {
      location_ids += shipments[i].p_location_index;
      location_ids += shipments[i].d_location_index;
    }

    for (size_t i = 0; i < total_vehicles; ++i) {
      if (vehicles[i].start_index != -1) {
        location_ids += vehicles[i].start_index;
      }
      if (vehicles[i].end_index != -1) {
        location_ids += vehicles[i].end_index;
      }
    }

    vrprouting::base::Base_Matrix time_matrix(matrix_cells_arr, total_cells, location_ids);

    /*
     * Verify matrix cells preconditions
     */
    if (!time_matrix.has_no_infinity()) {
      (*return_tuples) = NULL;
      (*return_count) = 0;
      err << "An Infinity value was found on the Matrix. Might be missing information of a node";
      *err_msg = pgr_msg(err.str());
      return;
    }

    /*
     * Verify size of matrix cell lies in the limit
     */
    if (time_matrix.size() > (std::numeric_limits<vroom::Index>::max)()) {
      (*return_tuples) = NULL;
      (*return_count) = 0;
      err << "The size of time matrix exceeds the limit";
      *err_msg = pgr_msg(err.str());
      return;
    }

    vrprouting::Vrp_vroom_problem problem;
    problem.add_matrix(time_matrix);
    problem.add_vehicles(vehicles, total_vehicles,
                         breaks, total_breaks,
                         breaks_tws, total_breaks_tws);
    problem.add_jobs(jobs, total_jobs,
                     jobs_tws, total_jobs_tws);
    problem.add_shipments(shipments, total_shipments,
                          shipments_tws, total_shipments_tws);

    std::vector < Vroom_rt > results = problem.solve();

    auto count = results.size();
    if (count == 0) {
      (*return_tuples) = NULL;
      (*return_count) = 0;
      notice << "No results found";
      *notice_msg = notice.str().empty()?
        *notice_msg :
        pgr_msg(notice.str().c_str());
      return;
    }

    (*return_tuples) = pgr_alloc(count, (*return_tuples));
    for (size_t i = 0; i < count; i++) {
      *((*return_tuples) + i) = results[i];
    }

    (*return_count) = count;

    pgassert(*err_msg == NULL);
    *log_msg = log.str().empty()?
      *log_msg :
      pgr_msg(log.str().c_str());
    *notice_msg = notice.str().empty()?
      *notice_msg :
      pgr_msg(notice.str().c_str());
  } catch (AssertFailedException &except) {
    (*return_tuples) = pgr_free(*return_tuples);
    (*return_count) = 0;
    err << except.what();
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch (const vroom::Exception &except) {
    (*return_tuples) = pgr_free(*return_tuples);
    (*return_count) = 0;
    err << except.message;
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch (std::exception &except) {
    (*return_tuples) = pgr_free(*return_tuples);
    (*return_count) = 0;
    err << except.what();
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch(...) {
    (*return_tuples) = pgr_free(*return_tuples);
    (*return_count) = 0;
    err << "Caught unknown exception!";
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  }
}
