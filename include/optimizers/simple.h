/*PGR-GNU*****************************************************************

FILE: optimize.cpp

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

#ifndef INCLUDE_OPTIMIZERS_SIMPLE_H_
#define INCLUDE_OPTIMIZERS_SIMPLE_H_
#pragma once

#include "problem/solution.hpp"
#include "problem/vehicle_pickDeliver.hpp"
#include "initialsol/initials_code.hpp"

namespace vrprouting {
namespace optimizers {
namespace simple {

class Optimize : public problem::Solution {
    using Initials_code = initialsol::simple::Initials_code;

 public:
    Optimize(const problem::Solution &solution, size_t times, const Initials_code&);

    /* @brief decrease_truck
     *
     * Optimization by decreasing trucks
     */
    void decrease_truck();
    void move_wait_time_based();
    void move_duration_based();
    void inter_swap(size_t times);
    Solution best_solution;

    Initials_code get_kind() const {return m_kind;}

 private:
    bool decrease_truck(size_t);
    void sort_for_move();
    void sort_by_duration();
    void sort_by_size();
    void sort_by_id();
    void delete_empty_truck();

    bool swap_worse(problem::Vehicle_pickDeliver &from, problem::Vehicle_pickDeliver &to);
    bool move_reduce_cost(problem::Vehicle_pickDeliver &from, problem::Vehicle_pickDeliver &to);
    bool inter_swap();

    // TODO(pending) use const & where applicable
    bool move_order(
        problem::Order order,
        problem::Vehicle_pickDeliver &from_truck,
        problem::Vehicle_pickDeliver &to_truck);
    bool swap_order();
    bool swap_order(
        problem::Order from_order,
        problem::Vehicle_pickDeliver &from_truck,
        problem::Order to_order,
        problem::Vehicle_pickDeliver &to_truck);
    void save_if_best();

    Initials_code m_kind;
};

}  //  namespace simple
}  //  namespace optimizers
}  //  namespace vrprouting

#endif  // INCLUDE_OPTIMIZERS_SIMPLE_H_
