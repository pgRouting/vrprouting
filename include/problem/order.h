/*PGR-GNU*****************************************************************

FILE: order.h

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

/** @file */

#ifndef INCLUDE_PROBLEM_ORDER_H_
#define INCLUDE_PROBLEM_ORDER_H_
#pragma once


#include "cpp_common/identifier.hpp"
#include "cpp_common/identifiers.hpp"
#include "problem/vehicle_node.h"

namespace vrprouting {
namespace problem {

class Order : public Identifier {
 public:
    /** @brief Print the order */
    friend std::ostream& operator << (std::ostream&, const Order &);

    /** @brief Order without data is not permitted */
    Order() = delete;

    /** @brief initializing an order with the pick & drop information */
    Order(Idx o_idx, Id o_id,
        const Vehicle_node &p_pickup,
        const Vehicle_node &p_delivery) :
      Identifier(o_idx, o_id),
      m_pickup(p_pickup),
      m_delivery(p_delivery) {
      }

    /** @name Accessors
     * @{
     */

    /** @brief The pickup node identifier */
    const Vehicle_node& pickup() const {return m_pickup;}

    /** @brief The delivery node identifier */
    const Vehicle_node& delivery() const {return m_delivery;}

    /** @brief Get a subset of the orders that can be placed after @b this order */
    Identifiers<size_t> subsetJ(const Identifiers<size_t> &J) const;

    /** @brief Get a subset of the orders that can be placed before @b this order */
    Identifiers<size_t> subsetI(const Identifiers<size_t> &I) const;

    /** @} */

    /** @brief set compatability of @b this orther with the other order */
    void set_compatibles(const Order&, Speed speed = 1.0);

    /** @brief is the order valid? */
    bool is_valid(Speed speed = 1.0) const;

    /** @brief Can order @b I be placed before @b this order? */
    bool isCompatibleIJ(const Order &I, Speed speed = 1.0) const;

 protected:
    /** Storage for the orders that can be placed after @b this order
     *
     @dot
     digraph G {
     graph [rankdir=LR];
     this [color=green];
     this -> "{J}";
     }
     @enddot
     */
    Identifiers<size_t> m_compatibleJ;

    /** Storage for the orders that can be placed before @b this order
     *
     @dot
     digraph G {
     graph [rankdir=LR];
     this [color=green];
     "{I}" -> this;
     }
     @enddot
     */
    Identifiers<size_t> m_compatibleI;

    /** The pick up node identifier
     *
     * It hold's the idx of the node
     */
    Vehicle_node m_pickup;

    /** The delivery node identifier
     *
     * It hold's the idx of the node
     */
    Vehicle_node m_delivery;
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_ORDER_H_
