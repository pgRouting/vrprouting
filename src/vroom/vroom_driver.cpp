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

#include "drivers/vroom_driver.h"

#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <cmath>

#include "vroom/vroom.hpp"

#include "c_types/vroom_rt.h"
#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"
#include "cpp_common/identifiers.hpp"
#include "cpp_common/base_matrix.hpp"
#include "cpp_common/vroom_vehicle_t.hpp"
#include "cpp_common/vroom_shipment_t.hpp"
#include "cpp_common/vroom_job_t.hpp"

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
 * @param[in] jobs                Pointer to the array of jobs
 * @param[in] total_jobs          The total number of jobs
 * @param[in] jobs_tws            Pointer to the array of jobs time windows
 * @param[in] total_jobs_tws      The total number of jobs time windows
 * @param[in] shipments           Pointer to the array of shipments
 * @param[in] total_shipments     The total number of shipments
 * @param[in] shipments_tws       Pointer to the array of shipments time windows
 * @param[in] total_shipments_tws The total number of shipments time windows
 * @param[in] vehicles            Pointer to the array of vehicles
 * @param[in] total_vehicles      The total number of total vehicles
 * @param[in] breaks              Pointer to the array of breaks
 * @param[in] total_breaks        The total number of total breaks
 * @param[in] breaks_tws          Pointer to the array of break timewindows
 * @param[in] total_breaks_tws    The total number of total breaks timewindows
 * @param[in] matrix_rows         Pointer to the array of matrix rows
 * @param[in] total_matrix_rows   The total number of matrix rows
 * @param[in] exploration_level   Exploration level to use while solving.
 * @param[in] timeout             Timeout value to stop the solving process (seconds).
 * @param[in] loading_time        Additional time spent in loading the data from SQL Query (seconds).
 * @param[out] return_tuples      The rows in the result
 * @param[out] return_count       The count of rows in the result
 * @param[out] log_msg            Stores the log message
 * @param[out] notice_msg         Stores the notice message
 * @param[out] err_msg            Stores the error message
 */
void
vrp_do_vroom(
        Vroom_job_t *jobs, size_t total_jobs,
        Vroom_time_window_t *jobs_tws, size_t total_jobs_tws,
        Vroom_shipment_t *shipments, size_t total_shipments,
        Vroom_time_window_t *shipments_tws, size_t total_shipments_tws,
        Vroom_vehicle_t *vehicles, size_t total_vehicles,
        Vroom_break_t *breaks, size_t total_breaks,
        Vroom_time_window_t *breaks_tws, size_t total_breaks_tws,
        Vroom_matrix_t *matrix_rows, size_t total_matrix_rows,

        int32_t exploration_level,
        int32_t timeout,
        int32_t loading_time,

        Vroom_rt **return_tuples,
        size_t *return_count,

        char ** log_msg,
        char ** notice_msg,
        char ** err_msg) {
    using vrprouting::alloc;
    using vrprouting::to_pg_msg;
    using vrprouting::free;

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
        pgassert(matrix_rows);
        pgassert(total_jobs || total_shipments);
        pgassert(total_vehicles);
        pgassert(total_matrix_rows);

        auto start_time = std::chrono::high_resolution_clock::now();

        Identifiers<Id> location_ids;

        for (size_t i = 0; i < total_jobs; ++i) {
            location_ids += jobs[i].location_id;
        }

        for (size_t i = 0; i < total_shipments; ++i) {
            location_ids += shipments[i].p_location_id;
            location_ids += shipments[i].d_location_id;
        }

        double min_speed_factor, max_speed_factor;
        min_speed_factor = max_speed_factor = vehicles[0].speed_factor;

        for (size_t i = 0; i < total_vehicles; ++i) {
            min_speed_factor = std::min(min_speed_factor, vehicles[i].speed_factor);
            max_speed_factor = std::max(max_speed_factor, vehicles[i].speed_factor);
            if (vehicles[i].start_id != -1) {
                location_ids += vehicles[i].start_id;
            }
            if (vehicles[i].end_id != -1) {
                location_ids += vehicles[i].end_id;
            }
        }

        /*
         * Verify that max value of speed factor is not greater
         * than 5 times the speed factor of any other vehicle.
         */
        if (max_speed_factor > 5 * min_speed_factor) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            err << "The speed_factor " << max_speed_factor << " is more than five times "
                "the speed factor " << min_speed_factor;
            *err_msg = to_pg_msg(err.str());
            return;
        }

        /*
         * Scale the vehicles speed factors according to the minimum speed factor
         */
        for (size_t i = 0; i < total_vehicles; ++i) {
            vehicles[i].speed_factor = std::round(vehicles[i].speed_factor / min_speed_factor);
        }

        /*
         * Create the matrix. Also, scale the time matrix according to min_speed_factor
         */
        vrprouting::base::Base_Matrix matrix(matrix_rows, total_matrix_rows,
                location_ids, min_speed_factor);

        /*
         * Verify matrix cells preconditions
         */
        if (!matrix.has_no_infinity()) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            err << "An Infinity value was found on the Matrix. Might be missing information of a node";
            *err_msg = to_pg_msg(err.str());
            return;
        }

        /*
         * Verify size of matrix cell lies in the limit
         */
        if (matrix.size() > (std::numeric_limits<vroom::Index>::max)()) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            err << "The size of time matrix exceeds the limit";
            *err_msg = to_pg_msg(err.str());
            return;
        }

        vrprouting::problem::Vroom problem;
        problem.add_matrix(matrix);
        problem.add_vehicles(vehicles, total_vehicles,
                breaks, total_breaks,
                breaks_tws, total_breaks_tws);
        problem.add_jobs(jobs, total_jobs,
                jobs_tws, total_jobs_tws);
        problem.add_shipments(shipments, total_shipments,
                shipments_tws, total_shipments_tws);

        auto end_time = std::chrono::high_resolution_clock::now();
        loading_time += static_cast<int32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time)
                .count());

        std::vector < Vroom_rt > results = problem.solve(exploration_level, timeout, loading_time);

        auto count = results.size();
        if (count == 0) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            notice << "No results found";
            *notice_msg = notice.str().empty()?
                *notice_msg :
                to_pg_msg(notice.str().c_str());
            return;
        }

        (*return_tuples) = alloc(count, (*return_tuples));
        for (size_t i = 0; i < count; i++) {
            *((*return_tuples) + i) = results[i];
        }

        (*return_count) = count;

        pgassert(*err_msg == NULL);
        *log_msg = log.str().empty()?
            *log_msg :
            to_pg_msg(log.str().c_str());
        *notice_msg = notice.str().empty()?
            *notice_msg :
            to_pg_msg(notice.str().c_str());
    } catch (AssertFailedException &except) {
        (*return_tuples) = free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = to_pg_msg(err.str().c_str());
        *log_msg = to_pg_msg(log.str().c_str());
    } catch (const vroom::Exception &except) {
        (*return_tuples) = free(*return_tuples);
        (*return_count) = 0;
        err << except.message;
        *err_msg = to_pg_msg(err.str().c_str());
        *log_msg = to_pg_msg(log.str().c_str());
    } catch (std::exception &except) {
        (*return_tuples) = free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = to_pg_msg(err.str().c_str());
        *log_msg = to_pg_msg(log.str().c_str());
    } catch(...) {
        (*return_tuples) = free(*return_tuples);
        (*return_count) = 0;
        err << "Caught unknown exception!";
        *err_msg = to_pg_msg(err.str().c_str());
        *log_msg = to_pg_msg(log.str().c_str());
    }
}
