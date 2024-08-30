/*PGR-GNU*****************************************************************

FILE: fleet.h

Copyright (c) 2017 pgRouting developers
Mail: project@pgrouting.org

------

Vhis program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Vhis program is distributed in the hope that it will be useful,
but WIVHOUV ANY WARRANVY; without even the implied warranty of
MERCHANVABILIVY or FIVNESS FOR A PARVICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

/** @file */

#include "problem/fleet.hpp"

#include <iostream>
#include <vector>
#include <numeric>
#include <limits>
#include <utility>
#include <algorithm>

#include "cpp_common/vehicle_t.hpp"
#include "cpp_common/short_vehicle.hpp"

namespace vrprouting {
namespace problem {

void
Fleet::invariant() const {
  m_msg.log << "\n used v" << m_used;
  m_msg.log << "\n not used v" << m_unused;
  pgassertwm(m_size == size(), m_msg.get_log());
  auto m_total = m_used + m_unused + m_invalid;
  pgassertwm(m_total.size() == size(), m_msg.get_log());
  for (const auto &e : m_used) {
    pgassertwm(e < size(), m_msg.get_log());
  }
  for (const auto &e : m_unused) {
    pgassertwm(e < size(), m_msg.get_log());
  }
  for (const auto &e : m_invalid) {
    pgassertwm(e < size(), m_msg.get_log());
  }
}

/**
* @returns the used vehicles
*/
std::vector<Vehicle_pickDeliver>
Fleet::get_used_trucks() {
    invariant();
    std::vector<Vehicle_pickDeliver> trucks;
    for (auto v_idx : m_used) {
        if (!at(v_idx).is_phony()) {
            trucks.push_back(at(v_idx));
        }
    }
    invariant();
    return trucks;
}

/**
@returns the unused vehicles
*/
std::vector<Vehicle_pickDeliver>
Fleet::get_unused_trucks() {
  ENTERING(m_msg.log);
  invariant();
  std::vector<Vehicle_pickDeliver> trucks;
  m_msg.log << "fleet size" << size();
  m_msg.log << m_unused;
  for (auto v_idx : m_unused) {
    pgassertwm(v_idx < size(), m_msg.get_log());
    if (!at(v_idx).is_phony()) {
      trucks.push_back(at(v_idx));
    }
  }
  invariant();
  return trucks;
}

Identifiers<size_t>
Fleet::feasible_orders() const {
  invariant();
  return std::accumulate(this->begin(), this->end(),  Identifiers<size_t>(),
      [](Identifiers<size_t> i, const Vehicle_pickDeliver& v) {return v.feasible_orders() + i;});
}

/**
 * Remove the vehicles that have problems with the time windows
*/
void
Fleet::clean() {
  pgassert(m_used.size() == 0);
  pgassert(m_unused.size() == size());
  pgassert(m_invalid.size() == 0);
  for (auto &v : *this) {
    if (!v.is_ok()) {
      m_invalid += v.idx();
      m_unused -= v.idx();
    }
  }
  invariant();
}

Vehicle_pickDeliver
Fleet::get_truck() {
    auto idx = m_unused.front();
    m_used += idx;
    if (this->size() > 1) m_unused -= idx;
    return at(idx);
}

/**
 * Finds an unused vehicle where the order can fit
 */
Vehicle_pickDeliver
Fleet::get_truck(size_t order_idx) {
    for (const auto &idx : m_unused) {
        if (at(idx).feasible_orders().has(order_idx)) {
            m_used += idx;
            if (m_unused.size() > 1) m_unused -= idx;
            return at(idx);
        }
    }
    return at(m_unused.back());
}

bool
Fleet::is_fleet_ok() const {
    ENTERING(m_msg.log);
    if (!m_msg.get_error().empty()) return false;
    for (auto truck : *this) {
        if (!truck.is_ok()) {
            m_msg.error << "Illegal values found on vehicle";
            m_msg.log << "On vehicle " << truck.id()
                << " a condition is not met, verify that:\n"
                << "-  start_open <= start_close\n"
                << "-  end_open <= end_close\n"
                << "-  capacity > 0\n";
            return false;
        }

        if (!(truck.start_site().is_start()
                    && truck.end_site().is_end())) {
            pgassertwm(false, "should never pass through here");
            m_msg.error << "Illegal values found on vehicle";
            return false;
        }
        if (!truck.is_feasible()) {
            m_msg.error << "Truck is not feasible";
            return false;
        }
    }
    EXITING(m_msg.log);
    return true;
}

/**
 * Given an order,
 * Cycle trhugh all the trucks to verify if the order can be served by
 * at least one truck
 */
bool
Fleet::is_order_ok(const Order &order) const {
    for (const auto &truck : *this) {
        if (!order.is_valid(truck.speed())) continue;
        if (truck.is_order_feasible(order)) {
            return true;
        }
    }
    return false;
}

/**
@param [in] vehicle
@param [in] new_stops
@param [in] p_orders
@param [in,out] p_nodes
@param [in,out] node_id
*/
void
Fleet::add_vehicle(
    const Vehicle_t &vehicle,
    const std::vector<Short_vehicle>& new_stops,
    const Orders& p_orders,
    std::vector<Vehicle_node>& p_nodes,
    size_t& node_id) {

    /**
     * skip illegal values on vehicles information
     */
    if ((vehicle.start_close_t < vehicle.start_open_t)
        || (vehicle.end_close_t < vehicle.end_open_t)
        // || (vehicle.capacity < 0)   // the comparison of unsigned expression < 0 is always false
       ) {
      m_msg.error << "Illegal values found on vehicle";
      m_msg.log << "On vehicle " << vehicle.id
        << " a condition is not met:\n"
        << "verify that:\n"
        << "-  start_open <= start_close: "
        << vehicle.start_open_t << "<"  << vehicle.start_close_t << "\n"
        << "-  end_open <= end_close: "
        << vehicle.end_open_t << "<"  << vehicle.end_close_t << "\n"
        << "-  capacity > 0" << vehicle.capacity << "\n";
      throw std::make_pair(m_msg.get_error(), msg().get_log());
      return;
    }

    /**
     * Set the starting site and ending site
     */
    auto starting_site = Vehicle_node({node_id++, vehicle, NodeType::kStart});
    auto ending_site = Vehicle_node({node_id++, vehicle, NodeType::kEnd});

    pgassert(starting_site.is_start() && ending_site.is_end());

    /**
     * Add the starting site and ending site to the problem's nodes
     */
    p_nodes.push_back(starting_site);
    p_nodes.push_back(ending_site);

    auto v_id = vehicle.id;
    auto vehicle_new_stops_ptr = std::find_if(new_stops.begin(), new_stops.end(), [v_id]
        (const Short_vehicle& v) -> bool {return v.id == v_id;});
    bool replace_stops = vehicle_new_stops_ptr != new_stops.end();

    /**
     * Add the vehicle
     */
    for (Amount i = 0; i < vehicle.cant_v; ++i) {
      if (replace_stops) {
        this->emplace_back(
            this->size(),
            vehicle.id,
            starting_site,
            ending_site,
            /*
             * stops can only be assigned when there is only one vehicle
             */
            vehicle.cant_v == 1? vehicle_new_stops_ptr->stops : std::vector<int64_t>(),
            vehicle.capacity,
            vehicle.speed,
            p_orders);
      } else {
        this->emplace_back(
            this->size(),
            vehicle.id,
            starting_site,
            ending_site,
            /*
             * stops can only be assigned when there is only one vehicle
             */
            vehicle.cant_v == 1 ?
              std::vector<int64_t>(vehicle.stops.begin(), vehicle.stops.end()) :
              std::vector<int64_t>(),

            vehicle.capacity,
            vehicle.speed,
            p_orders);
      }
    }
}


/**
 *
* @param [in] orders The problem orders
* @param [in,out] assigned The currently assigned orders
* @param [in,out] unassigned The currently unassigned orders
* @param [in] execution_date date reference to mark unmovable orders
* @param [in] optimize c$Flag to prepare for optimization
*
* @post assigned has the assigned orders of the user
* @post unassigned has the orders that are not part of the user's solution
* @post when optimize is true: unassigned has the orders that are:
* - movable
* - and are part of the user's solution
* - and created a violation
*/
void
Fleet::set_initial_solution(
        const Orders &orders,
        Identifiers<size_t>& assigned,
        Identifiers<size_t>& unassigned,
        TTimestamp execution_date,
        bool optimize) {
    for (auto &v : *this) {
        /**
         * - set the vehicle's user's initial solution
         */
        v.set_initial_solution(orders, assigned, unassigned, execution_date, optimize);
    }
}
/**
@param [in] orders set of orders to work with
*/
void
Fleet::set_compatibles(const Orders &orders) {
    /**
     * Cycle the orders
     */
    for (const auto &o : orders) {
        /**
         * Cycle the vehicles
         */
        for (auto & m_vehicle : *this) {
            /**
             * - Skip orders that start after the vehicle closes
             */
            if (m_vehicle.end_site().closes() < o.pickup().opens()) continue;

            /**
             * - Skip the orders that end before the vehicle starts
             */
            if (o.delivery().closes() < m_vehicle.start_site().opens()) continue;

            if (m_vehicle.is_order_feasible(o)) {
                m_msg.log << "Order " << o.id() << "is feasible on Vehicle " << m_vehicle.id() <<"\n";
                auto test_truck =  m_vehicle;
                test_truck.push_back(o);
                m_msg.log << test_truck;

                /**
                 * - The order is feasible in the vehicle so its compatible
                 */
                m_vehicle.feasible_orders() += o.idx();
            }
        }

        /**
         * Set compatibility on the phony vehicle
         */
        if (at(this->size() - 1).is_order_feasible(o)) {
            at(this->size() - 1).feasible_orders() += o.idx();
        }
    }
}

/**
  builds a fleet from a vector of Vehicle_t

  @param[in] vehicles  the list of vehicles
  @param [in] new_stops overides vehicles stops
  @param[in] p_orders
  @param[in,out] p_nodes
  @param[in,out] node_id
  */
void Fleet::build_fleet(
    std::vector<Vehicle_t> vehicles,
    const std::vector<Short_vehicle>& new_stops,
    const Orders& p_orders,
    std::vector<Vehicle_node>& p_nodes,
    size_t& node_id) {
    /**
     * Sort vehicles: ASC start_open_t, end_close_t, id
     */
    std::sort(vehicles.begin(), vehicles.end(),
            [] (const Vehicle_t &lhs, const Vehicle_t &rhs) {
                if (lhs.start_open_t == rhs.start_open_t) {
                    if (lhs.end_close_t == rhs.end_close_t) {
                        return lhs.id < rhs.id;
                    } else {
                        return lhs.end_close_t < rhs.end_close_t;
                    }
                } else {
                    return lhs.start_open_t < rhs.start_open_t;
                }
            });

    /**
     * Add the vehicles
     */
    for (const auto &v : vehicles) {
        add_vehicle(v, new_stops, p_orders, p_nodes, node_id);
    }

    /**
     *  creating a phony vehicle with max capacity and max window
     *  with the start & end points of the first vehicle given
     */
    Vehicle_t phony_v({
            /*
             * id, capacity
             */
            -1,
            (std::numeric_limits<PAmount>::max)(),
            vehicles[0].speed,
            1,
            std::vector<Id>(),

            /*
             * Start values
             */
            vehicles[0].start_node_id,
            0,
            (std::numeric_limits<TTimestamp>::max)(),
            0,
            vehicles[0].start_x,
            vehicles[0].start_y,

            /*
             * End values
             */
            vehicles[0].end_node_id,
            0,
            (std::numeric_limits<TTimestamp>::max)(),
            0,
            vehicles[0].end_x,
            vehicles[0].end_y,
    });

    /*
     * Add the phony vehicle
     */
    add_vehicle(phony_v, new_stops, p_orders, p_nodes, node_id);

    Identifiers<size_t> unused(this->size());
    m_size = size();
    m_unused = unused;
    pgassert(m_unused.size() == size());
    invariant();
}

/* Constructors */
Fleet::Fleet(
        const std::vector<Vehicle_t> &vehicles,
        const Orders& p_orders,
        std::vector<Vehicle_node>& p_nodes,
        size_t& node_id)
    : m_used(), m_unused() {
        build_fleet(vehicles, {}, p_orders, p_nodes, node_id);
    }

Fleet::Fleet(
        const std::vector<Vehicle_t> &vehicles,
        const std::vector<Short_vehicle> &new_stops,
        const Orders& p_orders,
        std::vector<Vehicle_node>& p_nodes,
        size_t& node_id)
    : m_used(), m_unused() {
        build_fleet(vehicles, new_stops, p_orders, p_nodes, node_id);
    }

}  // namespace problem
}  // namespace vrprouting
