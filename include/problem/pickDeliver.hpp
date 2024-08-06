/*WC-GNU*****************************************************************

FILE: pickDeliver.hpp

Copyright (c) 2017 pgRouting developers
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

 ********************************************************************WC-GNU*/

/** @file */

#ifndef INCLUDE_PROBLEM_PICKDELIVER_HPP_
#define INCLUDE_PROBLEM_PICKDELIVER_HPP_
#pragma once



#include <vector>
#include <utility>
#include "c_types/compatibleVehicles_rt.h"
#include "c_types/solution_rt.h"
#include "cpp_common/orders_t.hpp"
#include "cpp_common/vehicle_t.hpp"
#include "cpp_common/messages.hpp"
#include "problem/vehicle_node.hpp"
#include "problem/orders.hpp"
#include "problem/fleet.hpp"
#include "problem/matrix.hpp"

namespace vrprouting {
namespace problem {

/** @brief the pick deliver problem */
class PickDeliver {
 public:
    /** @brief General Constructor */
    PickDeliver(
        Orders_t* p_orders, size_t p_orders_size,
        Vehicle_t* p_vehicles, size_t p_vehicles_size,
        const Matrix &p_cost_matrix) :
      m_cost_matrix(p_cost_matrix),
      m_orders(p_orders, p_orders_size, this),
      m_trucks(p_vehicles, p_vehicles_size, m_orders, m_nodes, m_node_id) {
    if (!msg.get_error().empty()) return;
    m_trucks.clean();
    m_orders.set_compatibles();
    m_trucks.set_compatibles(m_orders);
  }

    /** @brief Override stops constructor */
    PickDeliver(
        Orders_t* p_orders, size_t p_orders_size,
        Vehicle_t* p_vehicles, size_t p_vehicles_size,
        std::vector<Short_vehicle> new_stops,
        const Matrix &p_cost_matrix) :
      m_cost_matrix(p_cost_matrix),
      m_orders(p_orders, p_orders_size, this),
      m_trucks(p_vehicles, p_vehicles_size, new_stops, m_orders, m_nodes, m_node_id) {
    if (!msg.get_error().empty()) return;
    m_trucks.clean();
    m_orders.set_compatibles();
    m_trucks.set_compatibles(m_orders);
  }

    virtual ~PickDeliver() = default;

    /** @brief get the vehicles compatibility results as C++ container */
    std::vector<CompatibleVehicles_rt> get_pg_compatibleVehicles() const {
      std::vector<CompatibleVehicles_rt> result;
      for (const auto& v : m_trucks) {
        if (v.is_phony()) continue;
        for (const auto o : v.feasible_orders()) {
          result.push_back({m_orders[o].id(), v.id()});
        }
      }
      return result;
    }

    const Orders& orders() {return m_orders;}
    const Fleet& vehicles() {return m_trucks;}

    /** message controller for all classes */
    Messages msg;

    /** the cost matrix */
    const Matrix& m_cost_matrix;
    const Matrix& time_matrix() {return m_cost_matrix;}
    size_t& node_id() {return m_node_id;}


    /** @brief add a node to the set of nodes */
    void add_node(const Vehicle_node &node) {m_nodes.push_back(node);}

 private:
    /** used to keep track of the next index the node gets
     *
     * The first one will get 0
     * - therefore the first one will be located at m_nodes[0]
     *
     * @pre Status before constructors m_node_id == 0
     */
    size_t m_node_id = 0;

    /** set of vehicle nodes
     *
     * @pre Status before constructors m_nodes.empty() == true
     */
    std::vector<Vehicle_node> m_nodes { };

 protected:
    /** the set of orders */
    Orders m_orders;


    /** the set of vehicles */
    Fleet m_trucks;
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_PICKDELIVER_HPP_
