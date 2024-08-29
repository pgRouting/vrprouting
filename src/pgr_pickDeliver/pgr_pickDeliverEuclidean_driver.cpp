/*PGR-GNU*****************************************************************
File: pickDeliver_driver.cpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo

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

#include "drivers/pgr_pickDeliverEuclidean_driver.h"

#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "c_types/solution_rt.h"

#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"

#include "cpp_common/orders_t.hpp"
#include "cpp_common/vehicle_t.hpp"
#include "initialsol/simple.hpp"
#include "optimizers/simple.hpp"
#include "problem/matrix.hpp"
#include "problem/pickDeliver.hpp"

namespace {

vrprouting::problem::Solution
get_initial_solution(vrprouting::problem::PickDeliver &problem_ptr, int m_initial_id) {
    using Solution = vrprouting::problem::Solution;
    using Initial_solution = vrprouting::initialsol::simple::Initial_solution;
    using Initials_code = vrprouting::initialsol::simple::Initials_code;
    Solution m_solutions(problem_ptr);
    if (m_initial_id == 0) {
        for (int i = 1; i < 7; ++i) {
            if (i == 1) {
                m_solutions = Initial_solution((Initials_code)i, problem_ptr);
            } else {
                auto new_sol = Initial_solution((Initials_code)i, problem_ptr);
                m_solutions = (new_sol < m_solutions)? new_sol : m_solutions;
            }
        }
    } else {
        m_solutions = Initial_solution((Initials_code)m_initial_id, problem_ptr);
    }

    return m_solutions;
}

bool
are_shipments_ok(
        Orders_t *orders_arr,
        size_t total_orders,
        std::string *err_string,
        std::string *hint_string) {
    /**
     * - demand > 0 (the type is unsigned so no need to check for negative values, only for it to be non 0
     * - pick_service_t >=0
     * - drop_service_t >=0
     * - p_open <= p_close
     * - d_open <= d_close
     */
    for (size_t i = 0; i < total_orders; ++i) {
        auto o = orders_arr[i];
        if (o.demand == 0) {
            *err_string = "Unexpected zero value found on column 'demand' of shipments";
            *hint_string = "Check shipment id #:" + std::to_string(o.id);
            return false;
        }

        if (o.pick_service_t < 0) {
            *err_string = "Unexpected negative value found on column 'p_service_t' of shipments";
            *hint_string = "Check shipment id #:" + std::to_string(o.id);
            return false;
        }

        if (o.deliver_service_t < 0) {
            *err_string = "Unexpected negative value found on column 'd_service_t' of shipments";
            *hint_string = "Check shipment id #:" + std::to_string(o.id);
            return false;
        }

        if (o.pick_open_t > o.pick_close_t) {
            *err_string = "Unexpected pickup time windows found on shipments";
            *hint_string = "Check shipment id #:" + std::to_string(o.id);
            return false;
        }

        if (o.deliver_open_t > o.deliver_close_t) {
            *err_string = "Unexpected delivery time windows found on shipments";
            *hint_string = "Check shipment id #:" + std::to_string(o.id);
            return false;
        }
    }
    return true;
}

}  // namespace

void
vrp_do_pgr_pickDeliverEuclidean(
        Orders_t *orders_arr, size_t total_orders,
        Vehicle_t *vehicles_arr, size_t total_vehicles,

        double factor,
        int max_cycles,
        int initial_solution_id,

        Solution_rt **return_tuples,
        size_t *return_count,

        char **log_msg,
        char **notice_msg,
        char **err_msg) {
    using vrprouting::alloc;
    using vrprouting::free;
    using vrprouting::to_pg_msg;

    char* hint = nullptr;

    std::ostringstream log;
    std::ostringstream notice;
    std::ostringstream err;
    try {
        /*
         * verify preconditions
         */
        pgassert(!(*log_msg));
        pgassert(!(*notice_msg));
        pgassert(!(*err_msg));
        pgassert(*return_count == 0);
        pgassert(!(*return_tuples));

        std::string err_string;
        std::string hint_string;

        if (!are_shipments_ok(orders_arr, total_orders, &err_string, &hint_string)) {
            *err_msg = to_pg_msg(err_string);
            *log_msg = to_pg_msg(hint_string);
            return;
        }

        /*
         * transform to C++ containers
         */
        std::vector<Orders_t> orders(
                orders_arr, orders_arr + total_orders);
        std::vector<Vehicle_t> vehicles(
                vehicles_arr, vehicles_arr + total_vehicles);

        /*
         * Prepare identifiers
         */
        std::map<std::pair<Coordinate, Coordinate>, Id> matrix_data;

        for (const auto &o : orders) {
            pgassert(o.pick_node_id == 0);
            pgassert(o.deliver_node_id == 0);
            matrix_data[std::pair<Coordinate, Coordinate>(o.pick_x, o.pick_y)] = 0;
            matrix_data[std::pair<Coordinate, Coordinate>(o.deliver_x, o.deliver_y)] = 0;
        }

        for (const auto &v : vehicles) {
            matrix_data[std::pair<Coordinate, Coordinate>(v.start_x, v.start_y)] = 0;
            matrix_data[std::pair<Coordinate, Coordinate>(v.end_x, v.end_y)] = 0;
        }

        Identifiers<int64_t> unique_ids;
        /*
         * Data does not have ids for the locations'
         */
        Id id(0);
        for (auto &e : matrix_data) {
            e.second = id++;
        }

        for (const auto &e : matrix_data) {
            unique_ids += e.second;
            log << e.second << "(" << e.first.first << "," << e.first.second << ")\n";
        }

        for (size_t i = 0; i < total_orders; ++i) {
            auto &o = orders_arr[i];
            o.pick_node_id    = matrix_data[std::pair<Coordinate, Coordinate>(o.pick_x, o.pick_y)];
            o.deliver_node_id = matrix_data[std::pair<Coordinate, Coordinate>(o.deliver_x, o.deliver_y)];
        }

        for (auto &v : vehicles) {
            v.start_node_id = matrix_data[std::pair<Coordinate, Coordinate>(v.start_x, v.start_y)];
            v.end_node_id = matrix_data[std::pair<Coordinate, Coordinate>(v.end_x, v.end_y)];
        }

        /*
         * Prepare matrix
         */
        vrprouting::problem::Matrix matrix(matrix_data, static_cast<Multiplier>(factor));

        /*
         * Construct problem
         */
        vrprouting::problem::PickDeliver pd_problem(
                orders_arr, total_orders,
                vehicles_arr, total_vehicles,
                matrix);

        if (pd_problem.msg.has_error()) {
            *log_msg = to_pg_msg(pd_problem.msg.get_log());
            *err_msg = to_pg_msg(pd_problem.msg.get_error());
            return;
        }
        log << pd_problem.msg.get_log();
        log << "Finish constructing problem\n";
        pd_problem.msg.clear();

        /*
         * get initial solutions
         */
        auto sol = get_initial_solution(pd_problem, initial_solution_id);

        /*
         * Solve (optimize)
         */
        using Optimize = vrprouting::optimizers::simple::Optimize;
        using Initials_code = vrprouting::initialsol::simple::Initials_code;
        sol = Optimize(sol, static_cast<size_t>(max_cycles), (Initials_code)initial_solution_id);

        /*
         * get the solution
         */
        auto solution = sol.get_postgres_result();
        log << sol.get_log();
        log << "solution size: " << solution.size() << "\n";

        /*
         * Prepare results
         */
        if (!solution.empty()) {
            (*return_tuples) = alloc(solution.size(), (*return_tuples));
            int seq = 0;
            for (const auto &row : solution) {
                (*return_tuples)[seq] = row;
                ++seq;
            }
        }
        (*return_count) = solution.size();

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
