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
#include "cpp_common/pgdata_getters.hpp"

#include "cpp_common/identifiers.hpp"
#include "cpp_common/vroom_job_t.hpp"
#include "cpp_common/vroom_matrix_t.hpp"
#include "cpp_common/vroom_vehicle_t.hpp"
#include "cpp_common/vroom_shipment_t.hpp"
#include "cpp_common/vroom_break_t.hpp"
#include "cpp_common/vroom_time_window_t.hpp"

#include "vroom/vroom.hpp"

void
vrp_do_vroom(
        char *jobs_sql,
        char *jobs_tws_sql,
        char *shipments_sql,
        char *shipments_tws_sql,
        char *vehicles_sql,
        char *breaks_sql,
        char *breaks_tws_sql,
        char *matrix_sql,

        int32_t exploration_level,
        int32_t timeout,

        int16_t fn_used,
        bool use_timestamps,

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
        using Matrix = vrprouting::vroom::Matrix;
        using vrprouting::pgget::vroom::get_matrix;
        using vrprouting::pgget::vroom::get_breaks;
        using vrprouting::pgget::vroom::get_timewindows;
        using vrprouting::pgget::vroom::get_jobs;
        using vrprouting::pgget::vroom::get_shipments;
        using vrprouting::pgget::vroom::get_vehicles;

        /*
         * verify preconditions
         */
        pgassert(!(*log_msg));
        pgassert(!(*notice_msg));
        pgassert(!(*err_msg));
        pgassert(!(*return_tuples));
        pgassert(!(*return_count));

        /*
         * Verify that both jobs_sql and shipments_sql are not NULL
         */
        if (!jobs_sql && !shipments_sql) {
            if (fn_used == 0) {
                *err_msg = to_pg_msg("Both Jobs SQL and Shipments NULL must not be NULL");
                return;
            } else if (fn_used == 1) {
                *err_msg = to_pg_msg("Jobs SQL must not be NULL");
                return;
            } else if (fn_used == 2) {
                *err_msg = to_pg_msg("Shipments SQL must not be NULL");
                return;
            }
        }

        if (!vehicles_sql) {
            *err_msg = to_pg_msg("Vehicles SQL must not be NULL");
            return;
        }

        if (!matrix_sql) {
            *err_msg = to_pg_msg("Matrix SQL must not be NULL");
            return;
        }

        auto start_time = std::chrono::high_resolution_clock::now();

        /* Data input starts */

        hint = jobs_sql;
        auto jobs  = get_jobs(
                jobs_sql? std::string(jobs_sql) : std::string(),
                use_timestamps);

        hint = shipments_sql;
        auto shipments  = get_shipments(
                shipments_sql? std::string(shipments_sql) : std::string(),
                use_timestamps);

        if (jobs.empty() && shipments.empty()) {
            if (fn_used == 0) {
                if (!shipments_sql || !jobs_sql) {
                    *notice_msg = to_pg_msg("Jobs SQL and/or Shipments SQL query are NULL");
                    return;
                }
                *notice_msg = to_pg_msg("Insufficient data found on Jobs SQL and/or Shipments SQL query.");
                auto s1 = shipments_sql? std::string(shipments_sql) : "";
                auto s2 = jobs_sql? std::string(jobs_sql) : "";
                s1 = s1 + " " + s2;
                *log_msg = to_pg_msg(s1);
            } else if (fn_used == 1) {
                *notice_msg = jobs_sql?
                    to_pg_msg("Insufficient data found on Jobs SQL query.")
                    : to_pg_msg("Jobs SQL query not found");
                *log_msg = jobs_sql? to_pg_msg(std::string(jobs_sql)) : nullptr;
            } else if (fn_used == 2) {
                *notice_msg = shipments_sql?
                    to_pg_msg("Insufficient data found on Shipments SQL query.")
                    : to_pg_msg("Jobs SQL query not found");
                *log_msg = shipments_sql? to_pg_msg(std::string(shipments_sql)) : nullptr;
            }
            return;
        }

        hint = jobs_tws_sql;
        auto jobs_tw  = get_timewindows(
                jobs_tws_sql? std::string(jobs_tws_sql) : std::string(),
                use_timestamps, false);

        hint = shipments_tws_sql;
        auto shipments_tw  = get_timewindows(
                shipments_tws_sql? std::string(shipments_tws_sql) : std::string(),
                use_timestamps, true);

        hint = vehicles_sql;
        auto vehicles = get_vehicles(vehicles_sql, use_timestamps);

        if (vehicles.size() == 0) {
            *notice_msg = vehicles_sql?
                to_pg_msg("Insufficient data found on Vehicles SQL query.")
                : to_pg_msg("Vehicles SQL query not found");
            *log_msg = vehicles_sql? to_pg_msg(std::string(vehicles_sql)) : nullptr;
            return;
        }

        hint = breaks_sql;
        auto breaks = get_breaks(
                breaks_sql? std::string(breaks_sql) : std::string(),
                use_timestamps);

        hint = breaks_tws_sql;
        auto breaks_tw = get_timewindows(
                breaks_tws_sql? std::string(breaks_tws_sql) : std::string(),
                use_timestamps, false);

        hint = matrix_sql;
        auto costs = get_matrix(matrix_sql, use_timestamps);

        if (costs.size() == 0) {
            *notice_msg = matrix_sql?  to_pg_msg("Insufficient data found on Matrix SQL query.")
                : to_pg_msg("Matrix SQL query not found");
            *log_msg = matrix_sql? to_pg_msg(std::string(matrix_sql)) : nullptr;
            return;
        }

        Identifiers<Id> location_ids;

        for (const auto &j : jobs) {
            location_ids += j.location_id;
        }

        for (const auto &s : shipments) {
            location_ids += s.p_location_id;
            location_ids += s.d_location_id;
        }

        double min_speed_factor, max_speed_factor;
        min_speed_factor = max_speed_factor = vehicles[0].speed_factor;

        for (const auto &v : vehicles) {
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
        for (auto &v : vehicles) {
            v.speed_factor = std::round(v.speed_factor / min_speed_factor);
        }

        /*
         * Create the matrix. Also, scale the time matrix according to min_speed_factor
         */
        Matrix matrix(costs, location_ids, min_speed_factor);

        /*
         * Verify size of matrix cell lies in the limit
         */
        if (matrix.size() > (std::numeric_limits<::vroom::Index>::max)()) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            err << "The size of time matrix exceeds the limit";
            *err_msg = to_pg_msg(err.str());
            return;
        }

        vrprouting::problem::Vroom problem;
        problem.add_matrix(matrix);
        problem.add_vehicles(vehicles, breaks, breaks_tw);
        problem.add_jobs(jobs, jobs_tw);
        problem.add_shipments(shipments, shipments_tw);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto loading_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

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
