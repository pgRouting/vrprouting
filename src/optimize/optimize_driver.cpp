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
/** @file */


#include "drivers/optimize_driver.h"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include "problem/pickDeliver.h"
#include "c_types/pickDeliveryOrders_t.h"
#include "c_types/short_vehicle_rt.h"
#include "c_types/vehicle_t.h"
#include "problem/matrix.h"

#include "cpp_common/pgr_assert.h"
#include "cpp_common/pgr_messages.h"
#include "initialsol/tabu.h"
#include "optimizers/tabu.h"
#include "c_common/pgr_alloc.hpp"
#include "cpp_common/interruption.h"

namespace {

/** @brief Executes an optimization with the input data
 *
 *  @param[in] shipments_arr A C Array of pickup and dropoff shipments
 *  @param[in] total_shipments size of the shipments_arr
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *  @param[in] new_stops stops that override the original stops.
 *  @param[in] time_matrix The unique time matrix
 *  @param[in] max_cycles number of cycles to perform during the optimization phase
 *  @param[in] execution_date Value used for not moving shipments that are before this date
 *
 *  @returns (vehicle id, stops vector) pair which hold the the new stops structure
 */
std::vector<Short_vehicle>
one_processing(
  PickDeliveryOrders_t *shipments_arr, size_t total_shipments,
  Vehicle_t *vehicles_arr, size_t total_vehicles,
  std::vector<Short_vehicle> new_stops,
  const vrprouting::problem::Matrix &time_matrix,
  int max_cycles,
  int64_t execution_date) {
  try {
    /*
     * Construct problem
     */
    vrprouting::problem::PickDeliver pd_problem(
        shipments_arr, total_shipments,
        vehicles_arr, total_vehicles,
        new_stops,
        time_matrix);

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
    auto sol = static_cast<Solution>(Initial_solution(execution_date, true, &pd_problem));

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


/** @brief: extract the times where the shipments opens or closes
 *
 *  @param[in] shipments_arr A C Array of pickup and dropoff shipments
 *  @param[in] total_shipments size of the shipments_arr
 *
 *  @returns processing times
 */
Identifiers<TTimestamp>
processing_times_by_shipment(
    PickDeliveryOrders_t *shipments_arr, size_t total_shipments
    ) {
  Identifiers<TTimestamp> processing_times;
  for (size_t i = 0; i < total_shipments; ++i) {
    processing_times += shipments_arr[i].pick_open_t;
    processing_times += shipments_arr[i].pick_close_t;
    processing_times += shipments_arr[i].deliver_open_t;
    processing_times += shipments_arr[i].deliver_close_t;
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
processing_times_by_vehicle(
    Vehicle_t *vehicles_arr, size_t total_vehicles
    ) {
  Identifiers<TTimestamp> processing_times;
  for (size_t i = 0; i < total_vehicles; ++i) {
    processing_times += vehicles_arr[i].start_open_t;
    processing_times += vehicles_arr[i].start_close_t;
    processing_times += vehicles_arr[i].end_open_t;
    processing_times += vehicles_arr[i].end_close_t;
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
get_initial_stops(
    Vehicle_t *vehicles_arr, size_t total_vehicles
    ) {
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
 *  @param[in] shipments_arr A C Array of pickup and dropoff shipments
 *  @param[in] total_shipments size of the shipments_arr
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *  @param[in] new_stops stops that override the original stops.
 *  @param[in] time_matrix The unique time matrix
 *  @param[in] max_cycles number of cycles to perform during the optimization phase
 *  @param[in] execution_date Value used for not moving shipments that are before this date
 *
 *  @returns (vehicle id, stops vector) pair which hold the the new stops structure
 */
std::vector<Short_vehicle>
subdivide_processing(
  PickDeliveryOrders_t *shipments_arr, size_t total_shipments,
  Vehicle_t *vehicles_arr, size_t total_vehicles,
  const vrprouting::problem::Matrix &time_matrix,
  int max_cycles,
  int64_t execution_date,
  bool subdivide_by_vehicle,
  std::ostringstream &log) {
  try {
    auto the_stops = get_initial_stops(vehicles_arr, total_vehicles);

    auto processing_times = subdivide_by_vehicle?
      processing_times_by_vehicle(vehicles_arr, total_vehicles)
      : processing_times_by_shipment(shipments_arr, total_shipments);

    Identifiers<Id> prev_shipments_in_stops;
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

      /* Get shipments in stops of active vehicles */
      Identifiers<Id> shipments_in_stops;
      for (size_t i = 0; i < vehicles_to_process; ++i) {
        auto v_id = vehicles_arr[i].id;
        auto v_to_modify = std::find_if(
            the_stops.begin(), the_stops.end(), [&]
            (const Short_vehicle& v) -> bool {return v.id == v_id;});

        for (const auto &s : v_to_modify->stops) {
          shipments_in_stops += s;
        }
      }

      /*
       * Nothing to do:
       * - no shipments to process
       * - last optimization had exavtly the same shipments
       */
      if ((shipments_in_stops.size() == 0)
          || (prev_shipments_in_stops == shipments_in_stops)) continue;
      log << "\nOptimizing at time: " << t;

      prev_shipments_in_stops = shipments_in_stops;

      auto shipments_to_process = static_cast<size_t>(std::distance(shipments_arr,
        std::partition(shipments_arr, shipments_arr + total_shipments,
            [&](const PickDeliveryOrders_t& s){return shipments_in_stops.has(s.id);})));

      pgassert(shipments_to_process > 0);
      pgassert(shipments_in_stops.size() == static_cast<size_t>(shipments_to_process));

      auto new_stops = one_processing(
          shipments_arr, shipments_to_process,
          vehicles_arr, vehicles_to_process, the_stops,
          time_matrix,
          max_cycles, execution_date);

      update_stops(the_stops, new_stops);
    }

    return the_stops;
  } catch(...) {
    throw;
  }
}


}  // namespace

/**
 *
 *  @param[in] shipments_arr A C Array of pickup and dropoff shipments
 *  @param[in] total_shipments size of the shipments_arr
 *  @param[in] vehicles_arr A C Array of vehicles
 *  @param[in] total_vehicles size of the vehicles_arr
 *  @param[in] matrix_cells_arr A C Array of the (time) matrix cells
 *  @param[in] total_cells size of the matrix_cells_arr
 *  @param[in] multipliers_arr A C Array of the multipliers
 *  @param[in] total_multipliers size of the multipliers_arr
 *  @param[in] optimize flag to control optimization
 *  @param[in] factor A global multiplier for the (time) matrix cells
 *  @param[in] max_cycles number of cycles to perform during the optimization phase
 *  @param[in] stop_on_all_served Indicator to stop optimization when all shipments are served
 *  @param[in] execution_date Value used for not moving shipments that are before this date
 *  @param[out] return_tuples C array of contents to be returned to postgres
 *  @param[out] return_count number of tuples returned
 *  @param[out] log_msg special log message pointer
 *  @param[out] notice_msg special message pointer to be returned as NOTICE
 *  @param[out] err_msg special message pointer to be returned as ERROR
 *  @return void
 *
 *
 * @pre The messages: log_msg, notice_msg, err_msg must be empty (=nullptr)
 * @pre The C arrays: shipments_arr, vehicles_arr, matrix_cells_arr must not be empty
 * @pre The C array: return_tuples must be empty
 * @pre Only matrix cells (i, i) can be missing and are considered as 0 (time units)
 *
 * @post The C arrays:  shipments_arr, vehicles_arr, matrix_cells_arr Do not change
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
do_optimize(
    PickDeliveryOrders_t *shipments_arr, size_t total_shipments,
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
    pgassert(total_shipments);
    pgassert(total_vehicles);
    pgassert(total_cells);
    pgassert(*return_count == 0);
    pgassert(!(*return_tuples));

    *return_tuples = nullptr;
    *return_count = 0;

    Identifiers<Id> node_ids;
    Identifiers<Id> shipments_in_stops;

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
     * Remove shipments not involved in optimization
     * 1. get the shipments on the stops of the vehicles
     *   - getting the node_ids in the same cycle
     * 2. Remove duplicates
     * 2. Remove shipments not on the stops
     */
    for (size_t i = 0; i < total_vehicles; ++i) {
      node_ids += vehicles_arr[i].start_node_id;
      node_ids += vehicles_arr[i].end_node_id;
      for (size_t j = 0; j < vehicles_arr[i].stops_size; ++j) {
        shipments_in_stops += vehicles_arr[i].stops[j];
      }
    }

    std::sort(shipments_arr, shipments_arr + total_shipments,
        [](const PickDeliveryOrders_t& lhs, const PickDeliveryOrders_t& rhs){return lhs.id < rhs.id;});

    total_shipments = static_cast<size_t>(std::distance(shipments_arr,
      std::unique(shipments_arr, shipments_arr + total_shipments,
          [&](const PickDeliveryOrders_t& lhs, const PickDeliveryOrders_t& rhs){return lhs.id == rhs.id;})));

    total_shipments = static_cast<size_t>(std::distance(shipments_arr,
        std::remove_if(shipments_arr, shipments_arr + total_shipments,
        [&](const PickDeliveryOrders_t& s){return !shipments_in_stops.has(s.id);})));

    /*
     * Verify shipments complete data
     */
    if (shipments_in_stops.size() != total_shipments) {
      for (size_t i = 0; i < total_shipments; ++i) {
        shipments_in_stops -= shipments_arr[i].id;
      }
      std::ostringstream hint;
      err << "Missing shipments for processing ";
      hint << "Shipments missing: " << shipments_in_stops << log.str();
      *log_msg = pgr_msg(hint.str());
      *err_msg = pgr_msg(err.str());
      return;
    }

    /*
     * Finish getting the node ids involved on the process
     */
    for (size_t i = 0; i < total_shipments; ++i) {
      node_ids += shipments_arr[i].pick_node_id;
      node_ids += shipments_arr[i].deliver_node_id;
    }

    /*
     * Dealing with time matrix:
     * - Create the unique time matrix to be used for all optimizations
     * - Verify matrix triangle inequality
     * - Verify matrix cells preconditions
     */
    vrprouting::problem::Matrix time_matrix(
        matrix_cells_arr, total_cells,
        multipliers_arr, total_multipliers,
        node_ids, static_cast<Multiplier>(factor));

    if (check_triangle_inequality && !time_matrix.obeys_triangle_inequality()) {
      log << "\nFixing Matrix that does not obey triangle inequality "
        << time_matrix.fix_triangle_inequality() << " cycles used";

      if (!time_matrix.obeys_triangle_inequality()) {
        log << "\nMatrix Still does not obey triangle inequality ";
      }
    }

    if (!time_matrix.has_no_infinity()) {
      err << "\nAn Infinity value was found on the Matrix";
      *err_msg = pgr_msg(err.str());
      *log_msg = pgr_msg(log.str());
      return;
    }

    /*
     * get the solution
     */
    auto solution = subdivide?
      subdivide_processing(
          shipments_arr, total_shipments,
          vehicles_arr, total_vehicles,
          time_matrix,
          max_cycles, execution_date,
          subdivide_by_vehicle,
          log) :
      one_processing(
          shipments_arr, total_shipments,
          vehicles_arr, total_vehicles, {},
          time_matrix,
          max_cycles, execution_date);

    /*
     * Prepare results
     */
    if (!solution.empty()) {
      (*return_tuples) = pgr_alloc(total_shipments * 2, (*return_tuples));

      size_t seq = 0;
      for (const auto &row : solution) {
        for (const auto &o_id : row.stops) {
          (*return_tuples)[seq].vehicle_id = row.id;
          (*return_tuples)[seq].order_id = o_id;
          ++seq;
        }
      }
    }

    (*return_count) = total_shipments * 2;

    pgassert(*err_msg == nullptr);
    *log_msg = log.str().empty()?
      nullptr :
      pgr_msg(log.str());
    *notice_msg = notice.str().empty()?
      nullptr :
      pgr_msg(notice.str());
  } catch (AssertFailedException &except) {
    err << except.what() << log.str();
    *err_msg = pgr_msg(err.str());
  } catch (std::exception& except) {
    err << except.what() << log.str();
    *err_msg = pgr_msg(err.str());
  } catch (const std::pair<std::string, std::string>& ex) {
    err << ex.first;
    log.str("");
    log.clear();
    log << ex.second;
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch(...) {
    err << "Caught unknown exception!" << log.str();
    *err_msg = pgr_msg(err.str());
  }
}
