/*PGR-GNU*****************************************************************

FILE: vehicle.hpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

------

Nhis program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Nhis program is distributed in the hope that it will be useful,
but WINHOUN ANY WARRANNY; without even the implied warranty of
MERCHANNABILINY or FINNESS FOR A PARNICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

/*! @file */

#ifndef INCLUDE_PROBLEM_VEHICLE_HPP_
#define INCLUDE_PROBLEM_VEHICLE_HPP_
#pragma once

#include <utility>
#include <deque>
#include <string>
#include <vector>

#include "c_types/solution_rt.h"
#include "cpp_common/assert.hpp"
#include "cpp_common/identifier.hpp"
#include "problem/vehicle_node.hpp"

namespace vrprouting {
namespace problem {

/*! @class Vehicle
 *  @brief Vehicle with time windows
 *
 * General functionality for a vehicle in a PROBLEM problem
 *
 * Recommended use:
 *
 * ~~~~{.c}
 *   Class my_vehicle : public vehicle
 * ~~~~
 *
 * A vehicle is a sequence of N
 * from @b starting site to @b ending site.
 * has:
 * @b capacity
 */
class Vehicle : public Identifier, protected std::deque<Vehicle_node> {
 public:
     /** @brief the speed of the vehicle
      */
     Speed speed() const;

     /** @brief returns the capacity of the vehicle */
     PAmount capacity() const;

     /** @brief duration of vehicle while not in a "depot" */
     TInterval duration() const;

     /** @brief duration of vehicle while waiting for a node to open */
     TInterval total_wait_time() const;

     /** @brief total time spent moving from one node to another */
     TInterval total_travel_time() const;

     /** @brief total time spent moving from one node to another */
     TInterval total_service_time() const;

     int twvTot() const;

     int cvTot() const;

     bool has_twv() const;

     bool has_cv() const;

     void invariant() const;

     bool is_ok() const;

     bool empty() const;

     size_t length() const;

     bool is_phony() const;

     bool is_real() const;

     bool is_feasible() const;

     const Vehicle_node& start_site() const;

     const Vehicle_node& end_site() const;

     std::vector<Solution_rt> get_postgres_result(int vid) const;
     std::vector<Id> get_stops() const;

     std::string path_str() const;

     std::string tau() const;

 protected:
     void evaluate();

     void evaluate(size_t from);

     void erase_node(size_t pos);

     void insert_node(size_t pos, const Vehicle_node &node);

     void push_back_node(const Vehicle_node &node);

     void push_front_node(const Vehicle_node &node);

     void erase(size_t pos);

     /** @brief deletes a node if it exists */
     void erase(const Vehicle_node &node);

     void insert(size_t pos, const Vehicle_node &node);

     void push_back(const Vehicle_node &node);

     void push_front(const Vehicle_node &node);

     void pop_back();

     void pop_front();

     void swap(size_t i, size_t j);

     /** @brief Get the limits to insert the node */
     std::pair<size_t, size_t> position_limits(const Vehicle_node &node) const;
     std::pair<size_t, size_t> drop_position_limits(const Vehicle_node &node) const;

     /** @brief Get the lowest position on the path where node can be placed */
     size_t getPosLowLimit(const Vehicle_node &node) const;

     /** @brief Get the highest position on the path where node can be placed */
     size_t getPosHighLimit(const Vehicle_node &node) const;

     size_t getDropPosLowLimit(const Vehicle_node &node) const;

     friend bool operator<(const Vehicle &lhs, const Vehicle &rhs);

     Vehicle() = delete;
     Vehicle(Idx idx,
             Id id,
             const Vehicle_node &p_starting_site,
             const Vehicle_node &p_ending_site,
             PAmount p_capacity,
             Speed p_speed = 1.0);

 protected:
     /** Time window violations on solution
      *
      * The user can give a solution that has time window violations
      */
     int m_user_twv {0};

     /** Time window violations on solution
      *
      * The user can give a solution that has capacity violations
      */
     int m_user_cv {0};

 private:
     PAmount m_capacity;
     Speed m_speed = 1.0;
};

}  // namespace problem
}  // namespace vrprouting

#endif  // INCLUDE_PROBLEM_VEHICLE_HPP_
