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


#include "drivers/pgr_pickDeliver/pickDeliverEuclidean_driver.h"

#include <sstream>
#include <utility>
#include <string>
#include <deque>
#include <vector>
#include <map>

#include "c_types/pickDeliveryOrders_t.h"
#include "c_types/solution_rt.h"
#include "c_common/pgr_alloc.hpp"
#include "cpp_common/pgr_assert.h"

#include "problem/matrix.h"
#include "initialsol/simple.h"
#include "optimizers/simple.h"
#include "problem/pickDeliver.h"

namespace {
vrprouting::problem::Solution
get_initial_solution(vrprouting::problem::PickDeliver* problem_ptr, int m_initial_id) {
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
    PickDeliveryOrders_t *customers_arr,
    size_t total_customers,
    std::string *err_string,
    std::string *hint_string) {
  /**
   * - demand > 0 (the type is unsigned so no need to check for negative values, only for it to be non 0
   * - pick_service_t >=0
   * - drop_service_t >=0
   * - p_open <= p_close
   * - d_open <= d_close
   */
  for (size_t i = 0; i < total_customers; ++i) {
    if (customers_arr[i].demand == 0) {
      *err_string = "Unexpected zero value found on column 'demand' of shipments";
      *hint_string = "Check shipment id #:" + std::to_string(customers_arr[i].id);
      return false;
    }

    if (customers_arr[i].pick_service_t < 0) {
      *err_string = "Unexpected negative value found on column 'p_service_t' of shipments";
      *hint_string = "Check shipment id #:" + std::to_string(customers_arr[i].id);
      return false;
    }

    if (customers_arr[i].deliver_service_t < 0) {
      *err_string = "Unexpected negative value found on column 'd_service_t' of shipments";
      *hint_string = "Check shipment id #:" + std::to_string(customers_arr[i].id);
      return false;
    }

    if (customers_arr[i].pick_open_t > customers_arr[i].pick_close_t) {
      *err_string = "Unexpected pickup time windows found on shipments";
      *hint_string = "Check shipment id #:" + std::to_string(customers_arr[i].id);
      return false;
    }

    if (customers_arr[i].deliver_open_t > customers_arr[i].deliver_close_t) {
      *err_string = "Unexpected delivery time windows found on shipments";
      *hint_string = "Check shipment id #:" + std::to_string(customers_arr[i].id);
      return false;
    }
  }
  return true;
}

}  // namespace

void
do_pgr_pickDeliverEuclidean(
    PickDeliveryOrders_t *customers_arr,
    size_t total_customers,

    Vehicle_t *vehicles_arr,
    size_t total_vehicles,

    double factor,
    int max_cycles,
    int initial_solution_id,

    Solution_rt **return_tuples,
    size_t *return_count,

    char **log_msg,
    char **notice_msg,
    char **err_msg) {
  std::ostringstream log;
  std::ostringstream notice;
  std::ostringstream err;
  try {
    *return_tuples = nullptr;
    *return_count = 0;
    std::string err_string;
    std::string hint_string;
    if (!are_shipments_ok(customers_arr, total_customers, &err_string, &hint_string)) {
      *err_msg = pgr_msg(err_string.c_str());
      *log_msg = pgr_msg(hint_string.c_str());
      return;
    }

    /*
     * transform to C++ containers
     */
    std::vector<PickDeliveryOrders_t> orders(
        customers_arr, customers_arr + total_customers);
    std::vector<Vehicle_t> vehicles(
        vehicles_arr, vehicles_arr + total_vehicles);

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

    for (size_t i = 0; i < total_customers; ++i) {
      customers_arr[i].pick_node_id =
        matrix_data[std::pair<Coordinate, Coordinate>(customers_arr[i].pick_x, customers_arr[i].pick_y)];

      customers_arr[i].deliver_node_id =
        matrix_data[std::pair<Coordinate, Coordinate>(customers_arr[i].deliver_x, customers_arr[i].deliver_y)];
    }
    for (auto &v : vehicles) {
      v.start_node_id = matrix_data[std::pair<Coordinate, Coordinate>(v.start_x, v.start_y)];
      v.end_node_id = matrix_data[std::pair<Coordinate, Coordinate>(v.end_x, v.end_y)];
    }

    vrprouting::problem::Matrix cost_matrix(matrix_data, static_cast<Multiplier>(factor));

    log << "Initialize problem\n";
    vrprouting::problem::PickDeliver pd_problem(
        customers_arr, total_customers,
        vehicles_arr, total_vehicles,
        cost_matrix);

    err << pd_problem.msg.get_error();
    if (!err.str().empty()) {
      log.str("");
      log.clear();
      log << pd_problem.msg.get_error();
      log << pd_problem.msg.get_log();
      *log_msg = pgr_msg(log.str().c_str());
      *err_msg = pgr_msg(err.str().c_str());
      return;
    }
    log << pd_problem.msg.get_log();
    log << "Finish Reading data\n";

#if 0
    try {
#endif
      auto sol = get_initial_solution(&pd_problem, initial_solution_id);
      using Optimize = vrprouting::optimizers::simple::Optimize;
      using Initials_code = vrprouting::initialsol::simple::Initials_code;
      sol = Optimize(sol, static_cast<size_t>(max_cycles), (Initials_code)initial_solution_id);
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
    log << "Finish solve\n";

    auto solution = sol.get_postgres_result();
    log << pd_problem.msg.get_log();
    log << "solution size: " << solution.size() << "\n";


    if (!solution.empty()) {
      (*return_tuples) = pgr_alloc(solution.size(), (*return_tuples));
      int seq = 0;
      for (const auto &row : solution) {
        (*return_tuples)[seq] = row;
        ++seq;
      }
    }
    (*return_count) = solution.size();

    log << pd_problem.msg.get_log();

    pgassert(*err_msg == NULL);
    *log_msg = log.str().empty()?
      nullptr :
      pgr_msg(log.str().c_str());
    *notice_msg = notice.str().empty()?
      nullptr :
      pgr_msg(notice.str().c_str());
  } catch (AssertFailedException &except) {
    if (*return_tuples) free(*return_tuples);
    (*return_count) = 0;
    err << except.what();
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch (std::exception& except) {
    if (*return_tuples) free(*return_tuples);
    (*return_count) = 0;
    err << except.what();
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch (const std::pair<std::string, std::string>& ex) {
    (*return_count) = 0;
    err << ex.first;
    log.str("");
    log.clear();
    log << ex.second;
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  } catch(...) {
    if (*return_tuples) free(*return_tuples);
    (*return_count) = 0;
    err << "Caught unknown exception!";
    *err_msg = pgr_msg(err.str().c_str());
    *log_msg = pgr_msg(log.str().c_str());
  }
}
