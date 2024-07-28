/*PGR-GNU*****************************************************************

FILE: tabu_list.cpp

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

#include "optimizers/tabu_list.hpp"

#include <string>
#include <iostream>
#include <algorithm>

namespace vrprouting {
namespace optimizers {
namespace tabu {

/**
 * @param [in] m the move
 */
void TabuList::add(const Move &m) {
    if (has(m))  return;
    while (size() >= max_length()) m_tabu_list.pop_front();
    m_tabu_list.push_back(m);
}

/**
 */
void TabuList::clear() {
    m_tabu_list.clear();
}

/**
 * @param [in] m move searched for
 * @returns true when the move was found in the tabu list
 * @returns false when the move was not found in the tabu list
 */
bool TabuList::has(const Move &m) const {
    return std::find(m_tabu_list.begin(), m_tabu_list.end(), m) != m_tabu_list.end();
}

/**
 * @param [in] from_vehicle  First vehicle of the move
 * @param [in] to_vehicle Second vehicle of the move
 * @param [in] order order involved on the move
 * @param [in] obj1
 * @param [in] obj2
 *
 * @returns true when the move was found in the tabu list
 * @returns false when the move was not found in the tabu list
 */
bool TabuList::has_move(const problem::Vehicle_pickDeliver& from_vehicle,
                        const problem::Vehicle_pickDeliver& to_vehicle,
                        const problem::Order& order, double obj1, double obj2) const {
    return has({from_vehicle, to_vehicle, order, obj1, obj2}) ||
            has({to_vehicle, from_vehicle, order, obj1, obj2});
}

/**
 * @param [in] from_vehicle  First vehicle of the move
 * @param [in] to_vehicle Second vehicle of the move
 * @param [in] from_order involved on the swap move
 * @param [in] to_order involved on the swap move
 * @param [in] obj1
 * @param [in] obj2
 *
 * @returns true when the move was found in the tabu list
 * @returns false when the move was not found in the tabu list
 */
bool TabuList::has_swap(const problem::Vehicle_pickDeliver& from_vehicle,
                        const problem::Vehicle_pickDeliver& to_vehicle,
                        const problem::Order& from_order,
                        const problem::Order& to_order, double obj1, double obj2) const {
    return has({from_vehicle, to_vehicle, from_order, to_order, obj1, obj2}) ||
            has({from_vehicle, to_vehicle, to_order, from_order, obj1, obj2}) ||
            has({to_vehicle, from_vehicle, from_order, to_order, obj1, obj2}) ||
            has({to_vehicle, from_vehicle, to_order, from_order, obj1, obj2});
}



/**
 *
 * @returns true when the list is empty
 * @returns false when the list is not empty
 */
bool TabuList::is_empty() const {
    return m_tabu_list.empty();
}

/**
 *
 * @returns the number of moves on the tabu list
 */
size_t TabuList::size() const {
    return m_tabu_list.size();
}

/**
 *
 * @returns the maximum possible number of moves on the tabu list
 */
size_t TabuList::max_length() const {
    return m_max_length;
}

/**
 * @param [in] length
 */
void TabuList::set_max_length(size_t length) {
    m_max_length = length;
}

/**
 * @param [in,out] log where the print takes place
 * @param [in] list The tabu list to be printed
 */
std::ostream&
operator << (std::ostream &log, const TabuList &list) {
    log << "\nTabuList length: " << list.size()
        << "\n *><* *** tabu list start *** ";

    for (const auto &m : list) log << m << ' ';

    log << " *** tabu list end ***\n";
    return log;
}

bool TabuList::has_infeasible(const std::string& candidate) {
    return (m_infeasible_list.find(candidate) != m_infeasible_list.end());
}

void TabuList::add_infeasible(const std::string& candidate) {
    m_infeasible_list.insert(candidate);
}

bool TabuList::has_seen(const std::string& candidate) {
    return (m_seen_list.find(candidate) != m_seen_list.end());
}

void TabuList::add_seen(const std::string& candidate) {
    m_seen_list.insert(candidate);
}


}  //  namespace tabu
}  //  namespace optimizers
}  //  namespace vrprouting
