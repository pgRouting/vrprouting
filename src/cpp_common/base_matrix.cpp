/*PGR-GNU*****************************************************************
FILE: matrix.cpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo

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

#include "cpp_common/base_matrix.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <cmath>
#include <utility>

#include "cpp_common/identifiers.hpp"
#include "cpp_common/pgr_assert.h"
#include "c_types/matrix_cell_t.h"


namespace vrprouting {
namespace base {

namespace detail {
double
get_distance(std::pair<Coordinate, Coordinate> p1 , std::pair<Coordinate, Coordinate> p2) {
  auto dx = p1.first - p2.first;
  auto dy = p1.second - p2.second;
  return std::sqrt(dx * dx + dy * dy);
}
}  // namespace detail

/**
 * Traverses the matrix information to get the ids of the nodes
 * @param [in] data Base_Matrix information
 * @post m_ids contains all the nodes original ids
 *
 @dot
 digraph G {
 graph [ranksep=".05"];
 node[fontsize=10, nodesep=0.2];
 start  [shape=Mdiamond];
 n0  [label="Base_Matrix::set_ids",shape=rect, color=green];
 subgraph clusterA {
 n1  [label="Cycle the information",shape=rect];
 subgraph clusterB {
 n2  [label="Extract the original identifiers",shape=rect];
 n3  [label="Save the extracted information",shape=rect];
 }
 }
 start -> n0 -> n1 -> n2 -> n3 -> end;
 end  [shape=Mdiamond];
 error [shape=Mdiamond,color=red]
 a [label="assertion",color=red];
 a -> error [color=red,label="fail",fontcolor=red];
 }
 @enddot
 */
void
Base_Matrix::set_ids(const std::vector<Matrix_cell_t> &data) {
  pgassert(m_ids.empty());
  Identifiers<Id> node_ids;
  /*
   * Cycle the information
   */
  for (const auto &cost : data) {
    /*
     * extract the original identifiers
     */
    node_ids += cost.from_vid;
    node_ids += cost.to_vid;
  }
  /*
   * Save the extracted information
   */
  m_ids.insert(m_ids.begin(), node_ids.begin(), node_ids.end());
}


/**
 * @param [in] id original identifier
 * @returns true when it exists on the saved ids
 *
 @dot
 digraph G {
 graph [ranksep=".05"];
 node[fontsize=10, nodesep=0.2];
 start  [shape=Mdiamond];
 n0  [label="Base_Matrix::has_id",shape=rect, color=green];
 n1  [label="Binary search the stored identifiers",shape=rect];
 n2  [label="Return search results",shape=rect];
 start -> n0 -> n1 -> n2 -> end;
 end  [shape=Mdiamond];
 error [shape=Mdiamond,color=red]
 a [label="assertion",color=red];
 a -> error [color=red,label="fail",fontcolor=red];
 }
 @enddot
 */
bool
Base_Matrix::has_id(Id id) const {
  /*
   * binary search the stored identifiers
   */
  auto pos = std::lower_bound(m_ids.cbegin(), m_ids.cend(), id);

  /*
   * Return search results
   */
  return pos != m_ids.end() && *pos == id;
}


/** Given an original node identifier returns the internal index
 *
 * @param [in] id
 * @returns the position of the identifier
 *
 @dot
 digraph G {
 graph [ranksep=".05"];
 node[fontsize=10, nodesep=0.2];
 start  [shape=Mdiamond];
 n0  [label="Base_Matrix::get_index",shape=rect, color=green];
 n1  [label="Binary search the stored identifiers",shape=rect];
 n2  [label="Return the index found",shape=rect];
 start -> n0 -> n1 -> n2 -> end;
 end  [shape=Mdiamond];
 error [shape=Mdiamond,color=red]
 a [label="assertion",color=red];
 a -> error [color=red,label="fail",fontcolor=red];
 }
 @enddot
 */
Idx
Base_Matrix::get_index(Id id) const {
  /*
   * binary search the stored identifiers
   */
  auto pos = std::lower_bound(m_ids.begin(), m_ids.end(), id);
  if (pos == m_ids.end()) {
    std::ostringstream msg;
    msg << *this << "\nNot found" << id;
    pgassertwm(false, msg.str());
    throw std::make_pair(std::string("(INTERNAL) Base_Matrix: Unable to find node on matrix"), msg.str());
  }
  pgassert(pos != m_ids.end());

  /*
   * return the index found
   */
  return static_cast<Idx>(pos - m_ids.begin());
}

/**
 * @param [in] data_costs  The set of costs
 * @param [in] size_matrix The size of the set of costs
 * @param [in] node_ids The selected node identifiers to be added
 * @param [in] multiplier All times are multiplied by this value
 *
 * @pre data_costs is not empty
 * @post ids has all the ids of node_ids
 * @post data_costs[from_vid, to_vid] is ignored when from_vid is not in node_ids or  to_vid is not in node_ids
 * @post costs[from_vid, to_vid] is not has the cell cost when from_vid, to_vid are in node_ids
 * @post costs[from_vid, to_vid] = inf when cell from_vid, to_vid does not exist
 * @post costs[from_vid, to_vid] = 0 when from_vid = to_vid
 *
 */
Base_Matrix::Base_Matrix(
    Matrix_cell_t *data_costs, size_t size_matrix,
    const Identifiers<Id>& node_ids,
    Multiplier multiplier) {
  /*
   * Sets the selected nodes identifiers
   */
  m_ids.insert(m_ids.begin(), node_ids.begin(), node_ids.end());

  /*
   * Create matrix
   */
  m_time_matrix.resize(
      m_ids.size(),
      std::vector<TInterval>(
        m_ids.size(),
        /*
         * Set initial values to infinity
         */
        (std::numeric_limits<TInterval>::max)()));

  Identifiers<Idx> inserted;
  /*
   * Cycle the matrix data
   */
  for (size_t i = 0; i < size_matrix; ++i) {
    auto data = data_costs[i];
    /*
     * skip if row is not from selected nodes
     */
    if (!(has_id(data.from_vid) && has_id(data.to_vid))) continue;

    /*
     * Save the information
     */
    m_time_matrix[get_index(data.from_vid)][get_index(data.to_vid)] =
      static_cast<TInterval>(static_cast<Multiplier>(data.cost) * multiplier);

    /*
     * If the opposite direction is infinity insert the same cost
     */
    if (m_time_matrix[get_index(data.to_vid)][get_index(data.from_vid)] == (std::numeric_limits<TInterval>::max)()) {
      m_time_matrix[get_index(data.to_vid)][get_index(data.from_vid)] =
        m_time_matrix[get_index(data.from_vid)][get_index(data.to_vid)];
    }
  }

  /*
   * Set the diagonal values to 0
   */
  for (size_t i = 0; i < m_time_matrix.size(); ++i) {
    m_time_matrix[i][i] = 0;
  }
}

/*
 * constructor for euclidean
 */
Base_Matrix::Base_Matrix(const std::map<std::pair<Coordinate, Coordinate>, Id> &euclidean_data, Multiplier multiplier) {
  m_ids.reserve(euclidean_data.size());
  for (const auto &e : euclidean_data) {
    m_ids.push_back(e.second);
  }
  m_time_matrix.resize(
      m_ids.size(),
      std::vector<TInterval>(
        m_ids.size(),
        (std::numeric_limits<TInterval>::max)()));

  for (const auto &from : euclidean_data) {
    for (const auto &to : euclidean_data) {
      auto from_id = get_index(from.second);
      auto to_id = get_index(to.second);
      m_time_matrix[from_id][to_id] =
        static_cast<TInterval>(static_cast<Multiplier>(detail::get_distance(from.first, to.first)) * multiplier);
      m_time_matrix[to_id][from_id] = m_time_matrix[from_id][to_id];
    }
  }

  for (size_t i = 0; i < m_time_matrix.size(); ++i) {
    m_time_matrix[i][i] = 0;
  }
}



/**
 * @returns false at the moment it finds an infinity value
 * @returns true otherwise
 *
 @dot
 digraph G {
 graph [ranksep=".05"];
 node[fontsize=10, nodesep=0.2];
 start  [shape=Mdiamond];
 n0  [label="Base_Matrix::has_no_infinity",shape=rect, color=green];
 n1  [label="Cycle the matrix",shape=rect];
 n2  [label="Found infinity?",shape=cds, color=blue];
 n3  [label="Return false",shape=rect];
 n4  [label="Return true",shape=rect];
 start -> n0 -> n1 -> n2
n2:w -> n3 [label="Yes"]
n2:e -> n4 [label="No"]
n4 -> end;
n3 -> end;
end  [shape=Mdiamond];
error [shape=Mdiamond,color=red]
a [label="assertion",color=red];
a -> error [color=red,label="fail",fontcolor=red];
}
@enddot
*/
bool
Base_Matrix::has_no_infinity() const {
  /*
   * Cycle the matrix
   */
  for (const auto &row : m_time_matrix) {
    for (const auto &val : row) {
      /*
       * found infinity?
       *
       * yes -> return false
       */
      if (val == (std::numeric_limits<TInterval>::max)()) return false;
    }
  }
  /*
   * return true
   */
  return true;
}

/*!
 * Triangle Inequality Theorem.
 *  The sum of the lengths of any two sides of a triangle is greater than the length of the third side.
 *  NOTE: can also be equal for streets
 * m_time_matrix[i][k] != inf
 * m_time_matrix[i][k] <= m_time_matrix[i][j] + m_time_matrix[j][k]
 */
bool
Base_Matrix::obeys_triangle_inequality() const {
  for (size_t i = 0; i < m_time_matrix.size(); ++i) {
    for (size_t j = 0; j < m_time_matrix.size(); ++j) {
      for (size_t k = 0; k < m_time_matrix.size(); ++k) {
        if (m_time_matrix[i][k] > (m_time_matrix[i][j] + m_time_matrix[j][k])) {
          return false;
        }
      }
    }
  }
  return true;
}

/*!
 * Fix Triangle Inequality Theorem.
 *  The sum of the lengths of any two sides of a triangle is greater than the length of the third side.
 *  NOTE: can also be equal for streets
 * costs[i][k] != inf
 * costs[i][k] <= costs[i][j] + costs[j][k]
 */
size_t
Base_Matrix::fix_triangle_inequality(size_t depth) {
  if (depth > m_time_matrix.size()) return depth;
  for (auto & i : m_time_matrix) {
    for (size_t j = 0; j < m_time_matrix.size(); ++j) {
      for (size_t k = 0; k < m_time_matrix.size(); ++k) {
        if (i[k] > (i[j] + m_time_matrix[j][k])) {
          i[k] = i[j] + m_time_matrix[j][k];
          return fix_triangle_inequality(++depth);
        }
      }
    }
  }
  return depth;
}

bool
Base_Matrix::is_symmetric() const {
  for (size_t i = 0; i < m_time_matrix.size(); ++i) {
    for (size_t j = 0; j < m_time_matrix.size(); ++j) {
      if (0.000001 < std::fabs(m_time_matrix[i][j] - m_time_matrix[j][i])) {
        std::ostringstream log;
        log << "i \t" << i
          << "j \t" << j
          << "m_time_matrix[i][j] \t" << m_time_matrix[i][j]
          << "m_time_matrix[j][i] \t" << m_time_matrix[j][i]
          << "\n";
        log << (*this);
        pgassertwm(false, log.str());
        return false;
      }
    }
  }
  return true;
}


/**
 * @param [in,out] log stream variable where to print
 * @param [in] matrix The matrix
 *
 @dot
 digraph G {
 graph [ranksep=".05"];
 node[fontsize=10, nodesep=0.2];
 start  [shape=Mdiamond];
 n0  [label="std::ostream& operator",shape=rect, color=green];
 n1  [label="Print the identifiers",shape=rect];
 subgraph clusterA {
 n2  [label="Cycle the cells",shape=rect];
 n3  [label="print the information",shape=rect];
 }
 start -> n0 -> n1 -> n2 -> n3 -> end
 end  [shape=Mdiamond];
 error [shape=Mdiamond,color=red]
 a [label="assertion",color=red];
 a -> error [color=red,label="fail",fontcolor=red];
 }
 @enddot
 */
std::ostream& operator<<(std::ostream &log, const Base_Matrix &matrix) {
  /*
   * print the identifiers
   */
  for (const auto id : matrix.m_ids) {
    log << "\t" << id;
  }
  log << "\n";
  size_t i = 0;

  /*
   * Cycle the cells
   */
  for (const auto &row : matrix.m_time_matrix) {
    size_t j = 0;
    for (const auto cost : row) {
      /*
       * print the information
       */
      log << "Internal(" << i << "," << j << ")"
        << "\tOriginal(" << matrix.m_ids[i] << "," << matrix.m_ids[j] << ")"
        << "\t = " << cost
        << "\n";
      ++j;
    }
    ++i;
  }
  return log;
}


}  // namespace base
}  // namespace vrprouting
