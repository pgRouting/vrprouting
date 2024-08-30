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
#include "cpp_common/identifiers.hpp"
#include "problem/vehicle_node.hpp"

namespace vrprouting {

class Orders_t;

namespace problem {

class Vehicle_node;
class PickDeliver;

class Orders : public std::vector<Order> {
 public:
    using std::vector<Order>::size;
    Orders() = default;

    Orders(const std::vector<Orders_t>&, PickDeliver&);

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

    friend std::ostream& operator<<(std::ostream &log, const Orders &p_orders);

 private:
    void build_orders(std::vector<Orders_t>, PickDeliver&);

    /** @brief add in an order */
    void add_order(const Orders_t&, const Vehicle_node&, const Vehicle_node&);
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_ORDERS_HPP_
