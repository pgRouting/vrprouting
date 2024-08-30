/*PGR-GNU*****************************************************************

FILE: vroom_matrix.hpp

Copyright (c) 2024 pgRouting developers
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

#ifndef INCLUDE_CPP_COMMON_VROOM_MATRIX_HPP_
#define INCLUDE_CPP_COMMON_VROOM_MATRIX_HPP_
#pragma once

#include <structures/generic/matrix.h>

#include <iosfwd>
#include <vector>
#include <map>
#include <utility>
#include <cstdint>

#include "c_types/typedefs.h"
#include "cpp_common/identifiers.hpp"

namespace vrprouting {

class Vroom_matrix_t;

namespace vroom {

/** @brief N x N matrix
 *
 * - The internal data interpretation is done by the user of this class
 * - Once created do not modifiy
 */
class Matrix {
 public:
    /** @brief Constructs an emtpy matrix */
    Matrix() = default;
    Matrix(const std::vector<Vroom_matrix_t>&, const Identifiers<Id>&, double);

    ::vroom::Matrix<::vroom::Duration> get_vroom_duration_matrix() const;
    ::vroom::Matrix<::vroom::Cost> get_vroom_cost_matrix() const;

    /** @brief the size of the matrix */
    size_t size() const {return m_ids.size();}

    /** @brief original id -> idx */
    Idx get_index(Id) const;

    /** @brief original id -> idx */
    Id get_original_id(Idx) const;

    /** @brief has identifier */
    bool has_id(Id) const;

 private:
    typedef std::vector<std::vector<std::pair<TInterval, TravelCost>>> VMatrix;

    /** @brief does the matrix values not given by the user? */
    bool has_infinity(const VMatrix&) const;
    void set_vroom_duration_matrix(const VMatrix&);
    void set_vroom_cost_matrix(const VMatrix&);

    /** DATA **/
    /** ordered list of user identifiers */
    std::vector<Id> m_ids;

    /** @brief the dureation matrix for vroom */
    ::vroom::Matrix<::vroom::Duration> m_dmatrix;
    /** @brief the dureation matrix for vroom */
    ::vroom::Matrix<::vroom::Cost>     m_cmatrix;
};

}  // namespace vroom
}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VROOM_MATRIX_HPP_
