/*PGR-GNU*****************************************************************
File: pickDeliver_driver.cpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2021 Celia Virginia Vergara Castillo
Copyright (c) 2021 Joseph Emile Honour Percival

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
/** @file */


#include "drivers/pickDeliver_driver.h"

#include <utility>
#include <sstream>
#include <string>

#include "c_types/solution_rt.h"

#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"

#include "cpp_common/orders_t.hpp"
#include "cpp_common/vehicle_t.hpp"
#include "initialsol/tabu.hpp"
#include "optimizers/tabu.hpp"
#include "problem/matrix.hpp"
#include "problem/pickDeliver.hpp"

void
vrp_do_pickDeliver(
        Orders_t *customers_arr, size_t total_customers,
        Vehicle_t *vehicles_arr, size_t total_vehicles,
        Matrix_cell_t *matrix_cells_arr, size_t total_cells,
        Time_multipliers_t *multipliers_arr, size_t total_multipliers,


        bool optimize,
        double factor,
        int max_cycles,
        bool stop_on_all_served,
        int64_t execution_date,

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
        pgassert(total_customers);
        pgassert(total_vehicles);
        pgassert(total_cells);
        pgassert(*return_count == 0);
        pgassert(!(*return_tuples));
        log << "do_pickDeliver\n";

        Identifiers<Id> node_ids;
        Identifiers<Id> order_ids;

        /*
         * nodes involved & orders involved
         */
        for (size_t i = 0; i < total_customers; ++i) {
            auto o = customers_arr[i];
            node_ids += o.pick_node_id;
            node_ids += o.deliver_node_id;
            order_ids += o.id;
        }

        bool missing = false;
        for (size_t i = 0; i < total_vehicles; ++i) {
            auto v = vehicles_arr[i];
            node_ids += v.start_node_id;
            node_ids += v.end_node_id;
            for (size_t j = 0; j < v.stops_size; ++j) {
                auto s = v.stops[j];
                if (!order_ids.has(s)) {
                    if (!missing) err << "Order in 'stops' information missing";
                    missing = true;
                    err << "Missing information of order " << s << "\n";
                }
            }
            if (missing) {
                *err_msg = to_pg_msg(err.str());
                return;
            }
        }

        /*
         * Prepare matrix
         */
        vrprouting::problem::Matrix cost_matrix(
                matrix_cells_arr, total_cells,
                multipliers_arr, total_multipliers,
                node_ids, static_cast<Multiplier>(factor));

        /*
         * Verify matrix triangle inequality
         */
        if (!cost_matrix.obeys_triangle_inequality()) {
            log << "[PickDeliver] Fixing Matrix that does not obey triangle inequality ";
            log << cost_matrix.fix_triangle_inequality() << " cycles used";

            if (!cost_matrix.obeys_triangle_inequality()) {
                log << "[pickDeliver] Matrix Still does not obey triangle inequality ";
            }
        }

        /*
         * Verify matrix cells preconditions
         */
        if (!cost_matrix.has_no_infinity()) {
            err << "An Infinity value was found on the Matrix";
            *err_msg = to_pg_msg(err.str());
            return;
        }


        log << "stop_on_all_served" << stop_on_all_served << "\n";
        log << "execution_date" << execution_date << "\n";
        log << "Initialize problem\n";
        /*
         * Construct problem
         */
        vrprouting::problem::PickDeliver pd_problem(
                customers_arr, total_customers,
                vehicles_arr, total_vehicles,
                cost_matrix);

        err << pd_problem.msg.get_error();
        if (!err.str().empty()) {
            log << pd_problem.msg.get_error();
            log << pd_problem.msg.get_log();
            *log_msg = to_pg_msg(log.str());
            *err_msg = to_pg_msg(err.str());
            return;
        }
        log << pd_problem.msg.get_log();
        pd_problem.msg.clear();

        log << "Finish Initialize problem\n";

        /*
         * get initial solutions
         */
        using Initial_solution = vrprouting::initialsol::tabu::Initial_solution;
        using Solution = vrprouting::problem::Solution;
        auto sol = static_cast<Solution>(Initial_solution(execution_date, optimize, pd_problem));

        /*
         * Solve (optimize)
         */
        using Optimize = vrprouting::optimizers::tabu::Optimize;
        sol = Optimize(sol, static_cast<size_t>(max_cycles), stop_on_all_served, optimize);

        log << pd_problem.msg.get_log();
        pd_problem.msg.clear();
        log << "Finish solve\n";

        /*
         * get the solution
         */
        auto solution = sol.get_postgres_result();
        log << pd_problem.msg.get_log();
        pd_problem.msg.clear();
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
