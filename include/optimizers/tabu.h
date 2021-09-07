/*PGR-GNU*****************************************************************

FILE: optimize.h

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

#ifndef INCLUDE_OPTIMIZERS_TABU_H_
#define INCLUDE_OPTIMIZERS_TABU_H_
#pragma once

#include "problem/solution.h"
#include "optimizers/tabu_list.h"

namespace vrprouting {
namespace optimizers {
namespace tabu {

/** @brief Class that optimizes a solution
 *
 * How to use:
 *
 * ~~~~{.c}
 * // Given a solution in solution_to_optimize
 * // in this example it is one cycle and to stop when all orders are served
 *
 * Solution optimized_solution = Optimize(solution_to_optimize, 1, true);
 * ~~~~
 *
 */
class Optimize : public problem::Solution {
 public:
    /** @brief Optimization operation */
    Optimize(const problem::Solution& solution, size_t times, bool stop_on_all_served, bool);

    /** The best solution so far */
    problem::Solution best_solution;

 private:
    /** Tabu lists of the problem */
    TabuList tabu_list;

    /** @brief Main optimization controller */
    void tabu_search();

    void sort_by_size(bool asc);

    /** @brief set tabu length **/
    void set_tabu_list_length();

    /** @brief deletes phony vehicles that are empty */
    void delete_empty_truck();

    /** @brief moves orders from phony vehicles to a real vehicle */
    bool move_2_real();

    /** @brief Single Pair Insertion: move order from one vehicle to another that reduces objective value */
    bool single_pair_insertion(bool intensify, bool diversify);

    /** @brief Swapping pairs Between Routes: initiate directed or undirected swaps of orders between vehicles. */
    bool swap_between_routes(bool intensify, bool diversify);

    /** @brief save a new found solution only if it is best */
    void save_if_best();

    /** Maximum number of cycles on the optimization process */
    size_t m_max_cycles;

    /** Flag to stop when all orders are in real vehicles */
    bool m_stop_on_all_served;

    /** Flag to just build a solution and not optimize it */
    bool m_optimize;

    /** Limit for a cycle */
    size_t m_standard_limit = 30;

    /** Intensify the search */
    void intensify();

    /** Diversify the search */
    bool diversify();

    /** Set of unassigned orders */
    Identifiers<size_t> m_unassignedOrders;
};

}  //  namespace tabu
}  //  namespace optimizers
}  //  namespace vrprouting

#endif  // INCLUDE_OPTIMIZERS_TABU_H_
