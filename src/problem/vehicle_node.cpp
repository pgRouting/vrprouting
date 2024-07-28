/*PGR-GNU*****************************************************************

FILE: vehicle_node.cpp

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


#include "problem/vehicle_node.hpp"
#include "cpp_common/assert.hpp"
#include "c_types/solution_rt.h"


namespace vrprouting {
namespace problem {

/**
@returns the value of the objective function for this node
*/
double
Vehicle_node::objective() const {
  return static_cast<double>(travel_time());
}

/**
 * @param[in] cargoLimit of the vehicle
 *
 * @pre node is a starting node
 *
 * - evaluates the node
 * - starts the cumulative information
 */
void
Vehicle_node::evaluate(PAmount cargoLimit) {
  pgassert(is_start());
  /* time */
  m_travel_time = 0;
  m_arrival_time = opens();
  m_wait_time = 0;
  m_departure_time = arrival_time() + service_time();

  /* time aggregates */
  m_tot_travel_time = 0;
  m_tot_wait_time = 0;
  m_tot_service_time = service_time();

  /* cargo aggregates */
  m_cargo = demand();

  /* violation aggregates */
  m_twvTot = m_cvTot = 0;
  m_cvTot = has_cv(cargoLimit) ? 1 : 0;
  m_delta_time = 0;
}


/**
  @param[in] pred The node preceding this node in the path.
  @param[in] cargoLimit of the vehicle.
  @param[in] speed value to use for calculations
  */
void
Vehicle_node::evaluate(
    const Vehicle_node &pred,
    PAmount cargoLimit,
    Speed speed
    ) {
  /* time */
  m_travel_time    = pred.travel_time_to(*this, pred.departure_time(), speed);
  m_arrival_time   = pred.departure_time() + travel_time();
  m_wait_time      = is_early_arrival(arrival_time()) ?
    opens() - m_arrival_time :
    0;
  m_departure_time = arrival_time() + wait_time() + service_time();

  /* time aggregates */
  m_tot_travel_time = pred.total_travel_time() + travel_time();
  m_tot_wait_time    = pred.total_wait_time()    + wait_time();
  m_tot_service_time = pred.total_service_time() + service_time();

  /* cargo aggregates */
  if (is_dump() &&  pred.cargo() >= 0) {
    demand(-pred.cargo());
  }
  m_cargo = pred.cargo() + demand();

  /* violations aggregates */

  m_twvTot = has_twv() ? pred.twvTot() + 1 : pred.twvTot();
  m_cvTot = has_cv(cargoLimit) ? pred.cvTot() + 1 : pred.cvTot();
  m_delta_time = departure_time() - pred.departure_time();
}


/**
 * @param [in,out] log Place to store the printed status of the node
 * @param [in] v Vehicle node to print
 * @returns the new string
 */
std::ostream&
operator << (std::ostream &log, const Vehicle_node &v) {
  log << static_cast<const Tw_node&>(v)
    << " twv = " << v.has_twv()
    << ", twvTot = " << v.twvTot()
    << ", cvTot = " << v.cvTot()
    << ", cargo = " << v.cargo()
    << ", travel_time = " << v.travel_time()
    << ", arrival_time = " << v.arrival_time()
    << ", wait_time = " << v.wait_time()
    << ", service_time = " << v.service_time()
    << ", departure_time = " << v.departure_time();
  return log;
}


/**
 *  Creates a disconnected vehicle node
 *  A node that is not served by any vehicle
 *
 * @param[in] node Time window node
 */

Vehicle_node::Vehicle_node(const Tw_node &node)
  : Tw_node(node),
  m_travel_time(0),
  m_arrival_time(0),
  m_wait_time(0),
  m_departure_time(0),
  m_delta_time(0),
  m_cargo(0),
  m_twvTot(0),
  m_cvTot(0),
  m_tot_wait_time(0),
  m_tot_travel_time(0),
  m_tot_service_time(0) {
  }


bool
Vehicle_node::deltaGeneratesTWV(TInterval delta_time) const {
  return is_late_arrival(m_arrival_time + delta_time);
}


/**
 * @param[in] cargoLimit of the vehicle
 * @returns true when the node violates the capacity constraints
 *
 * A capacity violations happens when:
 * - the node is a start or end node and the cargo is not 0
 * - the node's cargo is greater than the limit or is negative
 */
bool
Vehicle_node::has_cv(PAmount cargoLimit) const {
  return is_end() ||  is_start() ? m_cargo != 0
    : m_cargo > cargoLimit ||  m_cargo < 0;
}


/**
 *  @returns true when the arrival happens after the closing time
 */
bool
Vehicle_node::has_twv() const {
  return is_late_arrival(m_arrival_time);
}

/**
 * @returns the vehicle's path in a structure for postgres
 * @param [in] vid it   the vid-th vehicle in the solution
 * @param [in] v_id it  the vehicle identifier of the current node
 * @param [in] stop_seq using this sequence value for the current node
 */
Solution_rt
Vehicle_node::get_postgres_result(int vid, int64_t v_id, int stop_seq) const {
  return Solution_rt{
    vid,
      v_id,
      stop_seq,
      /* order_id
       * The order_id is invalid for stops type 0 and 5
       */
      (type() == 0 || type() == 5)? -1 : order(),
      id(),
      type(),
      static_cast<int64_t>(cargo()),
      static_cast<int64_t>(travel_time()),
      static_cast<int64_t>(arrival_time()),
      static_cast<int64_t>(wait_time()),
      static_cast<int64_t>(arrival_time() + wait_time()),
      static_cast<int64_t>(service_time()),
      static_cast<int64_t>(departure_time()),
      cvTot(),
      twvTot()};
}

}  //  namespace problem
}  //  namespace vrprouting
