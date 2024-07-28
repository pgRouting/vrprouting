/*PGR-GNU*****************************************************************

FILE: tabu_list.h

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

#ifndef INCLUDE_OPTIMIZERS_TABU_LIST_HPP_
#define INCLUDE_OPTIMIZERS_TABU_LIST_HPP_
#pragma once

#include <iosfwd>
#include <string>
#include <deque>
#include <set>

#include "optimizers/move.hpp"

namespace vrprouting {
namespace optimizers {
namespace tabu {


/** @brief The TabuList class defines the tabu list to be used in optimization. */
class TabuList {
 public:
    /** iterator type that allows modification */
    typedef std::deque<Move>::iterator iterator;

    /** iterator type that does not allows modification */
    typedef std::deque<Move>::const_iterator const_iterator;

    /** @brief Make a move "tabu" by adding it to the tabu list */
    void add(const Move &m);

    /** @brief removes all moves from the tabu list */
    void clear();

    /** @brief Checks to see if a "move" move is "tabu" (in the tabu list) */
    bool has_move(const problem::Vehicle_pickDeliver&,
                  const problem::Vehicle_pickDeliver&,
                  const problem::Order& from_order, double obj1, double obj2) const;

    /** @brief Checks to see if a "swap" move is "tabu" (in the tabu list) */
    bool has_swap(const problem::Vehicle_pickDeliver&,
                  const problem::Vehicle_pickDeliver&,
                  const problem::Order&,
                  const problem::Order&, double obj1, double obj2) const;

    /** @brief returns true if the tabu list is empty */
    bool is_empty() const;

    /** @brief returns the current length (number of moves) in the tabu list */
    size_t size() const;

    /** @brief returns the maximum length (tabu_length) of the tabu list */
    size_t max_length() const;

    /** @brief sets the maximum length (tabu_length) of the tabu list */
    void set_max_length(size_t length);

    /** @brief priting function */
    friend std::ostream& operator<<(std::ostream &, const TabuList &);

    /** Constant iterator pointing to the first element of the tabu list */
    const_iterator begin() const {return m_tabu_list.begin();}

    /** Constant iteratori pointing to the last element of the tabu list */
    const_iterator end() const {return m_tabu_list.end();}

    /** @brief Checks to see if a move candidate is infeasible (in the infeasible list) */
    bool has_infeasible(const std::string& candidate);

    /** @brief Adds a move candidate to the infeasible list */
    void add_infeasible(const std::string& candidate);

    /** @brief Checks to see if a move candidate has already been done before (in the seen list) */
    bool has_seen(const std::string& candidate);

    /** @brief Adds a move candidate to the seen list */
    void add_seen(const std::string& candidate);

 private:
    /** @brief Checks to see if a (general) move is "tabu" (in the tabu list) */
    bool has(const Move &) const;

    /** the maximum length (tabu_length) of the tabu list */
    size_t m_max_length;

    /** the tabu list */
    std::deque<Move> m_tabu_list;

    /** the infeasible list */
    std::set<std::string> m_infeasible_list;

    /** the seen list */
    std::set<std::string> m_seen_list;
};


}  //  namespace tabu
}  //  namespace optimizers
}  //  namespace vrprouting

#endif  // INCLUDE_OPTIMIZERS_TABU_LIST_HPP_

