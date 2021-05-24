/*PGR-GNU*****************************************************************

FILE: order.cpp

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

#include "problem/order.h"

namespace vrprouting {
namespace problem {

/**
  @returns Orders from @b I that are compatible with @b this order
  @param[in] I Set of orders that possibly are I -> @b this
  @dot
  digraph G {
  graph [rankdir=LR];
  this [color=green];
  "i" -> this;
  }
  @enddot
  */
Identifiers<size_t>
Order::subsetI(const Identifiers<size_t> &I) const {
  return m_compatibleI * I;
}

/**
  @returns Orders from @b J that are compatible with @b this order
  @param[in] J Set of orders that possibly are @b this -> J
  @dot
  digraph G {
  graph [rankdir=LR];
  this [color=green];
  this -> "j";
  }
  @enddot

*/
Identifiers<size_t>
Order::subsetJ(const Identifiers<size_t> &J) const {
  return m_compatibleJ * J;
}

/**
Prints:
- the pickup
- dropoff
- Set of orders I than are I -> @b this
- Set of orders J than are @b this -> I
@param[in,out] log
@param[in] order
@returns the ostream
*/
std::ostream&
operator<< (std::ostream &log, const Order &order) {
  log << "\n\nOrder "
    << static_cast<Identifier>(order) << ": \n"
    << "\tPickup: " << order.pickup() << "\n"
    << "\tDropoff: " << order.delivery() << "\n";
  log << "\nThere are | {I}| = "
    << order.m_compatibleI.size()
    << " -> order(" << order.idx()
    << ") -> | {J}| = " << order.m_compatibleJ.size()
    << "\n\n {";
  for (const auto o : order.m_compatibleI) {
    log << o << ", ";
  }
  log << "} -> " << order.idx() << " -> {";
  for (const auto o : order.m_compatibleJ) {
    log << o << ", ";
  }
  log << "}";

  return log;
}

/**
  @param[in] speed value to do the calculation
  @returns ture  when:

  Validate a pickup/delivery order
  - The pickup is well formed
  - The delivery is well formed
  - isCompatibleIJ to go to delivery after immediately visiting pickup
  */
bool
Order::is_valid(Speed speed) const {
  return
    pickup().is_pickup()
    && delivery().is_delivery()
    /* IS P -> D */
    && delivery().is_compatible_IJ(pickup(), speed);
}

/**
 * Initializing the set of nodes that can be placed
 * immediately after @b this node
 *
 * (this) -> J
 *
 * @param [in] J order after @b this node
 * @param [in] speed value to do the calculation
 * @post m_compatibleJ has order when @b this -> order
 * @post m_compatibleI has order when order -> @b this
 *
 */
void
Order::set_compatibles(const Order& J, Speed speed) {
  if (J.idx() == idx()) return;
  if (J.isCompatibleIJ(*this, speed)) {
    m_compatibleJ += J.idx();
  }
  if (this->isCompatibleIJ(J, speed)) {
    m_compatibleI += J.idx();
  }
}

/**
  @returns True when I -> @b this
  @dot
  digraph G {
  "other(P)" -> "other(D)" [color="blue"]
  "other(D)" -> "this(P)" [color="blue"]
  "this(P)" -> "this(D)" [color="blue"];
  "other(P)" -> "this(P)" [color="red"]
  "this(P)" -> "other(D)" [color="red"]
  "other(D)" -> "this(D)"  [color="red"];
  "other(P)" -> "this(P)" [color="green"]
  "this(P)" -> "this(D)"  [color="green"]
  "this(D)" -> "other(D)"  [color="green"];
  }
  @enddot

  @param[in] I order
  @param[in] speed value to do the calculation
  @returns true when order @b I can be placed before @b this order
  */
bool
Order::isCompatibleIJ(const Order &I, Speed speed) const {
  /* this is true in all cases */
  auto all_cases(
      pickup().is_compatible_IJ(I.pickup(), speed)
      && delivery().is_compatible_IJ(I.pickup(), speed));

  /* case other(P) other(D) this(P) this(D) */
  auto case1(pickup().is_compatible_IJ(I.delivery(), speed)
      && delivery().is_compatible_IJ(I.delivery(), speed));

  /* case other(P) this(P) other(D) this(D) */
  auto case2(I.delivery().is_compatible_IJ(pickup(), speed)
      && delivery().is_compatible_IJ(I.delivery(), speed));

  /* case other(P) this(P) this(D) other(D) */
  auto case3(I.delivery().is_compatible_IJ(pickup(), speed)
      && I.delivery().is_compatible_IJ(delivery(), speed));

  return all_cases &&  (case1 ||  case2 ||  case3);
}

}  //  namespace problem
}  //  namespace vrprouting

