/*PGR-GNU*****************************************************************

FILE: vehicle_pickDeliver.h

Copyright (c) 2016 pgRouting developers
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

/*! @file */

#ifndef INCLUDE_PROBLEM_VEHICLE_PICKDELIVER_H_
#define INCLUDE_PROBLEM_VEHICLE_PICKDELIVER_H_
#pragma once

#include <limits>
#include <vector>
#include "c_types/typedefs.h"
#include "cpp_common/pgr_assert.h"
#include "cpp_common/pgr_messages.h"
#include "cpp_common/identifiers.hpp"
#include "problem/vehicle.h"
#include "problem/orders.h"

namespace vrprouting {
namespace problem {
class Order;
class Orders;

class Vehicle_pickDeliver : public Vehicle {
 public:
     using Vehicle::insert;
     using Vehicle::insert_node;
     using Vehicle::erase;
     using Vehicle::size;
     using Vehicle::invariant;
     using Vehicle::evaluate;
     using Vehicle::is_feasible;

     using Vehicle::at;
     using Vehicle::empty;

     /**
      * @returns The vehicle's information on the log
      * @param [in,out] log place to store the vehicle's information
      * @param [in] v the vehicle to work with
      */
     friend std::ostream& operator<< (std::ostream &log, const Vehicle_pickDeliver &v) {
         int i(0);
         log << "\n\n****************** " << v.idx() << "th VEHICLE*************\n";
         log << "id = " << v.id()
             << "\tcapacity = " << v.capacity() << "\n";

         for (const auto &path_stop : v) {
             log << "Path_stop" << ++i << "\n";
             log << path_stop << "\n";
         }

         log << v.feasible_orders() << "\n";
         return log;
     }


     Vehicle_pickDeliver() = delete;
     Vehicle_pickDeliver(
             Idx p_idx,
             Id p_id,
             const Vehicle_node & p_starting_site,
             const Vehicle_node & p_ending_site,
             const std::vector<int64_t>& p_stops,
             PAmount p_capacity,
             Speed p_speed,
             const Orders& p_orders) :
         Vehicle(p_idx, p_id, p_starting_site, p_ending_site, p_capacity, p_speed),
         m_cost((std::numeric_limits<double>::max)()),
         m_orders_in_vehicle(),
         m_feasible_orders(),
         m_orders(p_orders),
         m_stops(p_stops) {}


     /** @brief returns the set of feasible orders for modification*/
     Identifiers<size_t>& feasible_orders() {return m_feasible_orders;}
     /** @brief returns the set of feasible orders */
     const Identifiers<size_t>& feasible_orders() const {return m_feasible_orders;}

     /** @brief returns the number of orders in the vehicle */
     Identifiers<size_t> orders_in_vehicle() const {return m_orders_in_vehicle;}

     /** @brief does the vehicle has the order? */
     bool has_order(const Order &order) const {return m_orders_in_vehicle.has(order.idx());}

     size_t orders_size() const {return m_orders_in_vehicle.size();}

     /** @brief puts an order at the end of the truck */
     void push_back(const Order &order);

     /** @brief Puts an order at the end front of the truck */
     void push_front(const Order &order);

     /** @brief Inserts an order with hill Climb approach*/
     bool hillClimb(const Order &order);

     /** @brief Inserts an order In semi-Lifo (almost last in first out) order */
     bool semiLIFO(const Order &order);

     /** @brief erases the order from the vehicle */
     void erase(const Order &order);

     size_t pop_back();
     size_t pop_front();

     // TODO(pending): move code to cpp file
     Order get_first_order() const {
         pgassert(!empty());
         return orders()[at(1).idx()];
     }

     /** @brief Get the value of the objective function */
     double objective() const;


     /** @brief sets the initial solution given by the user */
     void set_initial_solution(const Orders&, Identifiers<size_t>&, Identifiers<size_t>&, TTimestamp, bool);

     /** @brief sets as unmovable the orders that start before the execution date */
     void set_unmovable(TTimestamp execution_date);

     bool is_order_feasible(const Order &order) const;

     const Orders& orders() const {pgassert(m_orders.size() != 0); return m_orders;}

     Pgr_messages& msg() {return m_msg;}

 protected:
     using Vehicle::begin;
     using Vehicle::end;
     using Vehicle::rbegin;
     using Vehicle::rend;

     double m_cost;

     /** orders inserted in this vehicle */
     Identifiers<size_t> m_orders_in_vehicle;

     /** orders that fit in the truck */
     Identifiers<size_t> m_feasible_orders;

     Orders m_orders;

     Pgr_messages m_msg;

 private:
     /**
      * order ids of an initial solution given by the user
      * [1,2,1,3,3,2] = P1 P2 D1 P3 D3 D2
      */
     std::vector<int64_t> m_stops;
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_VEHICLE_PICKDELIVER_H_
