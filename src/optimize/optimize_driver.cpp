/*PGR-GNU*****************************************************************
File: optimize_driver.cpp

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

#include "drivers/optimize_driver.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "c_types/short_vehicle_rt.h"

#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"

#include "cpp_common/interruption.hpp"
#include "cpp_common/messages.hpp"

#include "cpp_common/orders_t.hpp"
#include "cpp_common/short_vehicle.hpp"
#include "cpp_common/vehicle_t.hpp"

#include "initialsol/tabu.hpp"
#include "optimizers/tabu.hpp"
#include "problem/matrix.hpp"
#include "problem/pickDeliver.hpp"

namespace {

using Short_vehicle = vrprouting::Short_vehicle;

/** @brief Executes an optimization with the input data
 *
 *  @param[in] orders_arr A C Array of pickup and dropoff orders
 *  @param[in] total_orders size of the orders_arr
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *
 *  @param[in] new_stops stops that override the original stops.
 *  @param[in] matrix The unique time matrix
 *  @param[in] max_cycles number of cycles to perform during the optimization phase
 *  @param[in] execution_date Value used for not moving orders that are before this date
 *
 *  @returns (vehicle id, stops vector) pair which hold the the new stops structure
 */
std::vector<Short_vehicle>
one_processing(
        Orders_t *orders_arr, size_t total_orders,
        Vehicle_t *vehicles_arr, size_t total_vehicles,
        std::vector<Short_vehicle> new_stops,
        const vrprouting::problem::Matrix &matrix,
        int max_cycles,
        int64_t execution_date) {
    try {
        /*
         * Construct problem
         */
        vrprouting::problem::PickDeliver pd_problem(
                orders_arr, total_orders,
                vehicles_arr, total_vehicles,
                new_stops,
                matrix);

        /*
         * Unknown problem when createing the pick Deliver problem
         */
        if (!pd_problem.msg.get_error().empty()) {
            throw std::make_pair(pd_problem.msg.get_error(), pd_problem.msg.get_log());
        }

        /*
         * get initial solution
         */
        using Initial_solution = vrprouting::initialsol::tabu::Initial_solution;
        using Solution = vrprouting::problem::Solution;
        auto sol = static_cast<Solution>(Initial_solution(execution_date, true, pd_problem));

        /*
         * Optimize the initial solution:
         * - false: do not stop on all served
         * - true:  optimize
         */
        using Optimize = vrprouting::optimizers::tabu::Optimize;
        sol = Optimize(sol, static_cast<size_t>(max_cycles), false, true);

        return sol.get_stops();
    } catch(...) {
        throw;
    }
}


/** @brief: extract the times where the orders opens or closes
 *
 *  @param[in] orders_arr A C Array of pickup and dropoff orders
 *  @param[in] total_orders size of the orders_arr
 *
 *  @returns processing times
 */
Identifiers<TTimestamp>
processing_times_by_order(Orders_t *orders_arr, size_t total_orders) {
    Identifiers<TTimestamp> processing_times;
    for (size_t i = 0; i < total_orders; ++i) {
        auto o = orders_arr[i];
        processing_times += o.pick_open_t;
        processing_times += o.pick_close_t;
        processing_times += o.deliver_open_t;
        processing_times += o.deliver_close_t;
    }
    return processing_times;
}

/** @brief: extract the times where the vehicle opens or closes
 *
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *
 *  @returns processing times
 */
Identifiers<TTimestamp>
processing_times_by_vehicle(Vehicle_t *vehicles_arr, size_t total_vehicles) {
    Identifiers<TTimestamp> processing_times;
    for (size_t i = 0; i < total_vehicles; ++i) {
        auto v =  vehicles_arr[i];
        processing_times += v.start_open_t;
        processing_times += v.start_close_t;
        processing_times += v.end_open_t;
        processing_times += v.end_close_t;
    }
    return processing_times;
}

/** @brief get the original stops
 *
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *
 *  @returns (vehicle id, stops vector) pair which hold the stops structure
 */
std::vector<Short_vehicle>
get_initial_stops(Vehicle_t *vehicles_arr, size_t total_vehicles) {
    std::vector<Short_vehicle> the_stops;
    for (size_t i = 0; i < total_vehicles; ++i) {
        std::vector<Id> stops;
        for (size_t j = 0; j < vehicles_arr[i].stops_size; ++j) {
            stops.push_back(vehicles_arr[i].stops[j]);
        }
        the_stops.push_back({vehicles_arr[i].id, stops});
    }
    std::sort(the_stops.begin(), the_stops.end(), []
            (const Short_vehicle &lhs, const Short_vehicle &rhs) {return lhs.id < rhs.id;});
    return the_stops;
}

/** @brief Update the vehicle stops to the new values
 *
 *  @param[in,out] the_stops set of stops that are to be updated
 *  @param[in] new_values subset of stops that are to be used for the update
 */
void
update_stops(std::vector<Short_vehicle>& the_stops,  // NOLINT [runtime/references]
        const std::vector<Short_vehicle> new_values) {
    for (const auto &v : new_values) {
        auto v_id = v.id;
        auto v_to_modify = std::find_if(
                the_stops.begin(), the_stops.end(), [v_id]
                (const Short_vehicle& v) -> bool {return v.id == v_id;});
        pgassert(v_to_modify != the_stops.end());
        v_to_modify->stops = v.stops;
    }
}

/** @brief Executes an optimization by subdivision of data
 *
 *  @param[in] orders_arr A C Array of pickup and dropoff orders
 *  @param[in] total_orders size of the orders_arr
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *  @param[in] new_stops stops that override the original stops.
 *  @param[in] matrix The unique time matrix
 *  @param[in] max_cycles number of cycles to perform during the optimization phase
 *  @param[in] execution_date Value used for not moving orders that are before this date
 *
 *  @returns (vehicle id, stops vector) pair which hold the the new stops structure
 */
std::vector<Short_vehicle>
subdivide_processing(
        Orders_t *orders_arr, size_t total_orders,
        Vehicle_t *vehicles_arr, size_t total_vehicles,
        const vrprouting::problem::Matrix &matrix,
        int max_cycles,
        int64_t execution_date,
        bool subdivide_by_vehicle,
        std::ostringstream &log) {
    try {
        auto the_stops = get_initial_stops(vehicles_arr, total_vehicles);

        auto processing_times = subdivide_by_vehicle?
            processing_times_by_vehicle(vehicles_arr, total_vehicles)
            : processing_times_by_order(orders_arr, total_orders);

        Identifiers<Id> prev_orders_in_stops;
        for (const auto &t : processing_times) {
            CHECK_FOR_INTERRUPTS();
            /*
             * Get active vehicles at time t
             */
            auto vehicles_to_process = static_cast<size_t>(std::distance(vehicles_arr,
                        std::partition(
                            vehicles_arr, vehicles_arr + total_vehicles,
                            [&](const Vehicle_t& v)
                            {return v.start_open_t <= t && t <= v.end_close_t;})));

            /* Get orders in stops of active vehicles */
            Identifiers<Id> orders_in_stops;
            for (size_t i = 0; i < vehicles_to_process; ++i) {
                auto v_id = vehicles_arr[i].id;
                auto v_to_modify = std::find_if(
                        the_stops.begin(), the_stops.end(), [&]
                        (const Short_vehicle& v) -> bool {return v.id == v_id;});

                for (const auto &s : v_to_modify->stops) {
                    orders_in_stops += s;
                }
            }

            /*
             * Nothing to do:
             * - no orders to process
             * - last optimization had exactly the same orders
             */
            if ((orders_in_stops.size() == 0)
                    || (prev_orders_in_stops == orders_in_stops)) continue;
            log << "\nOptimizing at time: " << t;

            prev_orders_in_stops = orders_in_stops;

            auto orders_to_process = static_cast<size_t>(std::distance(orders_arr,
                        std::partition(orders_arr, orders_arr + total_orders,
                            [&](const Orders_t& s){return orders_in_stops.has(s.id);})));

            pgassert(orders_to_process > 0);
            pgassert(orders_in_stops.size() == static_cast<size_t>(orders_to_process));

            auto new_stops = one_processing(
                    orders_arr, orders_to_process,
                    vehicles_arr, vehicles_to_process, the_stops,
                    matrix,
                    max_cycles, execution_date);

            update_stops(the_stops, new_stops);
        }

        return the_stops;
    } catch(...) {
        throw;
    }
}


}  // namespace

void
vrp_do_optimize(
        Orders_t *orders_arr, size_t total_orders,
        Vehicle_t *vehicles_arr, size_t total_vehicles,
        Matrix_cell_t *matrix_cells_arr, size_t total_cells,
        Time_multipliers_t *multipliers_arr, size_t total_multipliers,


        double factor,
        int max_cycles,
        int64_t execution_date,

        bool check_triangle_inequality,
        bool subdivide,
        bool subdivide_by_vehicle,

        Short_vehicle_rt **return_tuples,
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
        pgassert(total_orders);
        pgassert(total_vehicles);
        pgassert(total_cells);
        pgassert(*return_count == 0);
        pgassert(!(*return_tuples));

        Identifiers<Id> node_ids;
        Identifiers<Id> orders_in_stops;

        /*
         * Remove vehicles not going to be optimized and sort remaining vehicles
         * 1. sort by id
         * 2. remove duplicates
         *   - data comes from query that could possibly give a duplicate
         * 3. remove vehicles that closes(end) before the execution time
         */
        std::sort(vehicles_arr, vehicles_arr + total_vehicles,
                [](const Vehicle_t& lhs, const Vehicle_t& rhs){return lhs.id < rhs.id;});

        total_vehicles = static_cast<size_t>(std::distance(vehicles_arr,
                    std::unique(vehicles_arr, vehicles_arr + total_vehicles,
                        [&](const Vehicle_t& lhs, const Vehicle_t& rhs){return lhs.id == rhs.id;})));

        total_vehicles = static_cast<size_t>(std::distance(vehicles_arr,
                    std::remove_if(vehicles_arr, vehicles_arr + total_vehicles,
                        [&](const Vehicle_t& v){return v.end_close_t < execution_date;})));

        /*
         * nodes involved & orders involved
         */
        for (size_t i = 0; i < total_vehicles; ++i) {
            node_ids += vehicles_arr[i].start_node_id;
            node_ids += vehicles_arr[i].end_node_id;
            for (size_t j = 0; j < vehicles_arr[i].stops_size; ++j) {
                orders_in_stops += vehicles_arr[i].stops[j];
            }
        }

        /*
         * Remove orders not involved in optimization
         * 1. get the orders on the stops of the vehicles
         *   - getting the node_ids in the same cycle
         * 2. Remove duplicates
         * 2. Remove orders not on the stops
         */
        std::sort(orders_arr, orders_arr + total_orders,
                [](const Orders_t& lhs, const Orders_t& rhs){return lhs.id < rhs.id;});

        total_orders = static_cast<size_t>(std::distance(orders_arr,
                    std::unique(orders_arr, orders_arr + total_orders,
                        [&](const Orders_t& lhs, const Orders_t& rhs){return lhs.id == rhs.id;})));

        total_orders = static_cast<size_t>(std::distance(orders_arr,
                    std::remove_if(orders_arr, orders_arr + total_orders,
                        [&](const Orders_t& s){return !orders_in_stops.has(s.id);})));

        /*
         * Verify orders complete data
         */
        if (orders_in_stops.size() != total_orders) {
            for (size_t i = 0; i < total_orders; ++i) {
                orders_in_stops -= orders_arr[i].id;
            }
            err << "Missing orders for processing ";
            log << "Shipments missing: " << orders_in_stops << log.str();
            *log_msg = to_pg_msg(log.str());
            *err_msg = to_pg_msg(err.str());
            return;
        }

        /*
         * nodes involved
         */
        for (size_t i = 0; i < total_orders; ++i) {
            node_ids += orders_arr[i].pick_node_id;
            node_ids += orders_arr[i].deliver_node_id;
        }

        /*
         * Prepare matrix
         */
        vrprouting::problem::Matrix matrix(
                matrix_cells_arr, total_cells,
                multipliers_arr, total_multipliers,
                node_ids, static_cast<Multiplier>(factor));

        /*
         * Verify matrix triangle inequality
         */
        if (check_triangle_inequality && !matrix.obeys_triangle_inequality()) {
            log << "\nFixing Matrix that does not obey triangle inequality.\t"
                << matrix.fix_triangle_inequality() << " cycles used";

            if (!matrix.obeys_triangle_inequality()) {
                log << "\nMatrix Still does not obey triangle inequality.";
            }
        }

        /*
         * Verify matrix cells preconditions
         */
        if (!matrix.has_no_infinity()) {
            *err_msg = to_pg_msg("An Infinity value was found on the Matrix");
            *log_msg = to_pg_msg(log.str());
            return;
        }

        /*
         * get the solution
         */
        auto solution = subdivide?
            subdivide_processing(
                    orders_arr, total_orders,
                    vehicles_arr, total_vehicles,
                    matrix,
                    max_cycles, execution_date,
                    subdivide_by_vehicle,
                    log) :
            one_processing(
                    orders_arr, total_orders,
                    vehicles_arr, total_vehicles, {},
                    matrix,
                    max_cycles, execution_date);

        /*
         * Prepare results
         */
        if (!solution.empty()) {
            (*return_tuples) = alloc(total_orders * 2, (*return_tuples));

            size_t seq = 0;
            for (const auto &row : solution) {
                for (const auto &o_id : row.stops) {
                    (*return_tuples)[seq].vehicle_id = row.id;
                    (*return_tuples)[seq].order_id = o_id;
                    ++seq;
                }
            }
        }

        (*return_count) = total_orders * 2;

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
