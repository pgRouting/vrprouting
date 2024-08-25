/*PGR-GNU*****************************************************************

FILE: pd_orders.h

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

#include "problem/orders.hpp"

#include <algorithm>
#include <vector>

#include "cpp_common/assert.hpp"
#include "cpp_common/identifiers.hpp"
#include "problem/pickDeliver.hpp"


namespace vrprouting {
namespace problem {

Orders::Orders(
        Orders_t *p_orders, size_t p_size_orders,
        PickDeliver &problem_ptr) {
    Tw_node::m_time_matrix_ptr = &(problem_ptr.time_matrix());
    build_orders(p_orders, p_size_orders, problem_ptr);
}

/**
@returns the index of the order within_this_set that has more possibilities of placing orders after it
@param [in] within_this_set
@pre not within_this_set.empty()
*/
size_t
Orders::find_best_J(
        const Identifiers<size_t> &within_this_set) const {
    pgassert(!within_this_set.empty());
    auto best_order = within_this_set.front();
    size_t max_size = 0;

    for (const auto o : within_this_set) {
        auto size_J =  this->at(o).subsetJ(within_this_set).size();
        if (max_size < size_J) {
            max_size = size_J;
            best_order = o;
        }
    }
    return best_order;
}

/**
@returns the index of the order within_this_set that has more possibilities of placing orders before it
@param [in] within_this_set
@pre not within_this_set.empty()
*/
size_t
Orders::find_best_I(
        const Identifiers<size_t> &within_this_set) const {
    pgassert(!within_this_set.empty());
    auto best_order = within_this_set.front();
    size_t max_size = 0;
    for (const auto o : within_this_set) {
        auto size_I =  this->at(o).subsetI(within_this_set).size();
        if (max_size < size_I) {
            max_size = size_I;
            best_order = o;
        }
    }
    return best_order;
}

/**
@returns the index of the order within_this_set that has more possibilities of placing orders before or after it
@param [in] within_this_set
@pre not within_this_set.empty()
*/
size_t
Orders::find_best_I_J(
        const Identifiers<size_t> &within_this_set) const {
    pgassert(!within_this_set.empty());
    auto best_order = within_this_set.front();
    size_t max_size = 0;

    for (const auto o : within_this_set) {
        auto size_I =  this->at(o).subsetI(within_this_set).size();
        auto size_J =  this->at(o).subsetJ(within_this_set).size();
        if (max_size < (std::max)(size_I, size_J)) {
            max_size = (std::max)(size_I, size_J);
            best_order = o;
        }
    }
    return best_order;
}

/**
@returns true when each order from the set of orders is valid
*/
bool
Orders::is_valid(Speed speed) const {
    for (const auto &o : *this) {
        if (!o.is_valid(speed)) {
            return false;
        }
        pgassert(o.pickup().is_pickup());
        pgassert(o.delivery().is_delivery());
        /* P -> D */
        pgassert(o.delivery().is_compatible_IJ(o.pickup(), speed));
    }
    return true;
}

/**
@post For each order: order -> {J} is set
@post For each order: {I} -> order is set
*/
void
Orders::set_compatibles(Speed speed) {
    for (auto &I : *this) {
        for (const auto& J : *this) {
            I.set_compatibles(J, speed);
        }
    }
}

/**
  @param [in] orders set of orders
  @param [in] problem_ptr pointer to problem to get some needed information
  */
void
Orders::build_orders(
        Orders_t *orders, size_t size_orders,
        PickDeliver& problem_ptr) {
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
        auto o = orders[i];
        Vehicle_node pick({problem_ptr.node_id()++, o, NodeType::kPickup});
        Vehicle_node drop({problem_ptr.node_id()++, o, NodeType::kDelivery});

        problem_ptr.add_node(pick);
        problem_ptr.add_node(drop);

        this->emplace_back(Order{size(), o.id, pick, drop});
    }
}

}  //  namespace problem
}  //  namespace vrprouting

