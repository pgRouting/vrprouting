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

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "c_types/vroom_rt.h"

#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"

#include "cpp_common/identifiers.hpp"
#include "cpp_common/vroom_job_t.hpp"
#include "cpp_common/vroom_matrix.hpp"
#include "cpp_common/vroom_vehicle_t.hpp"
#include "cpp_common/vroom_shipment_t.hpp"
#include "vroom/vroom.hpp"

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

        char **log_msg,
        char **notice_msg,
        char **err_msg) {
    using vrprouting::alloc;
    using vrprouting::free;
    using vrprouting::to_pg_msg;

    char* hint = nullptr;

    std::ostringstream log;
    std::ostringstream err;
    std::ostringstream notice;
    try {
        /*
         * verify preconditions
         */
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

        using Matrix = vrprouting::vroom::Matrix;

        auto start_time = std::chrono::high_resolution_clock::now();

        Identifiers<Id> location_ids;

        for (size_t i = 0; i < total_jobs; ++i) {
            location_ids += jobs[i].location_id;
        }

        for (size_t i = 0; i < total_shipments; ++i) {
            auto s = shipments[i];
            location_ids += s.p_location_id;
            location_ids += s.d_location_id;
        }

        double min_speed_factor, max_speed_factor;
        min_speed_factor = max_speed_factor = vehicles[0].speed_factor;

        for (size_t i = 0; i < total_vehicles; ++i) {
            auto v = vehicles[i];
            min_speed_factor = std::min(min_speed_factor, v.speed_factor);
            max_speed_factor = std::max(max_speed_factor, v.speed_factor);
            if (v.start_id != -1) {
                location_ids += v.start_id;
            }
            if (v.end_id != -1) {
                location_ids += v.end_id;
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
        Matrix matrix(matrix_rows, total_matrix_rows, location_ids, min_speed_factor);

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
        problem.add_vehicles(vehicles, total_vehicles, breaks, total_breaks, breaks_tws, total_breaks_tws);
        problem.add_jobs(jobs, total_jobs, jobs_tws, total_jobs_tws);
        problem.add_shipments(shipments, total_shipments, shipments_tws, total_shipments_tws);

        auto end_time = std::chrono::high_resolution_clock::now();
        loading_time += static_cast<int32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time)
                .count());

        std::vector<Vroom_rt> results = problem.solve(exploration_level, timeout, loading_time);

        auto count = results.size();
        if (count == 0) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            notice << "No results found";
            *notice_msg = notice.str().empty()?
                *notice_msg :
                to_pg_msg(notice.str());
            return;
        }

        (*return_tuples) = alloc(count, (*return_tuples));
        for (size_t i = 0; i < count; i++) {
            *((*return_tuples) + i) = results[i];
        }

        (*return_count) = count;

        pgassert(*err_msg == nullptr);
        *log_msg = log.str().empty()? nullptr : to_pg_msg(log.str());
        *notice_msg = notice.str().empty()? nullptr : to_pg_msg(notice.str());
    } catch (AssertFailedException &except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        *err_msg = to_pg_msg(except.what());
        *log_msg = to_pg_msg(log.str());
    } catch (std::exception& except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        *err_msg = to_pg_msg(except.what());
        *log_msg = to_pg_msg(log.str());
    } catch (const std::string &except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        *err_msg = to_pg_msg(except);
        *log_msg = hint? to_pg_msg(hint) : to_pg_msg(log.str());
    } catch (const std::pair<std::string, std::string>& ex) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        *err_msg = to_pg_msg(ex.first);
        *log_msg = to_pg_msg(ex.second);
    } catch (const std::pair<std::string, int64_t>& except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        log << "id = " << except.second;
        *err_msg = to_pg_msg(except.first);
        *log_msg = to_pg_msg(log.str());
    } catch(...) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << "Caught unknown exception!";
        *err_msg = to_pg_msg(err.str());
        *log_msg = to_pg_msg(log.str());
    }
}
