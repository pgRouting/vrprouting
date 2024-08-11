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

#include <cstring>
#include <sstream>
#include <string>
#include <deque>

#include "problem/pickDeliver.hpp"
#include "cpp_common/orders_t.hpp"
#include "c_types/solution_rt.h"
#include "cpp_common/vehicle_t.hpp"
#include "problem/matrix.hpp"

#include "cpp_common/assert.hpp"
#include "initialsol/tabu.hpp"
#include "optimizers/tabu.hpp"
#include "cpp_common/alloc.hpp"

/**
 *
 *  @param[in] customers_arr A C Array of pickup and dropoff orders
 *  @param[in] total_customers size of the customers_arr
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *  @param[in] matrix_cells_arr A C Array of the (time) matrix cells
 *  @param[in] total_cells size of the matrix_cells_arr
 *  @param[in] multipliers_arr A C Array of the multipliers
 *  @param[in] total_multipliers size of the multipliers_arr
 *  @param[in] optimize flag to control optimization
 *  @param[in] factor A global multiplier for the (time) matrix cells
 *  @param[in] max_cycles number of cycles to perform during the optimization phase
 *  @param[in] stop_on_all_served Indicator to stop optimization when all orders are served
 *  @param[in] execution_date Value used for not moving orders that are before this date
 *  @param[out] return_tuples C array of contents to be returned to postgres
 *  @param[out] return_count number of tuples returned
 *  @param[out] log_msg special log message pointer
 *  @param[out] notice_msg special message pointer to be returned as NOTICE
 *  @param[out] err_msg special message pointer to be returned as ERROR
 *  @return void
 *
 *
 * @pre The messages: log_msg, notice_msg, err_msg must be empty (=nullptr)
 * @pre The C arrays: customers_arr, vehicles_arr, matrix_cells_arr must not be empty
 * @pre The C array: return_tuples must be empty
 * @pre Only matrix cells (i, i) can be missing and are considered as 0 (time units)
 *
 * @post The C arrays:  customers_arr, vehicles_arr, matrix_cells_arr Do not change
 * @post The C array: return_tuples contains the result for the problem given
 * @post The return_tuples array size is return_count
 * @post err_msg is empty if no throw from the process is catched
 * @post log_msg contains some logging
 * @post notice_msg is empty
 *
 *
 @dot
digraph G {
  node[fontsize=11, nodesep=0.75,ranksep=0.75];

  start  [shape=Mdiamond];
  n1  [label="Verify preconditions",shape=rect];
  n3  [label="Verify matrix cells preconditions",shape=rect];
  n4  [label="Construct problem",shape=cds,color=blue];
  n5  [label="get initial solutions",shape=cds,color=blue];
  n6  [label="solve (optimize)",shape=cds,color=blue];
  n7  [label="Prepare results",shape=rect];
  end  [shape=Mdiamond];
  error [shape=Mdiamond,color=red]
  start -> n1 -> n3 -> n4 -> n5 -> n6 -> n7 -> end;
  n1 -> error [ label="Caller error",color=red];
  n3 -> error [ label="User error",color=red];

}
@enddot

 *
 */
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
    using vrprouting::to_pg_msg;

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

        *return_tuples = nullptr;
        *return_count = 0;

        Identifiers<Id> node_ids;
        Identifiers<Id> order_ids;

        for (size_t i = 0; i < total_customers; ++i) {
            node_ids += customers_arr[i].pick_node_id;
            node_ids += customers_arr[i].deliver_node_id;
            order_ids += customers_arr[i].id;
        }

        bool missing = false;
        for (size_t i = 0; i < total_vehicles; ++i) {
            auto vehicle = vehicles_arr[i];
            node_ids += vehicle.start_node_id;
            node_ids += vehicle.end_node_id;
            for (size_t j = 0; j < vehicle.stops_size; ++j) {
                if (!order_ids.has(vehicle.stops[j])) {
                    if (!missing) err << "Order in 'stops' information missing";
                    missing = true;
                    err << "Missing information of order " << vehicle.stops[j] << "\n";
                }
            }
            if (missing) {
                *err_msg = to_pg_msg(err.str());
                return;
            }
        }

        vrprouting::problem::Matrix cost_matrix(
                matrix_cells_arr, total_cells,
                multipliers_arr, total_multipliers,
                node_ids, static_cast<Multiplier>(factor));
#if 1
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
#endif
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

#if 0
        try {
#endif
            /*
             * get initial solutions
             */
            using Initial_solution = vrprouting::initialsol::tabu::Initial_solution;
            using Solution = vrprouting::problem::Solution;
            auto sol = static_cast<Solution>(Initial_solution(execution_date, optimize, &pd_problem));
            /*
             * Solve (optimize)
             */
            using Optimize = vrprouting::optimizers::tabu::Optimize;
            sol = Optimize(sol, static_cast<size_t>(max_cycles), stop_on_all_served, optimize);
#if 0
        } catch (AssertFailedException &except) {
            log << pd_problem.msg.get_log();
            throw;
        } catch(...) {
            log << "Caught unknown exception!";
            throw;
        }
#endif

        log << pd_problem.msg.get_log();
        pd_problem.msg.clear();
        log << "Finish solve\n";

        /*
         * Prepare results
         */
        auto solution = sol.get_postgres_result();
        log << pd_problem.msg.get_log();
        pd_problem.msg.clear();
        log << "solution size: " << solution.size() << "\n";


        if (!solution.empty()) {
            (*return_tuples) = alloc(solution.size(), (*return_tuples));
            int seq = 0;
            for (const auto &row : solution) {
                (*return_tuples)[seq] = row;
                ++seq;
            }
        }
        (*return_count) = solution.size();

        log << pd_problem.msg.get_log();

        pgassert(*err_msg == nullptr);
        *log_msg = log.str().empty()?
            nullptr :
            to_pg_msg(log.str());
        *notice_msg = notice.str().empty()?
            nullptr :
            to_pg_msg(notice.str());
    } catch (AssertFailedException &except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << except.what() << log.str();
        *err_msg = to_pg_msg(err.str());
    } catch (std::exception& except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << except.what() << log.str();
        *err_msg = to_pg_msg(err.str());
    } catch(...) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << "Caught unknown exception!" << log.str();
        *err_msg = to_pg_msg(err.str());
    }
}
