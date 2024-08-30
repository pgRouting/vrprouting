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

#include "cpp_common/messages.hpp"
#include "problem/orders.hpp"
#include "problem/fleet.hpp"

using CompatibleVehicles_rt = struct CompatibleVehicles_rt;

namespace vrprouting {

class Orders_t;
class Vehicle_t;
class Short_vehicle;

namespace problem {

class Matrix;
class Vehicle_node;


/** @brief the pick deliver problem */
class PickDeliver {
 public:
    /** @brief General Constructor */
    PickDeliver(
        const std::vector<Orders_t>&,
        const std::vector<Vehicle_t>&,
        const Matrix &);

    /** @brief Override stops constructor */
    PickDeliver(
        const std::vector<Orders_t>&,
        const std::vector<Vehicle_t>&,
        std::vector<Short_vehicle>,
        const Matrix&);

    virtual ~PickDeliver() = default;

    /** @brief get the vehicles compatibility results as C++ container */
    std::vector<CompatibleVehicles_rt> get_pg_compatibleVehicles() const;

    const Orders& orders() {return m_orders;}
    const Fleet& vehicles() {return m_trucks;}

    /** message controller for all classes */
    Messages msg;

    /** the cost matrix */
    const Matrix& m_cost_matrix;
    const Matrix& time_matrix() const {return m_cost_matrix;}
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
