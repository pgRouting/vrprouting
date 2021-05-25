/*PGR-GNU*****************************************************************
FILE: tabu.cpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2021 Copyright (c) 2021 Joseph Emile Honour Percival

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


#include "initialsol/tabu.h"
#include <deque>
#include <algorithm>
#include <limits>

#include "cpp_common/pgr_assert.h"
#include "cpp_common/pgr_messages.h"
#include "problem/pickDeliver.h"
#if 0
#include "cpp_common/fleet.h"
#endif

namespace vrprouting {
namespace initialsol {
namespace tabu {

/**
@invariant assigned UNION unassigned = all_orders
@invariant assigned INTERSECTION unassigned = empty
*/
void
Initial_solution::invariant() const {
    pgassert(m_all_orders == (m_assigned + m_unassigned));
    pgassert((m_assigned * m_unassigned).empty());
}

/**
@param [in] execution_date used for setting unmovable orders from the past
@param [in] optimize prepare orders for optimization
@param [in] problem_ptr the problem pointer
*/
Initial_solution::Initial_solution(
        TTimestamp execution_date,
        bool optimize,
        problem::PickDeliver* problem_ptr) :
    Solution(problem_ptr),
    m_all_orders(),
    m_unassigned(),
    m_assigned() {
        invariant();

        /**
         * Get all the orders
         * - when an order is feasible on at least one vehicles is part of the orders set
         * - unfeasible orders are ignored
         */
        m_all_orders = vehicles().feasible_orders();
        m_unassigned = m_all_orders;

        process_given_solution_from_user(execution_date, optimize);
        pgassert(is_feasible());

        /**
         * From the of used vehicles
         * When there are unassigned orders
         * - mark as unmovable all orders on vehicles where the unassigned orders
         *   are not feasible
         */
        if (!m_unassigned.empty()) {
            for (auto &v : m_fleet) {
                bool is_usable(false);
                for (const auto &o : m_unassigned) {
                    if (v.feasible_orders().has(o)) {
                        /*
                         * found an order to be inserted that fits on the vehicle
                         */
                        is_usable = true;
                        break;
                    }
                }
                /*
                 * No order to be inserted fits on the vehicle
                 */
                if (!is_usable) v.set_unmovable(std::numeric_limits<TTimestamp>::max());
            }
        }

        /**
         * Get the remaining empty unused vehicles
         */
        auto unused = vehicles().get_unused_trucks();
        m_fleet.insert(m_fleet.end(), unused.begin(), unused.end());
        pgassert(is_feasible());

        /**
         * add the unassigned orders to phony vehicles
         */
        process_unassigned();
        pgassert(is_feasible());

        invariant();
    }

/**
 * User's initial solution is considered
 * @pre m_fleet is empty
 * @pre invariant()
 * @pre m_fleet has vehicles that have orders
 * @post invariant()
 * @post is feasible
 */
void
Initial_solution::process_given_solution_from_user(TTimestamp execution_date, bool optimize) {
    pgassert(m_fleet.empty());
    invariant();

    /**
     * Set the initial solution given by the user
     */
    vehicles().set_initial_solution(orders(), m_assigned, m_unassigned, execution_date, optimize);

    /**
     * Used vehicles: are the ones that have a solution from the user
     */
    auto used_v = vehicles().get_used_trucks();

    /**
     * The solution fleet are the used vehicles
     */
    m_fleet.insert(m_fleet.end(), used_v.begin(), used_v.end());

    pgassert(is_feasible());
    invariant();
}

/**
@pre invariant()
@pre is_feasible()
@post invariant()
@post is_feasible()
@post unassigned is empty
*/
void
Initial_solution::process_unassigned() {
    /**
     * test pre conditions
     */
    invariant();
    pgassert(is_feasible());

    Identifiers<size_t> notused;

    auto not_assigned = m_unassigned;

    /**
     * cycle the unassigned orders
     */
    for (const auto o : not_assigned) {
        /**
         * - get a new phony vehicle
         */
        auto phony_v = vehicles().get_phony();
        if (!phony_v.is_order_feasible(orders()[o])) {
            phony_v.push_back(orders()[o]);
            m_unassigned -= o;
            m_all_orders -= o;
            msg().error << "\n**Illegal Order** pick.opens() + tt > drop.closes() can not be inserted on any vehicle";
            continue;
        }

        /**
         * - Add the order to the phony vehicle
         */
        phony_v.push_back(orders()[o]);
        pgassert(phony_v.is_feasible());

        /**
         * - update for invariant
         */
        m_assigned += o;
        m_unassigned -= o;

        /**
         * - Add the vehicle to the fleet
         */
        m_fleet.push_back(phony_v);
        invariant();
    }

    /**
     * test post conditions
     */
    pgassert(m_unassigned.empty());
    pgassert(is_feasible());
    invariant();
}

}  //  namespace tabu
}  //  namespace initialsol
}  //  namespace vrprouting
