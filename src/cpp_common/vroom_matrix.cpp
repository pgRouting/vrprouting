/*PGR-GNU*****************************************************************
FILE: vroom_matrix.cpp

Copyright (c) 2024 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2024 Celia Virginia Vergara Castillo
Mail: vicky aat erosion.dev

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

#include "cpp_common/vroom_matrix.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <cmath>
#include <utility>

#include "cpp_common/identifiers.hpp"
#include "cpp_common/assert.hpp"
#include "cpp_common/matrix_cell_t.hpp"
#include "cpp_common/vroom_matrix_t.hpp"


namespace vrprouting {
namespace vroom {

/**
 * @param [in] id original identifier
 * @returns true when it exists on the saved ids
 */
bool
Matrix::has_id(Id id) const {
    auto pos = std::lower_bound(m_ids.cbegin(), m_ids.cend(), id);
    return pos != m_ids.end() && *pos == id;
}

/**
 * Given an original node identifier returns the internal index
 * @param [in] id
 * @returns the position of the identifier
 */
Idx
Matrix::get_index(Id id) const {
    auto pos = std::lower_bound(m_ids.begin(), m_ids.end(), id);
    if (pos == m_ids.end()) {
        std::ostringstream msg;
        msg << "Not found " << id;
        throw std::make_pair(std::string("(INTERNAL) Matrix: Unable to find node on matrix"), msg.str());
    }
    pgassert(pos != m_ids.end());

    return static_cast<Idx>(pos - m_ids.begin());
}

/**
 * Given the internal index, returns the original node identifier
 * @param [in] index
 * @returns the original node identifier
 */
Id
Matrix::get_original_id(Idx index) const {
    if (index >= m_ids.size()) {
        std::ostringstream msg;
        msg << "Out of range" << index;
        throw std::make_pair(std::string("(INTERNAL) Matrix: The given index is out of range"), msg.str());
    }
    pgassert(index < m_ids.size());

    return static_cast<Id>(m_ids[index]);
}


/**
 * @brief Constructor for VROOM matrix input
 *
 * @param [in] matrix  The set of costs
 * @param [in] location_ids The location identifiers
 * @param [in] scaling_factor Multiplier
 *
 * @pre matrix_rows is not empty
 * @post location_ids has all the ids
 * @post matrix_rows[u, v] is ignored when u or v are not in location_id
 * @post m_dmatrix & m_cmatrix ready to use with vroom
 * @throws matrix_rows[u, v] = inf, inf
 *
 */
Matrix::Matrix(
        const std::vector<Vroom_matrix_t> &matrix,
        const Identifiers<Id> &location_ids, double scaling_factor) {
    /*
     * Sets the selected nodes identifiers
     */
    m_ids.insert(m_ids.begin(), location_ids.begin(), location_ids.end());
    std::vector<std::vector<std::pair<TInterval, TravelCost>>> m_matrix;

    /*
     * Create matrix
     * Set initial values to infinity
     */
    m_matrix.resize(
            m_ids.size(),
            std::vector<std::pair<TInterval, TravelCost>>(m_ids.size(),
                std::make_pair(
                    (std::numeric_limits<TravelCost>::max)(),
                    (std::numeric_limits<TravelCost>::max)())));

    Identifiers<Idx> inserted;
    /*
     * Cycle the matrix data
     */
    for (const auto &cell : matrix) {
        /*
         * skip if row is not from selected nodes
         */
        if (!(has_id(cell.start_id) && has_id(cell.end_id))) continue;

        auto sid = get_index(cell.start_id);
        auto eid = get_index(cell.end_id);

        /*
         * Save the information. Scale the time matrix according to scaling_factor
         */
        m_matrix[sid][eid] =
            std::make_pair(
                    static_cast<Duration>(std::round(cell.duration / scaling_factor)),
                    static_cast<Duration>(cell.cost));

        /*
         * If the opposite direction is infinity insert the same cost
         */
        if (m_matrix[eid][sid].second == (std::numeric_limits<TravelCost>::max)()) {
            m_matrix[eid][sid] = m_matrix[sid][eid];
        }
    }

    /*
     * Set the diagonal values to 0
     */
    for (size_t i = 0; i < m_matrix.size(); ++i) {
        m_matrix[i][i] = std::make_pair(0, 0);
    }

    /*
     * Verify matrix cells preconditions
     */
    if (has_infinity(m_matrix)) {
        throw std::string("An Infinity value was found on the Matrix. Might be missing information of a node");
    }

    set_vroom_duration_matrix(m_matrix);
    set_vroom_cost_matrix(m_matrix);
}

::vroom::Matrix<::vroom::Duration>
Matrix::get_vroom_duration_matrix() const {
    return m_dmatrix;
}

::vroom::Matrix<::vroom::Cost>
Matrix::get_vroom_cost_matrix() const {
    return m_cmatrix;
}

/**
 * @brief sets the VROOM duration matrix from vrprouting Base Matrix
 */
void
Matrix::set_vroom_duration_matrix(const VMatrix &m_matrix) {
    size_t matrix_size = m_ids.size();
    m_dmatrix = ::vroom::Matrix<::vroom::Duration>(matrix_size);
    for (size_t i = 0; i < matrix_size; i++) {
        for (size_t j = 0; j < matrix_size; j++) {
            m_dmatrix[i][j] = static_cast<::vroom::Duration>(m_matrix[i][j].first);
        }
    }
}

/**
 * @brief sets the VROOM cost matrix from vrprouting Base Matrix
 */
void
Matrix::set_vroom_cost_matrix(const VMatrix &m_matrix) {
    size_t matrix_size = m_ids.size();
    m_cmatrix = ::vroom::Matrix<::vroom::Cost>(matrix_size);
    for (size_t i = 0; i < matrix_size; i++) {
        for (size_t j = 0; j < matrix_size; j++) {
            m_cmatrix[i][j] = static_cast<::vroom::Cost>(m_matrix[i][j].second);
        }
    }
}

/**
 * @returns false at the moment it finds an infinity value
 * @returns true otherwise
 */
bool
Matrix::has_infinity(const VMatrix &m_matrix) const {
    const auto inf1 = (std::numeric_limits<::vroom::Duration>::max)();
    const auto inf2 = (std::numeric_limits<::vroom::Cost>::max)();
    for (const auto &row : m_matrix) {
        for (const auto &val : row) {
            /*
             * found infinity?
             */
            if (val.first == inf1 || val.second == inf2) return true;
        }
    }
    return false;
}

}  // namespace vroom
}  // namespace vrprouting
