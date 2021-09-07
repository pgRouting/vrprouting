/*PGR-GNU*****************************************************************

FILE: matrix.h

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

#ifndef INCLUDE_PROBLEM_MATRIX_H_
#define INCLUDE_PROBLEM_MATRIX_H_
#pragma once

#include <map>
#include <utility>
#include <tuple>
#include <string>
#include <vector>

#include "cpp_common/base_matrix.h"
#include "cpp_common/identifiers.hpp"
#include "c_types/typedefs.h"


namespace vrprouting {
namespace problem {

class Matrix : public base::Base_Matrix {
 public:
    Matrix() = default;

    /** brief constructor for matrix version with time dependant multipliers */
    Matrix(Matrix_cell_t *, size_t, Time_multipliers_t*, size_t, const Identifiers<Id>&, Multiplier = 1.0);

    /** brief constructor for euclidean version with time dependant multipliers */
    Matrix(const std::map<std::pair<Coordinate, Coordinate>, Id>&, Time_multipliers_t*, size_t, Multiplier = 1.0);

    /** brief constructor for matrix version default multipliers */
    Matrix(Matrix_cell_t *, size_t, const Identifiers<Id>&, Multiplier = 1.0);

    /** brief constructor for euclidean version default multipliers */
    explicit Matrix(const std::map<std::pair<Coordinate, Coordinate>, Id>&, Multiplier = 1.0);

    /** @brief retrun the travel time times when using the time dependant multipliers */
    TInterval travel_time(Id, Id, TTimestamp) const;

    /** @brief retrun the travel time times from the matrix*/
    TInterval travel_time(Id, Id) const;

    std::string multipliers_str() const;

 private:
    /** @brief time dependant multiplier
     *
     * m_multipliers[i] ith time dependant multiplier
     * double = time when the multipliers starts to be valid
     * double = multiplier
     * The multiplier ends its validity at the time of the (i+1)th value
     */
    std::vector<std::tuple<TTimestamp, Multiplier>> m_multipliers;
};

}  // namespace problem
}  // namespace vrprouting

#endif  // INCLUDE_PROBLEM_MATRIX_H_
