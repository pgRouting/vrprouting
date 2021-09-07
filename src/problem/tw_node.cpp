/*PGR-GNU*****************************************************************

FILE: tw_node.cpp

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

#include "problem/tw_node.h"

#include <limits>
#include <string>

#include "cpp_common/pgr_assert.h"
#include "problem/matrix.h"
#include "c_types/pickDeliveryOrders_t.h"
#include "c_types/vehicle_t.h"


namespace vrprouting {
namespace problem {

const Matrix* Tw_node::m_time_matrix_ptr = nullptr;


/**
 * @param [in] other - pointer to the other Tw_node
 * @param [in] time - time of departure from previous node
 * @param [in] speed - speed used for calculation
 *
 *  @returns the travel time from @b this to @b other]
 */
TInterval
Tw_node::travel_time_to(const Tw_node &other, TTimestamp time, Speed speed) const {
  pgassert(speed != 0);
  return static_cast<TInterval>(static_cast<Speed>(m_time_matrix_ptr->travel_time(id(), other.id(), time)) / speed);
}

/**
 * @param [in] I the previous node before @b this node
 * @param [in] speed - speed used for calculation
 *
 * @returns the arrival time at @b this node after visiting @b I node when @b I opens
 *
 * I -> @b this
 * Value of TWC when arriving to @b this node after visiting node I at opening time
 */
TTimestamp
Tw_node::arrival_j_opens_i(const Tw_node &I, Speed speed) const {
  if (m_type == kStart) return (std::numeric_limits<TTimestamp>::max)();
  return I.opens() + I.service_time() + I.travel_time_to(*this, I.opens() + I.service_time(), speed);
}

/**
 * @param [in] I the previous node before @b this node
 * @param [in] speed - speed used for calculation
 *
 * @returns the arrival time at @b this node after visiting @b I node when @b I closes
 *
 * I -> @b this
 * Value of TWC when arriving to @b this node after visiting node I at closing time
 */
TTimestamp
Tw_node::arrival_j_closes_i(const Tw_node &I, Speed speed) const {
  if (m_type == kStart) return  (std::numeric_limits<TTimestamp>::max)();
  return I.closes() + I.service_time() + I.travel_time_to(*this, I.closes() + I.service_time(), speed);
}

/**
 * @param [in] I node visited before visiting @b this node
 * @param [in] speed - speed used for calculation
 * @returns true when arriving @b this node after visiting node I does not cause a time window violation
 * @returns false @b this node is a starting node (no node can be visited before a starting node)
 * @returns false I node is an ending node (no node can be visited after an ending node)
 */
bool
Tw_node::is_compatible_IJ(const Tw_node &I, Speed speed) const {
  /*
   * I /->  kStart
   */
  if (m_type == kStart) return false;

  /*
   * kEnd /-> (*this)
   */
  if (I.m_type == kEnd) return false;

  return !is_late_arrival(arrival_j_opens_i(I, speed));
}

/**
 * @param [in] I node visited before visiting @b this node
 * @param [in] speed - speed used for calculation
 * @returns true when TODO TBD
 * @returns false @b this node is TODO TBD
 * @returns false I node is TODO TBD
 */
bool
Tw_node::is_partially_compatible_IJ(const Tw_node &I, Speed speed) const {
  return
    is_compatible_IJ(I, speed)
    && !is_early_arrival(arrival_j_opens_i(I, speed))
    && is_late_arrival(arrival_j_closes_i(I, speed));
}

/**
 * @param [in] I node visited before visiting @b this node
 * @param [in] speed - speed used for calculation
 * @returns true when TODO TBD
 * @returns false @b this node is TODO TBD
 * @returns false I node is TODO TBD
 */
bool
Tw_node::is_tight_compatible_IJ(const Tw_node &I, Speed speed) const {
  return
    is_compatible_IJ(I, speed)
    && !is_early_arrival(arrival_j_opens_i(I, speed))
    && !is_late_arrival(arrival_j_closes_i(I, speed));
}

/**
 * @param [in] I node visited before visiting @b this node
 * @param [in] speed - speed used for calculation
 * @returns true when TODO TBD
 * @returns false @b this node is TODO TBD
 * @returns false I node is TODO TBD
 */
bool
Tw_node::is_partially_waitTime_compatible_IJ(
    const Tw_node &I,
    Speed speed) const {
  return
    is_compatible_IJ(I, speed)
    && is_early_arrival(arrival_j_opens_i(I, speed));
}

/**
 * @param [in] I node visited before visiting @b this node
 * @param [in] speed - speed used for calculation
 * @returns true when TODO TBD
 * @returns false @b this node is TODO TBD
 * @returns false I node is TODO TBD
 */
bool
Tw_node::is_waitTime_compatible_IJ(const Tw_node &I, Speed speed) const {
  return
    is_compatible_IJ(I, speed)
    && is_early_arrival(arrival_j_opens_i(I, speed));
}

/**
 * @returns the short hand of the kind of node
 */
std::string Tw_node::type_str() const {
  switch (type()) {
    case kStart: return "S";
    case kEnd: return "E";
    case kDump: return "D";
    case kLoad: return "L";
    case kPickup: return "P";
    case kDelivery: return "D";
    default: return "UNKNOWN";
  }
}

/**
 * @returns true when the node is a starting node and is valid
 */
bool
Tw_node::is_start() const {
  return
    m_type == kStart
    && (opens() < closes())
    && (service_time() >= 0)
    && (demand() == 0);
}

/**
 * @returns true when the node is a pickup node and is valid
 */
bool
Tw_node::is_pickup() const {
  return m_type == kPickup
    && (opens() < closes())
    && (service_time() >= 0)
    && (demand() > 0);
}

/**
 * @returns true when the node is a delivery node and is valid
 */
bool
Tw_node::is_delivery() const {
  return m_type == kDelivery
    && (opens() < closes())
    && (service_time() >= 0)
    && (demand() < 0);
}


/**
 * @returns true when the node is a dump node and is valid
 */
bool
Tw_node::is_dump() const {
  return m_type == kDump
    && (opens() < closes())
    && (service_time() >= 0)
    && (demand() <= 0);
}


/**
 * @returns true when the node is a load node and is valid
 */
bool
Tw_node::is_load() const {
  return m_type == kLoad
    && (opens() < closes())
    && (service_time() >= 0)
    && (demand() >= 0);
}

/**
 * @returns true when the node is an end node and is valid
 */
bool
Tw_node::is_end() const {
  return m_type == kEnd
    && (opens() < closes())
    && (service_time() >= 0)
    && (demand() == 0);
}


/**
  @returns true when the other node is the same as @b this node
  @param [in] other
  */
bool
Tw_node::operator ==(const Tw_node &other) const {
  if (&other == this) return true;
  return m_order == other.m_order
    && m_opens == other.m_opens
    && m_closes == other.m_closes
    && m_service_time == other.m_service_time
    && m_demand == other.m_demand
    && m_type == other.m_type
    && id() == other.id()
    && idx() == other.idx();
}


/**
  @returns true when the node depending on the kind is a valid node
  */
bool Tw_node::is_valid() const {
  switch (type()) {
    case kStart:
      return is_start();
      break;

    case kEnd:
      return is_end();
      break;

    case kDump:
      return is_dump();
      break;

    case kDelivery:
      return is_delivery();
      break;

    case kPickup:
      return is_pickup();
      break;

    case kLoad:
      return is_load();
      break;

    default:
      return false;
      break;
  }

  return false;
}

/**
  @param [in] id the internal id of the node
  @param [in] data the postgrSQL order information
  @param [in] type the kind of node to be created
  */
Tw_node::Tw_node(
    size_t id,
    const PickDeliveryOrders_t &data,
    const NodeType &type) :
  Identifier(id, data.pick_node_id),
  m_order(data.id),
  m_opens(data.pick_open_t),
  m_closes(data.pick_close_t),
  m_service_time(data.pick_service_t),
  m_demand(data.demand),
  m_type(type)  {
    if (m_type == kDelivery) {
      reset_id(data.deliver_node_id);
      m_opens = data.deliver_open_t;
      m_closes = data.deliver_close_t;
      m_service_time = data.deliver_service_t;
      m_demand *= -1;
    }
  }

/**
  @param [in] id the internal id of the node
  @param [in] data the postgrSQL vehicle information
  @param [in] type the kind of node to be created
  */
Tw_node::Tw_node(
    size_t id,
    const Vehicle_t &data,
    const NodeType &type) :
  Identifier(id, data.start_node_id),
  m_opens(data.start_open_t),
  m_closes(data.start_close_t),
  m_service_time(data.start_service_t),
  m_demand(0),
  m_type(type) {
    if (m_type == kEnd) {
      reset_id(data.end_node_id);
      m_opens = data.end_open_t;
      m_closes = data.end_close_t;
      m_service_time = data.end_service_t;
    }
  }


/**
  @returns the printed information of the node
  @param [in] log
  @param [in] n
  */
std::ostream& operator << (std::ostream &log, const Tw_node &n) {
  log << n.id()
    << "[opens = " << n.m_opens
    << "\tcloses = " << n.m_closes
    << "\tservice = " << n.m_service_time
    << "\tdemand = " << n.m_demand
    << "\ttype = " << n.type_str()
    << "]"
    << "\n";
  return log;
}

}  //  namespace problem
}  //  namespace vrprouting

