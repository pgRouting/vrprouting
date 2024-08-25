/*PGR-GNU*****************************************************************
File: pgr_pickDeliver_driver.cpp

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

#include "drivers/pgr_pickDeliver_driver.h"

#include <sstream>
#include <string>
#include <utility>

#include "c_types/solution_rt.h"

#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"
#include "cpp_common/pgdata_getters.hpp"
#include "cpp_common/orders_t.hpp"
#include "cpp_common/vehicle_t.hpp"
#include "initialsol/initials_code.hpp"
#include "initialsol/simple.hpp"
#include "optimizers/simple.hpp"
#include "problem/matrix.hpp"
#include "problem/pickDeliver.hpp"
#include "problem/solution.hpp"

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

}  // namespace

void
vrp_do_pgr_pickDeliver(
        char *orders_sql,
        char *vehicles_sql,
        char *matrix_sql,

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
        using Matrix = vrprouting::problem::Matrix;
        using vrprouting::pgget::pickdeliver::get_matrix;
        using vrprouting::pgget::pickdeliver::get_orders;
        using vrprouting::pgget::pickdeliver::get_vehicles;

        /*
         * verify preconditions
         */
        pgassert(!(*log_msg));
        pgassert(!(*notice_msg));
        pgassert(!(*err_msg));
        pgassert(*return_count == 0);
        pgassert(!(*return_tuples));

        if (initial_solution_id < 0 || initial_solution_id > 7) {
            *err_msg = to_pg_msg("Illegal value in parameter: initial_sol");
            *log_msg = to_pg_msg("Expected value: 0 <= initial_sol < 7");
            return;
        }

        if (max_cycles < 0) {
            *err_msg = to_pg_msg("Illegal value in parameter: max_cycles");
            *log_msg = to_pg_msg("Expected value: max_cycles >= 0");
            return;
        }

        if (factor <= 0) {
            *err_msg = to_pg_msg("Illegal value in parameter: factor");
            *log_msg = to_pg_msg("Expected value: factor > 0");
            return;
        }

        /* Data input starts */

        bool use_timestamps = false;
        bool is_euclidean = false;
        bool with_stops = false;

        hint = orders_sql;
        auto orders = get_orders(std::string(orders_sql), is_euclidean, use_timestamps);
        if (orders.size() == 0) {
            *notice_msg = to_pg_msg("Insufficient data found on 'orders' inner query");
            *log_msg = hint? to_pg_msg(hint) : nullptr;
            return;
        }

        hint = vehicles_sql;
        auto vehicles = get_vehicles(std::string(vehicles_sql), is_euclidean, use_timestamps, with_stops);
        if (vehicles.size() == 0) {
            *notice_msg = to_pg_msg("Insufficient data found on 'vehicles' inner query");
            *log_msg = hint? to_pg_msg(hint) : nullptr;
            return;
        }

        hint = matrix_sql;
        auto costs = get_matrix(std::string(matrix_sql), use_timestamps);

        if (costs.size() == 0) {
            *notice_msg = to_pg_msg("Insufficient data found on 'matrix' inner query");
            *log_msg = hint? to_pg_msg(hint) : nullptr;
            return;
        }
        hint = nullptr;

        /* Data input ends */

        /* Processing starts */

        Identifiers<Id> node_ids;
        Identifiers<Id> order_ids;

        for (const auto &o : orders) {
            node_ids += o.pick_node_id;
            node_ids += o.deliver_node_id;
            order_ids += o.id;
        }

        for (const auto &v : vehicles) {
            node_ids += v.start_node_id;
            node_ids += v.end_node_id;
        }

        /*
         * Prepare matrix
         */
        Matrix matrix(costs, node_ids, static_cast<Multiplier>(factor));

        /*
         * Verify matrix cells preconditions
         */
        if (!matrix.has_no_infinity()) {
            *err_msg = to_pg_msg("An Infinity value was found on the Matrix");
            *log_msg = to_pg_msg(log.str());
            return;
        }

        /*
         * Construct problem
         */
        vrprouting::problem::PickDeliver pd_problem(orders, vehicles, matrix);

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
        using Initials_code = vrprouting::initialsol::simple::Initials_code;
        auto sol = get_initial_solution(pd_problem, initial_solution_id);

        /*
         * Solve (optimize)
         */
        using Optimize = vrprouting::optimizers::simple::Optimize;
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
            size_t seq = 0;
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
