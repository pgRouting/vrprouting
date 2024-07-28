/*PGR-GNU*****************************************************************

FILE: fleet.hpp

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

#ifndef INCLUDE_PROBLEM_FLEET_HPP_
#define INCLUDE_PROBLEM_FLEET_HPP_
#pragma once

#include <iostream>
#include <vector>
#include <numeric>

#include "c_types/typedefs.h"
#include "cpp_common/short_vehicle.hpp"
#include "problem/vehicle_pickDeliver.hpp"

typedef struct Vehicle_t Vehicle_t;

namespace vrprouting {
class Pgr_messages;

namespace problem {
class Orders;
class Vehicle_node;

/*
 * PVR = pnode
 */
class Fleet: protected std::vector<Vehicle_pickDeliver> {
 public:
    using std::vector<Vehicle_pickDeliver>::begin;
    using std::vector<Vehicle_pickDeliver>::end;
    using std::vector<Vehicle_pickDeliver>::empty;
    using std::vector<Vehicle_pickDeliver>::at;
    using std::vector<Vehicle_pickDeliver>::back;

    /** @brief Create a fleet based on the Vehicles of the problem */
    Fleet(
        Vehicle_t* vehicles , size_t size_vehicles,
        const Orders& p_orders, std::vector<Vehicle_node>& p_nodes, size_t& node_id)
      : m_used(),
      m_unused() {
        build_fleet(vehicles, size_vehicles, {}, p_orders, p_nodes, node_id);
      }

    /** @brief Create a fleet based on the Vehicles of the problem */
    Fleet(Vehicle_t* vehicles , size_t size_vehicles,
        const std::vector<Short_vehicle> &new_stops,
        const Orders& p_orders, std::vector<Vehicle_node>& p_nodes, size_t& node_id)
      : m_used(),
      m_unused() {
        build_fleet(vehicles, size_vehicles, new_stops, p_orders, p_nodes, node_id);
      }

    /** @brief creating a fleet without information is not allowed */
    Fleet() = delete;

    /** @brief Get all the unused vehicles */
    std::vector<Vehicle_pickDeliver> get_unused_trucks();

    /** @brief Get all the used vehicles */
    std::vector<Vehicle_pickDeliver> get_used_trucks();

    /** @brief Get a vehicle from the user's defined vehicles */
    Vehicle_pickDeliver get_phony() const {return back();}

    /** @returns the fleet information in the ostream */
    /**
      @param [in] log
      @param [in] f
      */
    friend std::ostream& operator<<(std::ostream &log, const Fleet &f) {
      log << "fleet\n";
      for (const auto &v : f) log << v;
      log << "end fleet\n";
      return log;
    }

    Identifiers<size_t> feasible_orders() const;

    /** @brief removes from fleet all invalid vehicles */
    void clean();

    /** Finds an unused vehicle */
    Vehicle_pickDeliver get_truck();

    /** Finds an unused vehicle where the order can fit */
    Vehicle_pickDeliver get_truck(size_t order_idx);

    /** @brief sets the compatability of orders on the fleet */
    void set_compatibles(const Orders &orders);

    bool is_fleet_ok() const;

    bool is_order_ok(const Order &order) const;

    /** @brief set the vehicle's user's initial solution */
    void set_initial_solution(const Orders&, Identifiers<size_t>&, Identifiers<size_t>&, TTimestamp, bool);

    Pgr_messages& msg() {return m_msg;}

 protected:
    /** @brief add a new vehicle to the fleet */
    void add_vehicle(
        const Vehicle_t&, const std::vector<Short_vehicle>&,
        const Orders&,
        std::vector<Vehicle_node>& p_nodes, size_t& node_id);

    /** @brief build the fleet */
    void build_fleet(
        Vehicle_t*, size_t, const std::vector<Short_vehicle>&,
        const Orders&,
        std::vector<Vehicle_node>& p_nodes, size_t& node_id);

    void invariant() const;

    /** set of used vehicles */
    Identifiers<size_t> m_used;

    /** set of unused vehicles */
    Identifiers<size_t> m_unused;

    /** set of invalid vehicles */
    Identifiers<size_t> m_invalid;

    mutable Pgr_messages m_msg;

    /* for the invariant */
    size_t m_size;
};

}  // namespace problem
}  // namespace vrprouting
#endif  // INCLUDE_PROBLEM_FLEET_HPP_
