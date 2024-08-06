/*PGR-GNU*****************************************************************
File: compatibleVehicles_driver.cpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Developer:
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
/** @file */


#include "drivers/compatibleVehicles_driver.h"

#include <cstring>
#include <sstream>
#include <string>
#include <deque>

#include "problem/pickDeliver.hpp"
#include "problem/matrix.hpp"
#include "cpp_common/orders_t.hpp"
#include "c_types/compatibleVehicles_rt.h"
#include "cpp_common/vehicle_t.hpp"

#include "cpp_common/assert.hpp"
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
 *  @param[in] factor A global multiplier for the (time) matrix cells
 *  @param[out] return_tuples C array of contents to be returned to postgres
 *  @param[out] return_count number of tuples returned
 *  @param[out] log_msg special log message pointer
 *  @param[out] notice_msg special message pointer to be returned as NOTICE
 *  @param[out] err_msg special message pointer to be returned as ERROR
 *
 * @pre The messages: log_msg, notice_msg, err_msg must be empty (=nullptr)
 * @pre The C arrays: customers_arr, vehicles_arr, matrix_cells_arr must not be empty
 * @pre The C array: return_tuples must be empty
 * @pre Only matrix cells (i, i) can be missing and are considered as 0 (time units)
 *
 * @post The C arrays:  customers_arr, vehicles_arr, matrix_cells_arr Do not change
 * @post The C array: return_tuples contains the result for the problem given
 * @post The return_tuples array size is return_count
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
  n7  [label="Prepare results",shape=rect];
  end  [shape=Mdiamond];
  error [shape=Mdiamond,color=red]
  start -> n1 -> n3 -> n4 -> n7 -> end;
  n1 -> error [ label="Caller error",color=red];
  n3 -> error [ label="User error",color=red];

}
@enddot

 *
 */
void
do_compatibleVehicles(
        Orders_t customers_arr[],
        size_t total_customers,

        Vehicle_t *vehicles_arr,
        size_t total_vehicles,

        Matrix_cell_t *matrix_cells_arr,
        size_t total_cells,

        Time_multipliers_t *multipliers_arr,
        size_t total_multipliers,

        double factor,

        CompatibleVehicles_rt **return_tuples,
        size_t *return_count,

        char **log_msg,
        char **notice_msg,
        char **err_msg) {
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
        pgassert(total_vehicles);
        pgassert(*return_count == 0);
        pgassert(!(*return_tuples));
        log << "do_compatibleVehicles\n";


        Identifiers<Id> node_ids;

        for (size_t i = 0; i < total_customers; ++i) {
            node_ids += customers_arr[i].pick_node_id;
            node_ids += customers_arr[i].deliver_node_id;
        }

        for (size_t i = 0; i < total_vehicles; ++i) {
            node_ids += vehicles_arr[i].start_node_id;
            node_ids += vehicles_arr[i].end_node_id;
        }

        /*
         * Verify matrix cells preconditions
         */
        vrprouting::problem::Matrix cost_matrix(
                matrix_cells_arr, total_cells,
                multipliers_arr, total_multipliers,
                node_ids, static_cast<Multiplier>(factor));
#if 0
        /*
         * Verify matrix triangle inequality
         */
        if (!cost_matrix.obeys_triangle_inequality()) {
            log << "[Compatible Vehicles] Fixing Matrix that does not obey triangle inequality ";
            log << cost_matrix.fix_triangle_inequality() << " cycles used";

            if (!cost_matrix.obeys_triangle_inequality()) {
                log << "[Compatible Vehicles] Matrix Still does not obey triangle inequality";
            }
        }
#endif
        if (!cost_matrix.has_no_infinity()) {
            err << "An Infinity value was found on the Matrix";
            *err_msg = pgr_msg(err.str());
            return;
        }

        /*
         * Construct problem
         */
        log << "Initialize problem\n";
        vrprouting::problem::PickDeliver pd_problem(
                customers_arr, total_customers,
                vehicles_arr, total_vehicles,
                cost_matrix);

        err << pd_problem.msg.get_error();
        if (!err.str().empty()) {
            log << pd_problem.msg.get_log();
            *log_msg = pgr_msg(log.str());
            *err_msg = pgr_msg(err.str());
            return;
        }
        log << pd_problem.msg.get_log();
        log << "Finish Reading data\n";
        pd_problem.msg.clear();

        /*
         * Prepare results
         */
        auto solution = pd_problem.get_pg_compatibleVehicles();
        log << "solution size: " << solution.size() << "\n";
        log << "solution empty: " << solution.empty() << "\n";

        if (!solution.empty()) {
            log << "solution empty " << "\n";
            (*return_tuples) = pgr_alloc(solution.size(), (*return_tuples));
            int seq = 0;
            for (const auto &row : solution) {
                (*return_tuples)[seq] = row;
                ++seq;
            }
        }
        (*return_count) = solution.size();


        pgassert(*err_msg == nullptr);
        *log_msg = log.str().empty()?
            nullptr :
            pgr_msg(log.str());
        *notice_msg = notice.str().empty()?
            nullptr :
            pgr_msg(notice.str());
    } catch (AssertFailedException &except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = pgr_msg(err.str());
        *log_msg = pgr_msg(log.str());
    } catch (std::exception& except) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = pgr_msg(err.str());
        *log_msg = pgr_msg(log.str());
    } catch(...) {
        if (*return_tuples) free(*return_tuples);
        (*return_count) = 0;
        err << "Caught unknown exception!";
        *err_msg = pgr_msg(err.str());
        *log_msg = pgr_msg(log.str());
    }
}
