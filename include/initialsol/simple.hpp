/*PGR-GNU*****************************************************************

FILE: simple.hpp

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

#ifndef INCLUDE_INITIALSOL_SIMPLE_HPP_
#define INCLUDE_INITIALSOL_SIMPLE_HPP_
#pragma once

#include "cpp_common/identifiers.hpp"
#include "problem/solution.hpp"
#include "initialsol/initials_code.hpp"

namespace vrprouting {
namespace problem {
class Vehicle_PickDeliver;
class PickDeliver;
}  // namespace problem

namespace initialsol {
namespace simple {

class Initial_solution : public problem::Solution {
 public:
    Initial_solution() = delete;
    Initial_solution(Initials_code, problem::PickDeliver&);

    void invariant() const;

 private:
    /*
     * one truck per order
     */
    void one_truck_all_orders();

    void do_while_foo(int kind);

    void do_while_feasible(
        problem::Vehicle_pickDeliver& truck,
        Initials_code kind,
        Identifiers<size_t> &unassigned,
        Identifiers<size_t> &assigned);

    Identifiers<size_t> all_orders;
    Identifiers<size_t> unassigned;
    Identifiers<size_t> assigned;
};

}  //  namespace simple
}  //  namespace initialsol
}  //  namespace vrprouting

#endif  // INCLUDE_INITIALSOL_SIMPLE_HPP_
