/*PGR-GNU*****************************************************************

FILE: pd_orders.h

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

#include "problem/orders.h"

#include <vector>
#include <utility>

#include "cpp_common/assert.hpp"
#include "cpp_common/identifiers.hpp"


namespace vrprouting {
namespace problem {

/**
@returns the index of the order within_this_set that has more possibilities of placing orders after it
@param [in] within_this_set
@pre not within_this_set.empty()
*/
size_t
Orders::find_best_J(
        const Identifiers<size_t> &within_this_set) const {
    pgassert(!within_this_set.empty());
    auto best_order = within_this_set.front();
    size_t max_size = 0;

    for (const auto o : within_this_set) {
        auto size_J =  this->at(o).subsetJ(within_this_set).size();
        if (max_size < size_J) {
            max_size = size_J;
            best_order = o;
        }
    }
    return best_order;
}

/**
@returns the index of the order within_this_set that has more possibilities of placing orders before it
@param [in] within_this_set
@pre not within_this_set.empty()
*/
size_t
Orders::find_best_I(
        const Identifiers<size_t> &within_this_set) const {
    pgassert(!within_this_set.empty());
    auto best_order = within_this_set.front();
    size_t max_size = 0;
    for (const auto o : within_this_set) {
        auto size_I =  this->at(o).subsetI(within_this_set).size();
        if (max_size < size_I) {
            max_size = size_I;
            best_order = o;
        }
    }
    return best_order;
}

/**
@returns the index of the order within_this_set that has more possibilities of placing orders before or after it
@param [in] within_this_set
@pre not within_this_set.empty()
*/
size_t
Orders::find_best_I_J(
        const Identifiers<size_t> &within_this_set) const {
    pgassert(!within_this_set.empty());
    auto best_order = within_this_set.front();
    size_t max_size = 0;

    for (const auto o : within_this_set) {
        auto size_I =  this->at(o).subsetI(within_this_set).size();
        auto size_J =  this->at(o).subsetJ(within_this_set).size();
        if (max_size < (std::max)(size_I, size_J)) {
            max_size = (std::max)(size_I, size_J);
            best_order = o;
        }
    }
    return best_order;
}

/**
@returns true when each order from the set of orders is valid
*/
bool
Orders::is_valid(Speed speed) const {
    for (const auto &o : *this) {
        if (!o.is_valid(speed)) {
            return false;
        }
        pgassert(o.pickup().is_pickup());
        pgassert(o.delivery().is_delivery());
        /* P -> D */
        pgassert(o.delivery().is_compatible_IJ(o.pickup(), speed));
    }
    return true;
}

/**
@post For each order: order -> {J} is set
@post For each order: {I} -> order is set
*/
void
Orders::set_compatibles(Speed speed) {
    for (auto &I : *this) {
        for (const auto& J : *this) {
            I.set_compatibles(J, speed);
        }
    }
}

}  //  namespace problem
}  //  namespace vrprouting

