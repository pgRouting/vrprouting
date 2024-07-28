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

#include "problem/vehicle_pickDeliver.hpp"

#include <limits>
#include <utility>
#include <vector>

#include "problem/vehicle.hpp"
#include "problem/order.hpp"
#include "problem/orders.hpp"

namespace vrprouting {
namespace problem {


/**
 *
 * @param [in] order order to be pushed back
 * @pre !has_order(order)
 * @post has_order(order)
 *
 * ~~~~{.c}
 * Before: S <nodes> E
 *  After: S <nodes> P D E
 * ~~~~
 *
 * @post Can generate time window violation
 * @post Can generate capacity violation
 */
void
Vehicle_pickDeliver::push_back(const Order &order) {
  pgassert(!has_order(order));
  m_orders_in_vehicle += order.idx();
  insert_node(size() - 1, order.pickup());
  insert_node(size() - 1, order.delivery());
  evaluate(size() - 3);
  pgassert(has_order(order));
}

/**
 * Precondition:
 * !has_order(order)
 *
 * Postcondition:
 * has_order(order)
 * !has_cv();
 *
 * ~~~~{.c}
 * Before: S <nodes> E
 *   After: S P D <nodes> E
 * ~~~~
 *
 * Can generate time window violation
 * No capacity violation
 */

void
Vehicle_pickDeliver::push_front(const Order &order) {
  pgassert(!has_order(order));
  m_orders_in_vehicle += order.idx();
  insert_node(1, order.delivery());
  insert_node(1, order.pickup());
  evaluate();
  pgassert(has_order(order));
}


size_t
Vehicle_pickDeliver::pop_back() {
  invariant();
  pgassert(!empty());

  auto pick_itr = rbegin();
  while (pick_itr != rend() &&  !pick_itr->is_pickup()) {
    ++pick_itr;
  }

  pgassert(pick_itr->is_pickup());

  auto deleted_pick_idx = pick_itr->idx();

  for (const auto &o : this->orders()) {
    if (o.pickup().idx() == deleted_pick_idx) {
      erase(o);
      invariant();
      return o.idx();
    }
  }
  pgassert(false);
  return 0;
}


size_t
Vehicle_pickDeliver::pop_front() {
  invariant();
  pgassert(!empty());

  auto pick_itr = begin();
  while (pick_itr != end() &&  !pick_itr->is_pickup()) {
    ++pick_itr;
  }

  pgassert(pick_itr->is_pickup());

  auto deleted_pick_idx = pick_itr->idx();

  for (const auto &o : this->orders()) {
    if (o.pickup().idx() == deleted_pick_idx) {
      erase(o);
      invariant();
      return o.idx();
    }
  }

  pgassert(false);
  return 0;
}

/**
 * @param [in] order order to be removed from the vehicle
 * @pre has_order(order)
 * @post !has_order(order)
 *
 */

void
Vehicle_pickDeliver::erase(const Order &order) {
  pgassert(has_order(order));
  erase(order.pickup());
  erase(order.delivery());
  m_orders_in_vehicle -= order.idx();
  pgassert(!has_order(order));
}

/**
  @param [in] orders from the problem
  @param [in] assigned set of orders ids already assigned
  @param [in] unassigned set of orders to be assigned
  @param [in] execution_date reference date to set orders unmovable
  @param [in] optimize flag to set up for optimization
  */

void
Vehicle_pickDeliver::set_initial_solution(
    const Orders &orders,
    Identifiers<size_t>& assigned,
    Identifiers<size_t>& unassigned,
    TTimestamp execution_date,
    bool optimize) {
  /**
   * check pre conditions
   */
  pgassert(this->m_user_twv == 0);
  pgassert(this->m_user_cv == 0);

  /**
   * no stops -> exit
   */
  if (m_stops.empty()) return;
  std::vector<size_t> stops;
#if 1
  msg().log << "\n******\n" << this->id() << "\t stops(o_id) -> ";
  for (const auto &s : m_stops) {
    msg().log << s << ", ";
  }
#endif


  Identifiers<size_t> picked_orders;
  size_t i(0);
  /**
   * Cycle the stops
   */
  for (const auto o_id : m_stops) {
    /**
     * Find the order
     */
    auto order = std::find_if(orders.begin(), orders.end(), [o_id]
        (const Order& o) -> bool {
          return o.id() == o_id;
        });

    pgassert(order != orders.end());

    if (assigned.has(order->idx())) {
      pgassertwm(false, "Assigning an order twice");
    }

    stops.push_back(order->idx());
    if (picked_orders.has(order->idx())) {
      /**
       * its a drop off
       */
      insert(i + 1, order->delivery());
      picked_orders -= order->idx();
      m_orders_in_vehicle += order->idx();
      assigned += order->idx();
      unassigned -= order->idx();
    } else {
      /**
       * its a pickup
       */
      insert(i + 1, order->pickup());
      picked_orders += order->idx();
    }
    ++i;
  }
  evaluate();
  set_unmovable(execution_date);

#if 1
  msg().log << "\n" << this->id() << "\t (idx,id) -> ";
  for (const auto &s : stops) {
    msg().log << "(" << s << ", " << orders[s].id() << ")";
  }
#endif

  /**
   * Can not ignore user error when giving an initial solution
   */
  if (!is_feasible()) {
    msg().log << "\n**********************************Vehicle is not feasible with initial orders";
    msg().log << "twvTot " << this->twvTot() << "\tm_user_twv" << this->m_user_twv << "\n";
    msg().log << "cvTot " << this->cvTot() << "\tm_user_cv" << this->m_user_cv << "\n";
    msg().log << "\nVehicle: " << this->id() << "Orders: ";
    for (const auto &s : m_stops) msg().log << s << ",";
    msg().log << "\n";
    msg().log << "\n" << *this;



    if (optimize) {
      auto orders_to_remove = m_orders_in_vehicle;
      /**
       * Removing movable orders
       */
      for (const auto o : orders_to_remove) {
        auto order = this->orders();
        erase(this->orders()[o]);
        m_orders_in_vehicle -= o;
        assigned -= o;
        unassigned += o;
      }
    }
    this->m_user_twv = this->twvTot();
    this->m_user_cv = this->cvTot();
  }

  /**
   * check post conditions
   */
  pgassert(is_feasible());
  msg().log << "\n" << this->tau();
}

/**
 * When order's open < execution date
 * - collects the idx()
 * - removes the order idx from the m_orders_in_vehicle
 *
 * @param [in] execution_date orders starting before this time are marked as unmovable
 */

void
Vehicle_pickDeliver::set_unmovable(TTimestamp execution_date) {
  Identifiers<size_t> unmovable;
  msg().log << "\nVehicle: " << this->id() << "unmovable: {";
  for (const auto &o : m_orders_in_vehicle) {
    for (const auto &s : *this) {
      auto order = this->orders()[o];

      if (s.order() == order.id() && s.is_pickup()) {
        if (s.opens() < execution_date) {
          unmovable += o;
          msg().log << order.id() << ",";
        }
      }
    }
  }

  msg().log << "}";

  /**
   * For the optimizer is like the order does not exist
   */
  m_orders_in_vehicle -= unmovable;
}

/**
 * @returns 0 when the Vehicle is phony
 * @returns the value of the objective function
 */

double Vehicle_pickDeliver::objective() const {
  return is_phony()?
    0 :
    static_cast<double>(total_travel_time());
}

/**
 * @returns ture when the order is feasible on the vehicle
 * @param [in] order to be tested
 * @pre vehicle is empty
 */

bool
Vehicle_pickDeliver::is_order_feasible(const Order &order) const {
  auto test_truck =  *this;
  test_truck.push_back(order);
  return test_truck.is_feasible();
}

/**
 * Precondition:
 * !has_order(order)
 *
 * Postcondition:
 * has_order(order)
 * !has_cv();
 *
 * ~~~~{.c}
 * Before: S .... (P1 ....... P2) ... D2 .... D1 .... E
 *  After: S .... (P .. P1 .. P2) ... D2 .. D .. D1 .... E
 * ~~~~
 *
 * push_back is performed when
 *   - drop generates a time window violation
 *
 * Can generate time window violation
 * No capacity violation
 */

bool
Vehicle_pickDeliver::semiLIFO(const Order &order) {
  invariant();
  pgassert(!has_order(order));

  /*
   * Insert pick up as first picked
   */
  insert(1, order.pickup());

  auto deliver_pos(drop_position_limits(order.delivery()));

  /*
   * delivery generates twv in all positions
   */
  if (deliver_pos.second < deliver_pos.first) {
    /*
     * Remove inserted pickup
     */
    erase(1);
    invariant();
    return false;
  }

  pgassert(!has_order(order));
  while (deliver_pos.first <= deliver_pos.second) {
    insert(deliver_pos.second, order.delivery());

    if (is_feasible() && !at(deliver_pos.second + 1).is_pickup()) {
      /*
       * Found a position to insert the delivery
       */


      m_orders_in_vehicle += order.idx();

      /*
       * There is one more order in the vehicle
       */
      pgassert(has_order(order));
      pgassert(is_feasible());
      pgassert(!has_cv());
      pgassert(!has_twv());
      pgassert(has_order(order));
      invariant();
      return true;
    }

    /*
     * This position in path is not suitable
     */
    erase(deliver_pos.second);

    /*
     * got to next position
     */
    --deliver_pos.second;
  }

  /*
   * Order could not be inserted
   */
  erase(1);

  pgassert(!has_order(order));
  invariant();
  return false;
}

/**
 *
 * @param [in] order to be inserted
 * @pre invariant(order)
 * @pre !has_order(order)
 * @post has_order(order)
 * @post invariant(order)
 *
 * ~~~~{.c}
 * Before: S ...... E
 *  After: S ....P .... D .... E
 * ~~~~
 *
 */
bool
Vehicle_pickDeliver::hillClimb(const Order &order) {
  /**
   * check pre conditions
   */
  invariant();
  pgassert(!has_order(order));

  auto pick_pos(position_limits(order.pickup()));
  auto deliver_pos(position_limits(order.delivery()));

  if (pick_pos.second < pick_pos.first) {
    /*
     *  pickup generates twv everywhere
     */
    return false;
  }

  if (deliver_pos.second < deliver_pos.first) {
    /*
     *  delivery generates twv everywhere
     */
    return false;
  }
  /*
   * Because delivery positions were estimated without the pickup:
   *   - increase the upper limit position estimation
   */
  ++deliver_pos.first;
  ++deliver_pos.second;


  auto best_pick_pos = size();
  auto best_deliver_pos = size() + 1;
  auto current_objective(objective());
  auto min_delta_objective = (std::numeric_limits<double>::max)();

  auto found(false);

  // insert order.pickup() (that is, the order pickup node) into the earliest possible pick position
  insert(pick_pos.first, order.pickup());

  // while the pick is < latest pick
  while (pick_pos.first <= pick_pos.second) {
    auto deliver_range = deliver_pos;
    if (deliver_range.first <= pick_pos.first) deliver_range.first = pick_pos.first + 1;

    /*
     * hill climb
     *  - find the best position
     *  finds the best position based on objective.
     */
    insert(deliver_range.first, order.delivery());
    while (deliver_range.first <= deliver_range.second) {
      if (is_feasible()) {
        auto delta_objective = objective() - current_objective;
        if (delta_objective < min_delta_objective) {
          min_delta_objective = delta_objective;
          best_pick_pos = pick_pos.first;
          best_deliver_pos = deliver_range.first;
          found = true;
        }
      }
      if (at(deliver_range.first + 1).is_end()) break;
      swap(deliver_range.first, deliver_range.first + 1);
      ++deliver_range.first;
    }

    pgassert(at(deliver_range.first).order() == order.id());
    pgassert(at(pick_pos.first).order() == order.id());
    erase_node(deliver_range.first);

    if (at(pick_pos.first + 1).is_end()) break;

    swap(pick_pos.first, pick_pos.first + 1);

    ++pick_pos.first;
  }
  erase(pick_pos.first);

  if (!found) {
    /* order causes twv or cv */
    return false;
  }

  /**
   * Inserting the order
   */
  insert(best_pick_pos, order.pickup());
  insert(best_deliver_pos, order.delivery());
  m_orders_in_vehicle += order.idx();

  /**
   * check post conditions
   */
  pgassert(is_feasible());
  invariant();
  return true;
}


}  //  namespace problem
}  //  namespace vrprouting

