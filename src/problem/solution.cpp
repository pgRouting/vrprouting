/*PGR-GNU*****************************************************************

FILE: solution.cpp

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
#include "problem/solution.hpp"

#include <deque>
#include <vector>
#include <string>
#include <ostream>
#include <numeric>
#include <algorithm>
#include <tuple>
#include <iomanip>
#include "problem/pickDeliver.hpp"
#include "cpp_common/short_vehicle.hpp"

typedef struct Solution_rt Solution_rt;

namespace vrprouting {
namespace problem {

/**
 * @returns container for postgres
 */
std::vector<Short_vehicle>
Solution::get_stops() const {
  std::vector<Short_vehicle> result;
  for (auto v : m_fleet) {
    result.push_back(Short_vehicle{v.id(), v.get_stops()});
  }
  return result;
}

/**
 * @returns container for postgres
 */
std::vector<Solution_rt>
Solution::get_postgres_result() const {
  std::vector<Solution_rt> result;

  /* postgres numbering starts with 1 */
  int i(1);

  for (auto v : m_fleet) {
    v.evaluate(0);
    auto data = v.get_postgres_result(i);
    /*
     * Results adjusted for the depot and the first stop
     * So the vehicle waits on the depot not on the first stop
     */
    data[1].arrivalTime = data[1].operationTime;
    data[0].waitDuration = data[1].waitDuration;
    data[1].waitDuration = 0;
    data[0].departureTime = data[0].arrivalTime + data[0].waitDuration;


    result.insert(result.end(), data.begin(), data.end());
    ++i;
  }

  Solution_rt aggregates = {
    /*
     * Vehicle id = -2 indicates its an aggregate row
     *
     * (twv, cv, fleet, wait, duration)
     */
    -2,  // summary row on vehicle_number
    twvTot(),  // on vehicle_id
    cvTot(),   // on vehicle_seq
    -1,  // on order_id
    -1,  // on stop_id
    -2,  // on stop_type (gets increased later by one so it gets -1)
    -1,  // not accounting total loads
    static_cast<int64_t>(total_travel_time()),
    -1,  // not accounting arrival_travel_time
    static_cast<int64_t>(wait_time()),
    -1,  // not accounting operation time
    static_cast<int64_t>(total_service_time()),
    static_cast<int64_t>(duration()),
    cvTot(),
    twvTot()
  };
  result.push_back(aggregates);

  return result;
}

/**
 * @param [in] title Title to Use for the tau
 * @returns The solution in one compacted string
 */
std::string
Solution::tau(const std::string &title) const {
  std::string str {"\n" + title + ": " + '\n'};
  for (const auto& v : m_fleet) str += ("\n" + v.tau());
  str += "\n" + cost_str() + "\n";
  return str;
}

/**
 * The solution is feasible when all vehicles are feasible
 *
 * @returns true when all vehicles in solution are feasible
 */
bool
Solution::is_feasible() const {
  return std::find_if(m_fleet.begin(), m_fleet.end(),
      [] (const Vehicle_pickDeliver& v) -> bool {return !v.is_feasible();}) == m_fleet.end();
}

/**
 * @returns the solution's duration
 *
 * The solution duration is the sum of the durations of all vehicles
 */
TInterval
Solution::duration() const {
  return std::accumulate(begin(m_fleet), end(m_fleet), static_cast<TInterval>(0),
      [](TInterval i, const Vehicle_pickDeliver& v) {return v.duration() + i;});
}

/**
 * @returns the total waiting time
 *
 * The total waiting time of the solution is the sum of the waiting time of all vehicles
 */
TInterval
Solution::wait_time() const {
  return std::accumulate(begin(m_fleet), end(m_fleet), static_cast<TInterval>(0),
      [](TInterval i, const Vehicle_pickDeliver& v) {return v.total_wait_time() + i;});
}

/**
 * @returns the total waiting time
 *
 * The total travel time of the solution is the sum of the travel times of all vehicles
 */
TInterval
Solution::total_travel_time() const {
  return std::accumulate(begin(m_fleet), end(m_fleet), static_cast<TInterval>(0),
      [](TInterval i, const Vehicle_pickDeliver& v) {return v.total_travel_time() + i;});
}

/**
 *
 * @returns the total service time of the solution
 *
 * The total service time of the solution is the sum of the service times of all vehicles
 */
TInterval
Solution::total_service_time() const {
  return std::accumulate(begin(m_fleet), end(m_fleet), static_cast<TInterval>(0),
      [](TInterval i, const Vehicle_pickDeliver& v) {return v.total_service_time() + i;});
}

/**
 * @returns the total number of time window violations
 *
 * The total time window violations of the solution is the sum of the time window violations of all vehicles
 */
int
Solution::twvTot() const {
  return std::accumulate(begin(m_fleet), end(m_fleet), 0,
      [](int i, const Vehicle_pickDeliver& v) {return v.twvTot() + i;});
}

/**
 *
 * @return Total number of capacity violations
 * The total capacity violations of the solution is the sum of the capacity violations of all vehicles
 */
int
Solution::cvTot() const {
  return std::accumulate(begin(m_fleet), end(m_fleet), 0,
      [](int i, const Vehicle_pickDeliver& v) {return v.cvTot() + i;});
}

/**
 * @returns totals of
 *
 * idx | variable
 * ---- | -----
 * 0 | twv
 * 1 | cv
 * 2 | fleet size
 * 3 | waiting time
 * 4 | duration
 * 5 | travel time
 *
 * ~~~ .c
 * std::get<idx>
 * ~~~
 */
std::tuple<int, int, size_t, TInterval, TInterval, TInterval>
Solution::cost() const {
  TInterval total_duration(0);
  TInterval total_wait_time(0);
  TInterval total_tt(0);
  int total_twv(0);
  int total_cv(0);
  /*
   * Cycle the fleet
   */
  for (const auto& v : m_fleet) {
    total_duration += v.duration();
    total_wait_time += v.total_wait_time();
    total_twv += v.twvTot();
    total_cv += v.cvTot();
    total_tt += v.total_travel_time();
  }
  /*
   * build the tuple
   */
  return std::make_tuple(
      total_twv, total_cv, m_fleet.size(),
      total_wait_time, total_duration,
      total_tt);
}
/**
 * @returns The costs in one string
 */
std::string
Solution::cost_str() const {
  /*
   * get the cost
   */
  auto s_cost(cost());
  std::ostringstream log;

  /*
   * Build the string
   */
  log << std::fixed << std::setprecision(4)
    << "twv=" << std::get<0>(s_cost)
    << " cv=" << std::get<1>(s_cost)
    << " wait=" << std::get<3>(s_cost)
    << " duration=" << std::get<4>(s_cost)
    << " tt=" << std::get<5>(s_cost);

  return log.str();
}

bool
Solution::operator<(const Solution &s_rhs) const {
  auto lhs(cost());
  auto rhs(s_rhs.cost());

  /*
   * time window violations
   */
  if (std::get<0>(lhs) < std::get<0>(rhs))
    return true;
  if (std::get<0>(lhs) > std::get<0>(rhs))
    return false;

  /*
   * capacity violations
   */
  if (std::get<1>(lhs) < std::get<1>(rhs))
    return true;
  if (std::get<1>(lhs) > std::get<1>(rhs))
    return false;

  /*
   * fleet size
   */
  if (std::get<2>(lhs) < std::get<2>(rhs))
    return true;
  if (std::get<2>(lhs) > std::get<2>(rhs))
    return false;

  /*
   * waiting time
   */
  if (std::get<3>(lhs) < std::get<3>(rhs))
    return true;
  if (std::get<3>(lhs) > std::get<3>(rhs))
    return false;

  /*
   * duration
   */
  if (std::get<4>(lhs) < std::get<4>(rhs))
    return true;
  if (std::get<4>(lhs) > std::get<4>(rhs))
    return false;

  return false;
}

}  //  namespace problem
}  //  namespace vrprouting

