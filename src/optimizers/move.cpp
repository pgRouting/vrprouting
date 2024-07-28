/*PGR-GNU*****************************************************************

FILE: move.cpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2021 Joseph Emile Honour Percival


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

#include "optimizers/move.h"
#include <iostream>
#include "problem/vehicle_pickDeliver.hpp"
#include "problem/order.hpp"

namespace vrprouting {
namespace optimizers {
namespace tabu {

Move::Move(const problem::Vehicle_pickDeliver& from_v, const problem::Vehicle_pickDeliver& to_v,
    const problem::Order& order, double to_obj, double from_obj) :
  m_vid1(from_v.id()),
  m_vid2(to_v.id()),
  m_oid1(order.id()),
  m_oid2(0),
  m_is_swap(false),
  m_obj1(to_obj),
  m_obj2(from_obj) { }

Move::Move(const problem::Vehicle_pickDeliver& from_v, const problem::Vehicle_pickDeliver& to_v,
    const problem::Order& from_order, const problem::Order& to_order, double to_obj, double from_obj) :
  m_vid1(from_v.id()),
  m_vid2(to_v.id()),
  m_oid1(from_order.id()),
  m_oid2(to_order.id()),
  m_is_swap(true),
  m_obj1(to_obj),
  m_obj2(from_obj) { }

/**
 * @param [in,out] log - where to write
 * @param [in] m - move to write
 *
 * @returns log with move information
 */
std::ostream&
  operator << (std::ostream &log, const Move &m) {
    log << m.m_oid1 << ":" << m.m_oid2 << ":"
      << m.m_vid1 << ":" << m.m_vid2 << ":"
      << m.m_is_swap;
    return log;
  }

/**
 * @param [in] a - left hand side move
 * @param [in] b - right hand side move
 *
 * @returns true when the left hand side move move is the same as the right hand side move
 */
bool
  operator==(const Move& a, const Move& b) {
    /*
     * possible cases are captured in TabuList::has_move and TabuList::has_swap:
     */
    return (
        a.oid1() == b.oid1()
        && a.oid2() == b.oid2()
        && a.vid1() == b.vid1()
        && a.vid2() == b.vid2()
#if 0
        && a.to_objective() == b.to_objective()
        && a.from_objective() == b.from_objective()
#endif
        && a.is_swap() == b.is_swap());
  }

}  //  namespace tabu
}  //  namespace optimizers
}  //  namespace vrprouting
