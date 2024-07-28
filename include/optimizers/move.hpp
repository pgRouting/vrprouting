/*PGR-GNU*****************************************************************

FILE: move.hpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Created by Joseph Percival on 2020/06/15.
Modified by Vicky
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

#ifndef INCLUDE_OPTIMIZERS_MOVE_HPP_
#define INCLUDE_OPTIMIZERS_MOVE_HPP_
#pragma once

#include <cstdint>
#include <iosfwd>

namespace vrprouting {
namespace problem {
class Order;
class Vehicle_pickDeliver;
}

namespace optimizers {
namespace tabu {


/** @brief The Move class defines moves that are evaluated and set as tabu. */
class Move {
 public:
    /** @brief empty move is not allowed */
    Move() = delete;

    /** @brief The Move constructor for a "move" move */
    Move(const problem::Vehicle_pickDeliver&, const problem::Vehicle_pickDeliver&,
         const problem::Order&, double, double);

    /** @brief The Move constructor for a "swap" move */
    Move(const problem::Vehicle_pickDeliver&, const problem::Vehicle_pickDeliver&,
         const problem::Order&, const problem::Order&, double, double);

    /** @brief The identifier of the order to move or swap*/
    int64_t oid1() const {return m_oid1;}

    /**@brief The identifier of the order to swap */
    int64_t oid2() const {return m_oid2;}

    /** @brief The identifier of the vehicle to move the order to*/
    int64_t vid1() const {return m_vid1;}

    /** @brief The identifier of the vehicle to move the order to*/
    int64_t vid2() const {return m_vid2;}

    /** @brief is the move a swap move? */
    bool is_swap() const {return m_is_swap;}

    /** @brief Objective value of the "to" vehicle. Currently unused. */
    double to_objective() const {return m_obj1;}

    /** @brief Objective value of the "from" vehicle. Currently unused. */
    double from_objective() const {return m_obj2;}

    /** @brief printing function */
    friend std::ostream& operator<<(std::ostream &, const Move &);

    /** @brief equality operator */
    friend bool operator==(const Move&, const Move&);

 private:
    /** The identifier of the vehicle to move the order from */
    int64_t m_vid1;

    /** The identifier of the vehicle to move the order to */
    int64_t m_vid2;

    /** The identifier of the first moved order */
    int64_t m_oid1;

    /** The identifier of the second moved order (valid only for swap moves) */
    int64_t m_oid2;

    /** flag == true when belongs to swap */
    bool m_is_swap;

    /** The objective value of the vehicle to move the order to */
    double m_obj1;

    /** The objective value of the vehicle to move the order from */
    double m_obj2;
};

}  //  namespace tabu
}  //  namespace optimizers
}  //  namespace vrprouting

#endif  // INCLUDE_OPTIMIZERS_MOVE_HPP_

