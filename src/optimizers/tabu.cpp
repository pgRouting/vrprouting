/*PGR-GNU*****************************************************************

FILE: tabu.cpp

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

#include "optimizers/tabu.hpp"

#include <algorithm>
#include <string>
#include <deque>

#include "cpp_common/assert.hpp"
#include "cpp_common/messages.hpp"

#include "optimizers/move.hpp"

/**
 * Anonymus namespace for static functions
 */
namespace {

/** @brief set of unassigned orders
 */
Identifiers<size_t>
set_unassignedOrders(const std::deque<vrprouting::problem::Vehicle_pickDeliver> &fleet) {
    Identifiers<size_t> unassigned_orders;
    for (const auto &v : fleet) {
        if (v.is_phony()) unassigned_orders += v.orders_in_vehicle();
    }
    return unassigned_orders;
}

/** @brief Are all orders served?
 *
 * @returns false when a phony vehicle has an order
 * @returns true when all phony vehicles have no orders
 * @param [in] unassigned
 */
bool
are_all_served(const Identifiers<size_t> &unassigned) {
    return unassigned.empty();
}
}  // namespace

namespace vrprouting {
namespace optimizers {
namespace tabu {

/**
 *
@dot
digraph G {
  node[fontsize=11, nodesep=0.75,ranksep=0.75];  start  [shape=Mdiamond];
  n1  [label="tabu search\n(optimization)",shape=cds,color=blue];
  n2  [label="Save best fleet/order structure found",shape=rect];
  n3  [label="delete empty phony trucks",shape=rect];
  n4  [label="Order results",shape=rect];
  end  [shape=Mdiamond];
  error [shape=Mdiamond,color=red]
  start -> n1 -> n2 -> n3 -> n4 -> end;
  a [label="assertion",color=red];
  a -> error [color=red,label="fail",fontcolor=red];
}
@enddot
 *
 * @param [in] old_solution - solution to be optimized
 * @param [in] max_cycles - number of times to perform a single tabu search (optimization) cycle
 * @param [in] stop_on_all_served - a stopping condition: stop when all orders are served
 * @param [in] optimize - a stopping condition when @b false: only add orders; do not optimize
 * @post this solution's fleet has the best solution found
 */
Optimize::Optimize(
        const problem::Solution &old_solution,
        size_t max_cycles,
        bool stop_on_all_served,
        bool optimize) :
        problem::Solution(old_solution),
        best_solution(old_solution),
        m_max_cycles(max_cycles),
        m_stop_on_all_served(stop_on_all_served),
        m_optimize(optimize) {
    ENTERING(log);

    /*
     * this function does the actual work
     */
    tabu_search();

    /*
     * delete empty phony trucks
     */
    delete_empty_truck();

    /*
     * Save best fleet/order structure found
     */
    this->m_fleet = best_solution.fleet();

    log << tau("Best solution found");
    EXITING(log);
}

/**
 * move_2_real
 *
 * - moving orders from a phony vehicle to a real vehicle
 * - vehicles can be unordered
 * - returns true if unassigned orders were assigned to a (real) vehicle
 */
bool
Optimize::move_2_real() {
    ENTERING(log);
    /*
     * nothing to do:
     * - No orders pending on phony vehicle
     */
    if (are_all_served(m_unassignedOrders)) return false;
    pgassert(!are_all_served(m_unassignedOrders));

    bool moves_were_done(false);

    /*
     * cycling to find phony vehicles
     */
    for (auto &phony_vehicle : m_fleet) {
        /*
         * skip
         * - real vehicle
         * - empty vehicle
         */
        if (phony_vehicle.is_real() || phony_vehicle.empty()) continue;
        pgassert(phony_vehicle.is_phony() && !phony_vehicle.empty());


        auto orders_in_phony_vehicle = phony_vehicle.orders_in_vehicle();
        for (const auto o_id : orders_in_phony_vehicle) {
            /*
             * get the order to be inserted on a real vehicle
             */
            double best_score = 0;
            auto order = orders()[o_id];

            auto best_vehicle_ref = &phony_vehicle;

            for (auto &real_vehicle : m_fleet) {
                /*
                 * skip when:
                 *  - destination vehicle is the current vehicle
                 *  - destination vehicle is phony
                 *  - When current order is not feasible on real vehicle
                 */
                if (&phony_vehicle == &real_vehicle) continue;
                if (real_vehicle.is_phony()) continue;
                if (!real_vehicle.feasible_orders().has(o_id)) continue;
                if (real_vehicle.has_order(order)) continue;

                pgassert(&phony_vehicle != &real_vehicle);
                pgassert(real_vehicle.is_real());

                /*
                 * setup local working vehicles
                 */
                auto real_vehicle_copy = real_vehicle;

                /* current travel_time */
                auto curr_travel_time = phony_vehicle.total_travel_time() + real_vehicle_copy.total_travel_time();

                /*
                 * insert order on destination vehicle
                 */
                pgassert(!real_vehicle_copy.has_order(order));
                real_vehicle_copy.hillClimb(order);

                // either the to vehicle has the and is feasible OR it does not have the order
                pgassert((real_vehicle_copy.has_order(order) && real_vehicle_copy.is_feasible())
                        || !real_vehicle_copy.has_order(order));

                /*
                 * Skip to next order if the order wasnt inserted
                 */
                if (!real_vehicle_copy.has_order(order)) continue;
                pgassert(real_vehicle_copy.has_order(order));

                auto new_travel_time = phony_vehicle.total_travel_time() + real_vehicle_copy.total_travel_time();
                auto delta_travel_time = new_travel_time - curr_travel_time;

                auto delta_objective = delta_travel_time;
                auto estimated_objective = objective() + static_cast<double>(delta_objective);


                if (best_score == 0 || estimated_objective < best_score) {
                    best_score = estimated_objective;
                    best_vehicle_ref = &real_vehicle;
                }
            }   // to_v

            if (best_score != 0) {
                pgassert(best_vehicle_ref != &phony_vehicle);
                phony_vehicle.erase(order);
                best_vehicle_ref->hillClimb(order);
                m_unassignedOrders -= order.idx();
                best_solution = (*this);
                moves_were_done = true;
            }
        }  // orders
    }  // from phony
    EXITING(log);
    if (moves_were_done) tabu_list.clear();
    return moves_were_done;
}

/**
 * set_tabu_list_length
 *
 * - Sets the tabu list max length
 */
void
Optimize::set_tabu_list_length() {
    auto max_length = std::max(orders().size(), m_standard_limit);
    tabu_list.set_max_length(max_length);
}


/**
 * tabu_search will find the best solution. The best solution is one that either:
 * 1. adds an order
 * 2. meets the evaluation criteria.
 * It searches until either (terminating conditions):
 * 1. all cycles are complete
 * 2. all possible orders are served without optimization and optimize is set to false
 * 3. all orders are served and stop_on_all_served is set to true
 * The stopping condition stop_on_all_served exists in order to give a fast response to the user.
 * Note:
 * - Phony vehicles (if exist) have orders that have not been served
 *
 * The vehicle sort results on:
 * [pv1, pv2, pv3, ... pvK, rv1 rv2 ... rvN]
 * Phony vehicles (if exist) have orders that have not been served
 * where:
 * - [pv1, pv2, ... pvK] are phony vehicles
 * - [rv1 ... rvN] are real vehicles
 * And within the real vehicles
 * for i < j : rv(i).size < rv(j).size
 *
 * Loop through cycles of tabu search
 * Exit the loop when:
 * - all orders are served if stop_on_all_served
 * - maximum number of cycles is reached
 */
void
Optimize::tabu_search() {
    ENTERING(log);

    sort_by_size(true);

    m_unassignedOrders = set_unassignedOrders(m_fleet);

    move_2_real();
    delete_empty_truck();
    /*
     * stop if no optimization is asked
     */
    if (!m_optimize) return;

    /*
     * stop if all orders are served
     */
    if (m_stop_on_all_served && are_all_served(m_unassignedOrders)) return;

    set_tabu_list_length();
    /*
     * Do the cycles
     */
    /*
     * initialize the iterator counter
     */
    size_t iter = 0;

    int no_moves = 0;

    int max_no_moves = 2;

    int max_no_swaps = 2;

    bool diversification = true;

    bool intensification = false;

    bool do_spi = true;

    [[maybe_unused]] int could_not_spi = 0;

    int stuck_counter = 0;

    [[maybe_unused]] int wander_counter = 0;

    int max_no_improvement = 1000;

    std::string neighborhood;

    int wander_length = 100;

    while (iter < m_max_cycles) {
        double curr_best = best_solution.objective();

        if (stuck_counter == max_no_improvement) {
            intensify();
            if (best_solution.objective() == curr_best)
                break;
        }

        if (no_moves < max_no_moves && do_spi) {
            neighborhood = "spi";
        } else {
            neighborhood = "sbr";
        }

        bool moved;

        if (neighborhood == "spi") {
            moved = single_pair_insertion(false, false);
            delete_empty_truck();
            if (moved) {
                if (!are_all_served(m_unassignedOrders)) move_2_real();
            }
        } else {
            moved = swap_between_routes(false, false);
            if (moved) {
                if (!are_all_served(m_unassignedOrders)) move_2_real();
            }
        }

        if (m_stop_on_all_served && are_all_served(m_unassignedOrders)) break;

        if (!moved) {
            no_moves += 1;
            if (neighborhood == "spi") {
                could_not_spi += 1;
            }
            if (neighborhood == "sbr") {
                intensify();
            }
        } else {
            no_moves = 0;
        }

        if (no_moves >= max_no_swaps + max_no_moves) break;

        if (m_stop_on_all_served && are_all_served(m_unassignedOrders)) break;

        /*
         * check for aimless wandering ...
         */
        if (curr_best == best_solution.objective()) {
            stuck_counter += 1;
            wander_counter += 1;
            if (stuck_counter % wander_length == 0) {
                intensification = !intensification;
                diversification = !diversification;

                if (intensification) {
                    intensify();
                }
                if (diversification) {
                    diversify();
                }
            }
        } else {
            // if it is not the same then it must be better!
            stuck_counter = 0;
        }
        iter++;
    }
    EXITING(log);
}


/**
 * intensify
 *
 * - Intensifies the search by returning to the current best solution and only focusing on moves that improve the
 *  current best solution.
 */
void
Optimize::intensify() {
    m_fleet = best_solution.fleet();

    bool do_spi = true;
    bool do_sbr = true;

    int max_iter = 20;
    int i = 0;
    while (do_spi && i < max_iter) {
        do_spi = single_pair_insertion(true, false);
        if (do_spi) if (!are_all_served(m_unassignedOrders)) move_2_real();
        i++;
    }
    i = 0;
    while (do_sbr && i < max_iter) {
        do_sbr = swap_between_routes(true, false);
        if (do_spi) if (!are_all_served(m_unassignedOrders)) move_2_real();
        i++;
    }
}


/**
 * diversify
 *
 * - diversifies the search by moving into a solution space that has never been visited before
 * - returns true if diversification was successful.
 */
bool
Optimize::diversify() {
    /*
     * set the number of maximum swaps between routes for diversification
     */
    size_t max_swaps = std::min(m_fleet.size(), orders().size() / 10);
    size_t s = 0;
    bool swaps_were_done = true;
    while (swaps_were_done && s < max_swaps) {
        swaps_were_done = single_pair_insertion(false, true);
        if (!swaps_were_done) {
            swaps_were_done = swap_between_routes(false, true);
        }
        if (!swaps_were_done) {
            swaps_were_done = swap_between_routes(false, false);
        }
        if (swaps_were_done) if (!are_all_served(m_unassignedOrders)) move_2_real();
        s++;
    }
    return swaps_were_done;
}



/** @brief Single Pair Insertion
 *
 * @returns false when a single pair insertion was not successful
 * @returns true when a single pair insertion was successful
 * @param [in] intensify to declare an intensification phase single pair insertion
 * @param [in] diversify to declare an diversification phase single pair insertion
 */
bool
Optimize::single_pair_insertion(bool intensify, bool diversify) {
    sort_by_size(true);
    auto best_to_v = &m_fleet[0];
    auto best_from_v = &m_fleet[0];
    double best_score = intensify ? objective() : 0;
    double best_to_score;
    double best_from_score;
    bool moved = false;
    auto best_order = orders()[0];
    bool has_phony = false;

    std::string best_candidate;

    for (auto &from_vehicle : m_fleet) {
        if (from_vehicle.is_phony() || from_vehicle.empty()) continue;

        auto first_order_set = from_vehicle.orders_in_vehicle();
        for (const auto o_id : first_order_set) {
            auto order = orders()[o_id];
            for (auto &to_v : m_fleet) {
                if (&from_vehicle == &to_v) continue;
                if (to_v.is_phony()) {
                    has_phony = true;
                }
                if (to_v.is_phony()) continue;
                if (!has_phony && to_v.empty()) continue;

                if (!to_v.feasible_orders().has(o_id)) continue;

                std::ostringstream ss;

                ss << to_v.path_str() << ":" << o_id;

                std::string candidate = ss.str();

                if (tabu_list.has_infeasible(candidate)) continue;

                if (diversify && tabu_list.has_seen(candidate)) continue;

                /*
                 * setup local working vehicles
                 */
                auto to_v_copy = to_v;
                auto from_v_copy = from_vehicle;

                /* current travel_time */
                auto curr_travel_time = from_v_copy.total_travel_time() + to_v_copy.total_travel_time();

                /*
                 * insert order on destination vehicle
                 */
                pgassert(!to_v_copy.has_order(order));
                to_v_copy.hillClimb(order);

                // either the to vehicle has the order and is feasible OR it does not have the order
                pgassert((to_v_copy.has_order(order) && to_v_copy.is_feasible()) || !to_v_copy.has_order(order));

                /*
                 * Skip to next order if the order wasn't inserted
                 */
                if (!to_v_copy.has_order(order)) {
                    tabu_list.add_infeasible(candidate);
                    continue;
                }

                from_v_copy.erase(order);
                if (from_v_copy.has_order(order) || !from_v_copy.is_feasible()) continue;

                auto new_travel_time = from_v_copy.total_travel_time() + to_v_copy.total_travel_time();
                auto delta_travel_time = new_travel_time - curr_travel_time;

                auto delta_objective = delta_travel_time;
                auto estimated_objective = objective() + static_cast<double>(delta_objective);

                /*
                 * evaluate the move
                 */
                if (estimated_objective >= best_score && !from_v_copy.empty() && best_score != 0) continue;

                if (tabu_list.has_move(
                            from_v_copy, to_v_copy, order, to_v_copy.objective(),
                            from_v_copy.objective())
                        && estimated_objective >= best_solution.objective()
                        && !from_v_copy.empty()) continue;

                if (estimated_objective < best_score || from_v_copy.empty() || best_score == 0) {
                    moved = true;
                    best_score = estimated_objective;
                    best_to_score = to_v.objective();
                    best_from_score = from_vehicle.objective();
                    best_to_v = &to_v;
                    best_from_v = &from_vehicle;
                    best_order = order;
                    best_candidate = candidate;
                }
            }  // to vehicles
        }  // orders
    }  // from vehicles

    if (moved) {
        tabu_list.add(Move((*best_from_v), (*best_to_v), best_order, best_to_score, best_from_score));
        best_to_v->hillClimb(best_order);
        best_from_v->erase(best_order);
        tabu_list.add_seen(best_candidate);
        if (best_from_v->is_phony()) m_unassignedOrders -= best_order.idx();
        save_if_best();
    }
    return moved;
}


/** @brief Swap Between Routes
 *
 * @returns false when a swap between routes was not successful
 * @returns true when a swap between routes was successful
 * @param [in] intensify to declare an intensification phase swap between routes
 * @param [in] diversify to declare an diversification phase swap between routes
 */
bool
Optimize::swap_between_routes(bool intensify, bool diversify) {
    sort_by_size(false);
    auto best_to_v = &m_fleet[0];
    auto best_from_v = &m_fleet[0];
    double best_score = intensify ? objective() : 0;
    double best_to_score;
    double best_from_score;
    bool swapped = false;
    auto best_from_order = orders()[0];
    auto best_to_order = orders()[0];

    std::string best_candidate1;
    std::string best_candidate2;

    for (size_t i = 0; i < m_fleet.size(); ++i)  {
        problem::Vehicle_pickDeliver &from_vehicle = m_fleet[i];
        if (from_vehicle.is_phony() || from_vehicle.empty()) continue;

        auto first_order_set = from_vehicle.orders_in_vehicle();
        for (const auto o_id1 : first_order_set) {
            auto order1 = orders()[o_id1];
            for (size_t j = i + 1; j < m_fleet.size(); ++j) {
                problem::Vehicle_pickDeliver &to_v = m_fleet[j];
                if (&from_vehicle == &to_v) continue;
                if (to_v.is_phony() || to_v.empty()) continue;
                if (!to_v.feasible_orders().has(o_id1)) continue;


                /*
                 * cycle through to orders for swap
                 */
                auto second_order_set = to_v.orders_in_vehicle();
                for (const auto o_id2 : second_order_set) {
                    auto order2 = orders()[o_id2];
                    if (!from_vehicle.feasible_orders().has(o_id2)) continue;

                    auto curr_from_objective = from_vehicle.objective();
                    auto curr_to_objective = to_v.objective();

                    /*
                     * setup local working vehicles
                     */
                    auto from_v_copy = from_vehicle;
                    auto to_v_copy = to_v;


                    pgassert(from_v_copy.has_order(order1));
                    pgassert(to_v_copy.has_order(order2));


                    /*
                     * do one truck at a time
                     */
                    to_v_copy.erase(order2);
                    if (to_v_copy.orders_in_vehicle().has(o_id2) || !to_v_copy.is_feasible()) continue;

                    std::ostringstream ss1;
                    ss1 << to_v_copy.path_str() << ":" << o_id1;

                    std::string candidate1 = ss1.str();

                    if (tabu_list.has_infeasible(candidate1)) continue;

                    from_v_copy.erase(order1);
                    if (from_v_copy.orders_in_vehicle().has(o_id1) || !from_v_copy.is_feasible()) continue;

                    std::ostringstream ss2;
                    ss2 << from_v_copy.path_str() << ":" << o_id2;

                    std::string candidate2 = ss2.str();

                    if (diversify && tabu_list.has_seen(candidate1) && tabu_list.has_seen(candidate2)) continue;

                    if (tabu_list.has_infeasible(candidate2)) continue;

                    to_v_copy.hillClimb(order1);
                    if (!to_v_copy.has_order(order1) || !to_v_copy.is_feasible()) {
                        tabu_list.add_infeasible(candidate1);
                        continue;
                    }


                    from_v_copy.hillClimb(order2);
                    if (!from_v_copy.has_order(order2) || !from_v_copy.is_feasible()) {
                        tabu_list.add_infeasible(candidate2);
                        continue;
                    }


                    /*
                     * Evaluate the swap
                     */
                    auto new_from_objective = from_v_copy.objective();
                    auto new_to_objective = to_v_copy.objective();

                    auto estimated_delta =
                            +(new_to_objective + new_from_objective)
                            - (curr_from_objective + curr_to_objective);

                    auto estimated_objective = objective() + estimated_delta;
                    /*
                     * dont swap if there is no improvement in either vehicle
                     */
                    if (estimated_objective >= best_score && best_score != 0) continue;

                    if (tabu_list.has_swap(
                                from_v_copy, to_v_copy, order1, order2, from_v_copy.objective(),
                                to_v_copy.objective())
                        && estimated_objective >= best_solution.objective()) continue;


                    if (estimated_objective < best_score || best_score == 0) {
                        swapped = true;
                        best_from_score = from_vehicle.objective();
                        best_to_score = to_v.objective();
                        best_score = estimated_objective;
                        best_to_v = &to_v;
                        best_from_v = &from_vehicle;
                        best_from_order = order1;
                        best_to_order = order2;
                        best_candidate1 = candidate1;
                        best_candidate2 = candidate2;
                    }
                }
            }  // to vehicles
        }  // orders
    }  // from vehicles
    if (swapped) {
        tabu_list.add(
                Move((*best_from_v), (*best_to_v), best_from_order, best_to_order, best_to_score, best_from_score));
        best_from_v->erase(best_from_order);
        best_to_v->erase(best_to_order);
        best_from_v->hillClimb(best_to_order);
        best_to_v->hillClimb(best_from_order);
        tabu_list.add_seen(best_candidate1);
        tabu_list.add_seen(best_candidate2);
        if (best_from_v->is_phony()) {
            m_unassignedOrders -= best_from_order.idx();
            m_unassignedOrders += best_to_order.idx();
        }
        if (best_to_v->is_phony()) {
            m_unassignedOrders -= best_to_order.idx();
            m_unassignedOrders += best_from_order.idx();
        }
        save_if_best();
    }
    return swapped;
}

#if 1
/**
 *
 * @post pv1.size <= pv2.size  ... <=  pvN.size  v1.size <= v2.size  ... <=  vN.size
 */
void
Optimize::sort_by_size(bool asc) {
    /*
     * sort by number of orders on the vehicles
     */
    if (asc) {
        std::sort(m_fleet.begin(), m_fleet.end(), []
                (const problem::Vehicle_pickDeliver &lhs, const problem::Vehicle_pickDeliver &rhs)
                -> bool {
            if (lhs.orders_in_vehicle().size() == rhs.orders_in_vehicle().size()) {
                return lhs.id() < rhs.id();
            } else {
                return lhs.orders_in_vehicle().size() < rhs.orders_in_vehicle().size();
            }
        });
    } else {
        std::sort(m_fleet.begin(), m_fleet.end(), []
                (const problem::Vehicle_pickDeliver &lhs, const problem::Vehicle_pickDeliver &rhs)
                -> bool {
            if (lhs.orders_in_vehicle().size() == rhs.orders_in_vehicle().size()) {
                return lhs.id() > rhs.id();
            } else {
                return lhs.orders_in_vehicle().size() > rhs.orders_in_vehicle().size();
            }
        });
    }


    /*
     * Leave the phony vehicles at the beginning
     */
    std::stable_partition(m_fleet.begin(), m_fleet.end(), []
            (const problem::Vehicle_pickDeliver &v)
            -> bool {
            return v.id() < 0;
            });
}
#endif

/**
 *
 * @post the fleet does not have phony empty vehicles
 * @post does not remove real vehicles
 * @post the fleet size is reduced
 *
 */
void
Optimize::delete_empty_truck() {
    /*
     * remove from the fleet vehicles that are phony and empty
     */
    m_fleet.erase(std::remove_if(
            m_fleet.begin(),
            m_fleet.end(),
            [](const problem::Vehicle_pickDeliver &v){
                return v.orders_in_vehicle().empty() && v.is_phony();}),
                  m_fleet.end());
}


/**
 * Save as best solution when objective function value is smaller
 */
void
Optimize::save_if_best() {
    if (objective() < best_solution.objective()) {
        best_solution = (*this);
        log << "\t***  best objective " << best_solution.cost_str();
    }
}

}  //  namespace tabu
}  //  namespace optimizers
}  //  namespace vrprouting
