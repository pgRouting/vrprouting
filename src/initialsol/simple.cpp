/*PGR-GNU*****************************************************************
FILE: simple.cpp

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


#include "initialsol/simple.hpp"
#include <deque>
#include <algorithm>
#include <set>

#include "cpp_common/assert.hpp"
#include "problem/node_types.hpp"
#include "problem/orders.hpp"
#include "problem/pickDeliver.hpp"

namespace vrprouting {
namespace initialsol {
namespace simple {

void
Initial_solution::invariant() const {
    /* this checks there is no order duplicated */
    pgassert(all_orders == (assigned + unassigned));
    pgassert((assigned * unassigned).empty());
}

Initial_solution::Initial_solution(
        Initials_code kind,
        problem::PickDeliver &problem_ptr) :
    problem::Solution(problem_ptr),
    all_orders(m_orders.size()),
    unassigned(m_orders.size()),
    assigned() {
        invariant();
        pgassert(kind >= 0 && kind <= OneDepot);

        switch (kind) {
            case OneTruck:
                one_truck_all_orders();
                break;
            case OnePerTruck:
            case FrontTruck:
            case BackTruck:
            case BestInsert:
            case BestBack:
            case BestFront:
            case OneDepot:
                do_while_foo(kind);
                break;
            default: pgassert(false);
        }

    invariant();
}

void
Initial_solution::do_while_foo(int kind) {
    invariant();
    pgassert(kind > 0 && kind <= OneDepot);

    Identifiers<size_t> notused;

    while (!unassigned.empty()) {
        auto current = unassigned.size();
        auto truck = vehicles().get_truck(unassigned.front());
        /*
         * kind 1 to 7 work with the same code structure
         */
        do_while_feasible(truck, (Initials_code)kind, unassigned, assigned);
        pgassertwm(current > unassigned.size(), get_log().c_str());

        m_fleet.push_back(truck);
        invariant();
    }

    pgassertwm(true, get_log().c_str());
    invariant();
}

void
Initial_solution::one_truck_all_orders() {
    invariant();
    log << "\nInitial_solution::one_truck_all_orders\n";
    auto truck = vehicles().get_truck();
    while (!unassigned.empty()) {
        auto order(truck.orders().at(*unassigned.begin()));

        truck.hillClimb(order);

        assigned += unassigned.front();
        unassigned.pop_front();

        invariant();
    }
    m_fleet.push_back(truck);
    invariant();
}

void
Initial_solution::do_while_feasible(
        problem::Vehicle_pickDeliver& vehicle,
        Initials_code kind,
        Identifiers<size_t> &unassigned,
        Identifiers<size_t> &assigned) {
    pgassert(is_feasible());
    auto current_feasible = vehicle.feasible_orders() * unassigned;

    while (!current_feasible.empty()) {
        auto order = m_orders[current_feasible.front()];

        switch (kind) {
            case OnePerTruck:
                vehicle.push_back(order);
                pgassert(is_feasible());
                assigned += order.idx();
                unassigned -= order.idx();
                invariant();
                return;
                break;
            case FrontTruck:
                vehicle.push_front(order);
                break;
            case BackTruck:
                vehicle.push_back(order);
                break;
            case BestInsert:
                vehicle.hillClimb(order);
                break;
            case BestBack:
                order = m_orders[m_orders.find_best_J(current_feasible)];
                vehicle.hillClimb(order);
                break;
            case BestFront:
                order = m_orders[m_orders.find_best_I(current_feasible)];
                vehicle.hillClimb(order);
                break;
            case OneDepot:
                vehicle.semiLIFO(order);
                break;
            default: pgassert(false);
        }

        if (vehicle.orders_size() == 1 && !is_feasible()) {
            pgassert(false);
        }

        if (!is_feasible()) {
            vehicle.erase(order);
        } else if (vehicle.has_order(order)) {
            assigned += order.idx();
            unassigned -= order.idx();
            if (kind == BestBack) {
                current_feasible = m_orders[order.idx()].subsetJ(
                        current_feasible);
            }
            if (kind == BestFront) {
                current_feasible = m_orders[order.idx()].subsetI(
                        current_feasible);
            }
        }

        current_feasible -= order.idx();
        invariant();
    }

    pgassert(is_feasible());
    invariant();
}

}  //  namespace simple
}  //  namespace initialsol
}  //  namespace vrprouting
