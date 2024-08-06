/*PGR-GNU*****************************************************************

FILE: orders.hpp

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

/*! @file */

#ifndef INCLUDE_PROBLEM_ORDERS_HPP_
#define INCLUDE_PROBLEM_ORDERS_HPP_
#pragma once

#include <vector>
#include <algorithm>

#include "problem/order.hpp"
#include "cpp_common/orders_t.hpp"
#include "cpp_common/assert.hpp"
#include "cpp_common/identifiers.hpp"
#include "problem/tw_node.hpp"
#include "problem/vehicle_node.hpp"
#include "problem/node_types.hpp"



namespace vrprouting {
namespace problem {

class Orders : public std::vector<Order> {
 public:
    using std::vector<Order>::size;
    Orders() = default;

    // todo remove template its problem::PickDeliver
    template <typename PTR>
      Orders(Orders_t* p_orders, size_t p_size_orders, const PTR problem_ptr) {
        Tw_node::m_time_matrix_ptr = &problem_ptr->time_matrix();
        build_orders(p_orders, p_size_orders, problem_ptr);
      }

    /** @brief find the best order -> @b this */
    size_t find_best_I(const Identifiers<size_t> &within_this_set) const;

    /** @brief find the best order -> @b this -> order */
    size_t find_best_J(const Identifiers<size_t> &within_this_set) const;

    /** @brief find the best order -> @b this -> order */
    size_t find_best_I_J(const Identifiers<size_t> &within_this_set) const;

    /** @brief set the compatability between all orders */
    void set_compatibles(Speed = 1.0);

    /** @brief is the order valid? */
    bool is_valid(Speed = 1.0) const;

    friend std::ostream& operator<<(std::ostream &log, const Orders &p_orders) {
      log << "Orders\n";
      for (const auto &o : p_orders) log << o << "\n";
      log << "end Orders\n";
      return log;
    }

 private:
    template <typename PTR>
      void build_orders(Orders_t *, size_t, const PTR problem_ptr);

    /** @brief add in an order */
    void add_order(const Orders_t &order,
        const Vehicle_node &pick,
        const Vehicle_node &drop) {
      push_back(Order(size(), order.id, pick, drop));
    }
};

/**
  @param [in] orders
  @param [in] size_orders
  @param [in] problem_ptr pointer to problem to get some needed information
  */
template <typename PTR>
void
Orders::build_orders(Orders_t *orders, size_t size_orders, const PTR problem_ptr) {
  /**
   * - Sort orders: ASC pick_open_t, deliver_close_t, id
   */
  std::sort(orders, orders + size_orders,
      [] (const Orders_t &lhs, const Orders_t &rhs) {
        if (lhs.pick_open_t == rhs.pick_open_t) {
          if (lhs.deliver_close_t == rhs.deliver_close_t) {
            return lhs.id < rhs.id;
          } else {
            return lhs.deliver_close_t < rhs.deliver_close_t;
          }
        } else {
          return lhs.pick_open_t < rhs.pick_open_t;
        }
      });

  for (size_t i = 0; i < size_orders; ++i) {
    auto order = orders[i];
    Vehicle_node pick({problem_ptr->node_id()++, order, NodeType::kPickup});
    Vehicle_node drop({problem_ptr->node_id()++, order, NodeType::kDelivery});

    problem_ptr->add_node(pick);
    problem_ptr->add_node(drop);

    this->emplace_back(Order{size(), order.id, pick, drop});
  }
}

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_ORDERS_HPP_
