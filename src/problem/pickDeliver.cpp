/*WC-GNU*****************************************************************

FILE: pickDeliver.cpp

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

#include "problem/pickDeliver.hpp"

#include <vector>
#include <utility>
#include "c_types/compatibleVehicles_rt.h"
#include "c_types/solution_rt.h"
#include "cpp_common/orders_t.hpp"
#include "cpp_common/vehicle_t.hpp"
#include "cpp_common/short_vehicle.hpp"
#include "cpp_common/messages.hpp"
#include "problem/vehicle_node.hpp"
#include "problem/orders.hpp"
#include "problem/fleet.hpp"
#include "problem/matrix.hpp"

namespace vrprouting {
namespace problem {

PickDeliver::PickDeliver(
        const std::vector<Orders_t> &p_orders,
        const std::vector<Vehicle_t> &p_vehicles,
        const Matrix &p_cost_matrix) :
    m_cost_matrix(p_cost_matrix),
    m_orders(p_orders, *this),
    m_trucks(p_vehicles, m_orders, m_nodes, m_node_id) {
        if (!msg.get_error().empty()) return;
        m_trucks.clean();
        m_orders.set_compatibles();
        m_trucks.set_compatibles(m_orders);
    }

/** @brief Override stops constructor */
PickDeliver::PickDeliver(
        const std::vector<Orders_t> &p_orders,
        const std::vector<Vehicle_t> &p_vehicles,
        std::vector<Short_vehicle> new_stops,
        const Matrix &p_cost_matrix) :
    m_cost_matrix(p_cost_matrix),
    m_orders(p_orders, *this),
    m_trucks(p_vehicles, new_stops, m_orders, m_nodes, m_node_id) {
        if (!msg.get_error().empty()) return;
        m_trucks.clean();
        m_orders.set_compatibles();
        m_trucks.set_compatibles(m_orders);
    }


/** @brief get the vehicles compatibility results as C++ container */
std::vector<CompatibleVehicles_rt> PickDeliver::get_pg_compatibleVehicles() const {
    std::vector<CompatibleVehicles_rt> result;
    for (const auto& v : m_trucks) {
        if (v.is_phony()) continue;
        for (const auto o : v.feasible_orders()) {
            result.push_back({m_orders[o].id(), v.id()});
        }
    }
    return result;
}

}  //  namespace problem
}  //  namespace vrprouting
