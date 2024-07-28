/*PGR-GNU*****************************************************************

FILE: solution.hpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

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

#ifndef INCLUDE_PROBLEM_SOLUTION_HPP_
#define INCLUDE_PROBLEM_SOLUTION_HPP_
#pragma once

#include <deque>
#include <vector>
#include <string>
#include <ostream>
#include <numeric>
#include <algorithm>
#include <tuple>
#include <iomanip>

#include "problem/vehicle_pickDeliver.hpp"
#include "problem/fleet.hpp"
#include "c_types/short_vehicle.h"

typedef struct Solution_rt Solution_rt;

namespace vrprouting {
namespace problem {

class Solution {
 public:
    /** @brief constructor */
    Solution() = delete;

    /** @brief constructor */
    template <typename P>
      explicit Solution(P* p_problem) :
        m_orders(p_problem->orders()),
        m_trucks(p_problem->vehicles()),
        m_msg(p_problem->msg) { }


    /** @brief copy constructor */
    Solution(const Solution &sol) = default;

    /** @brief copy assignment */
    Solution& operator = (const Solution& sol) = default;

    /** @brief get solution like postgres needs it */
    std::vector<Short_vehicle> get_stops() const;

    /** @brief get solution like postgres needs it */
    std::vector<Solution_rt> get_postgres_result() const;

    /** @brief printing function */
    friend std::ostream& operator<< (std::ostream &log, const Solution &solution) {
      for (const auto& vehicle : solution.m_fleet) log << vehicle;
      log << "\n SOLUTION:\n\n " << solution.tau();
      return log;
    }

    /** @brief writing the solution in compact form into a string */
    std::string tau(const std::string &title = "Tau") const;

    /** @brief is the solution feasible? */
    bool is_feasible() const;

    /** @brief Total duration of the solution*/
    TInterval duration() const;

    /** @brief Total waiting time of the solution*/
    TInterval wait_time() const;

    /** @brief Total travel time of the solution*/
    TInterval total_travel_time() const;

    /** @brief Total service time of the solution*/
    TInterval total_service_time() const;

    /** @brief Total number of time windows constraint violations of the solution*/
    int twvTot() const;

    /** @brief Total number of capacity constraint violations of the solution*/
    int cvTot() const;

    /** Get the current fleet solution */
    const std::deque<Vehicle_pickDeliver>& fleet() const {return m_fleet;}

    /** @brief Get the value of the objective function */
    double objective() const {return static_cast<double>(total_travel_time());}

    /** @brief Get all statistics in one cycle */
    std::tuple<int, int, size_t, TInterval, TInterval, TInterval> cost() const;

    std::string cost_str() const;

    bool operator<(const Solution&) const;

    Pgr_messages& msg() {return m_msg;}
    const Orders& orders() const {return m_orders;}
    Fleet& vehicles() {return m_trucks;}

 protected:
    /** The current solution */
    std::deque<Vehicle_pickDeliver> m_fleet;

    /** the problem info */
    Orders m_orders;
    Fleet  m_trucks;
    Pgr_messages m_msg;
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_SOLUTION_HPP_
