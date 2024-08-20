/*PGR-GNU*****************************************************************

FILE: base_matrix.hpp

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

#ifndef INCLUDE_CPP_COMMON_BASE_MATRIX_HPP_
#define INCLUDE_CPP_COMMON_BASE_MATRIX_HPP_
#pragma once

#include <iosfwd>
#include <vector>
#include <map>

#include "c_types/typedefs.h"
#include "cpp_common/identifiers.hpp"

typedef struct Matrix_cell_t Matrix_cell_t;

namespace vrprouting {
namespace base {


/** @brief N x N matrix
 *
 * - The internal data interpretation is done by the user of this class
 * - Once created can not be modified
 *

@dot
digraph structs {
    Base_Matrix [shape=record,label="{Base_Matrix | ids: Ordered node identifiers | data: The matrix contents}"];
}
@enddot
 */
class Base_Matrix {
 public:
    /** @brief Constructs an emtpy matrix */
    Base_Matrix() = default;
    /** @brief Constructs a matrix for only specific identifiers */
    Base_Matrix(Matrix_cell_t*, size_t, const Identifiers<Id>&, Multiplier);
    /** @brief Constructs a matrix for the euclidean */
    Base_Matrix(const std::map<std::pair<Coordinate, Coordinate>, Id>&, Multiplier);

    /** @name status of the matrix
     * @{
     */
    /** @brief does the matrix values not given by the user? */
    bool has_no_infinity() const;

    /** @brief does the matrix obeys the triangle inequality? */
    bool obeys_triangle_inequality() const;
    size_t fix_triangle_inequality(size_t depth = 0);

    /** @brief is the matrix empty? */
    bool empty() const {return m_ids.empty();}

    /** @brief |idx|
     *
     * @returns the size of the matrix
     */
    size_t size() const {return m_ids.size();}


    /** @}*/

    TInterval at(Idx i, Idx j) const {return m_time_matrix[i][j];}


    /** @brief print matrix (row per cell)*/
    friend std::ostream& operator<<(
            std::ostream &log,
            const Base_Matrix &matrix);

    /** @brief has identifier */
    bool has_id(Id) const;

    /** @brief original id -> idx */
    Idx get_index(Id) const;

    /** @brief idx -> original id */
    Id get_original_id(Idx) const;

 private:
    /** @brief set the ids of the nodes */
    void set_ids(const std::vector<Matrix_cell_t> &);

    /** DATA **/
    /** ordered list of user identifiers */
    std::vector<Id> m_ids;

    /** @brief the actual time matrix
     *
     * m_time_matrix[i][j] i and j are index from the ids
     */
    std::vector<std::vector<TInterval>> m_time_matrix;
};

}  // namespace base
}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_BASE_MATRIX_HPP_
