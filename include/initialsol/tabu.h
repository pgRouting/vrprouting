/*PGR-GNU*****************************************************************

FILE: initial_solution.h

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

#ifndef INCLUDE_INITIALSOL_TABU_H_
#define INCLUDE_INITIALSOL_TABU_H_
#pragma once

#include "c_types/typedefs.h"
#include "cpp_common/identifiers.hpp"
#include "problem/solution.h"
#include "problem/vehicle_pickDeliver.h"

namespace vrprouting {
namespace problem {
class PickDeliver;
}

namespace initialsol {
namespace tabu {

class Initial_solution :  public problem::Solution {
 public:
    /** @brief Initial solution without information is not valid */
    Initial_solution() = delete;

    /** @brief Creating a complete initial solution */
    Initial_solution(TTimestamp, bool, problem::PickDeliver*);

 private:
    using Solution::vehicles;

    /** @brief class invariant about the orders that have been assigned */
    void invariant() const;

    /** @brief processes the initial solution given by the user */
    void process_given_solution_from_user(TTimestamp, bool);

    /** @brief Adds unassigned orders to phony vehicles */
    void process_unassigned();

    /** set of all orders */
    Identifiers<size_t> m_all_orders;

    /** set of unassigned orders */
    Identifiers<size_t> m_unassigned;

    /** set of assigned orders */
    Identifiers<size_t> m_assigned;
};

}  //  namespace tabu
}  //  namespace initialsol
}  //  namespace vrprouting

#endif  // INCLUDE_INITIALSOL_TABU_H_
