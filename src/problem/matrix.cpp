/*PGR-GNU*****************************************************************

FILE: Matrix.cpp

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

#include "problem/matrix.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <cmath>
#include <utility>
#include <iomanip>
#include <tuple>


#include "cpp_common/assert.hpp"
#include "c_types/matrix_cell_t.h"
#include "c_types/time_multipliers_t.h"


namespace vrprouting {
namespace problem {

namespace {

/**
 * @param [in] p_multipliers time dependant multiplier data
 * @param [in] size_multipliers number of rows in the data
 * @returns time dependant multiplier container
 */
std::vector<std::tuple<TTimestamp, Multiplier>>
set_tdm(Time_multipliers_t *p_multipliers, size_t size_multipliers) {
    pgassert(size_multipliers > 1);
    std::vector<std::tuple<TTimestamp, Multiplier>> tdm;
    /*
     * Sort the multipliers info
     */
    std::sort(p_multipliers, p_multipliers + size_multipliers,
            [] (const Time_multipliers_t &lhs, const Time_multipliers_t &rhs) {
                return lhs.start_time < rhs.start_time;
            });
    for (size_t i = 0; i < size_multipliers; ++i) {
        tdm.emplace_back(p_multipliers[i].start_time, p_multipliers[i].multiplier);
    }
    return tdm;
}

/**
 * @param [in] tdm time dependant multiplier container
 * @param [in] time the time from where the multiplier is calculated
 * @returns the multiplier corresponding to the time
 */
Multiplier
get_tdm(const std::vector<std::tuple<TTimestamp, Multiplier>> &tdm, TTimestamp time) {
    pgassert(time >= 0);
    Multiplier coeff(1);
    for (const auto &e : tdm) {
        if (time < std::get<0>(e)) break;
        coeff = std::get<1>(e);
    }
    return coeff;
}

/**
 * @param [in] tdm time dependant multiplier container
 * @param [in] time the time from where the multiplier is calculated
 * @returns the next multiplier corresponding to the time
 */
Multiplier
next_tdm(const std::vector<std::tuple<TTimestamp, Multiplier>> &tdm, TTimestamp time) {
    Multiplier coeff(1);
    for (const auto &e : tdm) {
        coeff = std::get<1>(e);
        if (std::get<0>(e) >= time) break;
    }
    return coeff;
}

/**
 * @param [in] tdm time dependant multiplier container
 * @param [in] time the time from where the multiplier is calculated
 * @returns the time when multiplier change
 *
 * Example with 3 time multiplier, no more multiplier after 11
 *
 * 0               9               11
 * |---------------|---------------|
 *
 * time is 9:30 then the time change is the next one after that so 11
 * t_change = 0
 * 0 < 9:30 true
 * continue
 *
 * t_change = 9
 * 9 < 9:30
 * true
 *
 * t_change = 11
 * 11 < 9:30 false  cycle breaks
 *
 * return t_change = 11
 *
 *
 * time is 11:30
 *
 * t_change = 0
 * 0 < 11:30 true
 * continue
 *
 * t_change = 9
 * 9 < 11:30
 * true
 *
 * t_change = 11
 * 11 < 11:30 true
 * cycle ends naturally
 *
 * return t_change = 11
 *
 * post condition:
 *
 * From first example
 * t_change > time
 *
 * from second example
 * t_change is the last one on the list
 *
 */
TTimestamp
time_change(const std::vector<std::tuple<TTimestamp, Multiplier>> &tdm, TTimestamp time) {
    TTimestamp t_change(0);
    for (const auto &e : tdm) {
        t_change = std::get<0>(e);
        if (time <= t_change) break;
    }
    pgassert((time <= t_change)
             || (t_change == std::get<0>(tdm[tdm.size() - 1 ])));
    return t_change;
}

}  // namespace

Matrix::Matrix(
        Matrix_cell_t *matrix, size_t size_matrix,
        Time_multipliers_t *multipliers, size_t size_multipliers,
        const Identifiers<Id>& node_ids,
        Multiplier multiplier) :
    Base_Matrix(matrix, size_matrix, node_ids, multiplier),
    m_multipliers(set_tdm(multipliers, size_multipliers)) { }

/*
 * constructor for euclidean with time dependant multipliers
 */
Matrix::Matrix(
        const std::map<std::pair<Coordinate, Coordinate>, Id> &euclidean_data,
        Time_multipliers_t *multipliers, size_t size_multipliers,
        Multiplier multiplier) :
    Base_Matrix(euclidean_data, multiplier),
    m_multipliers(set_tdm(multipliers, size_multipliers)) { }

/*
 * constructor for euclidean default multipliers
 */
Matrix::Matrix(
        Matrix_cell_t *matrix, size_t size_matrix,
        const Identifiers<Id>& node_ids,
        Multiplier multiplier) :
    Base_Matrix(matrix, size_matrix, node_ids, multiplier),
    m_multipliers{{0, 1}} { }

/*
 * constructor for euclidean default multipliers
 */
Matrix::Matrix(
        const std::map<std::pair<Coordinate, Coordinate>, Id> &euclidean_data,
        Multiplier multiplier) :
    Base_Matrix(euclidean_data, multiplier),
    m_multipliers{{0, 1}} { }


/**
 * @param[in] i,j original identifiers
 * @param[in] date_time_of_departure_from_i time of departure from i
 *
 * @return infinity When the original identifiers do not exist on the matrix
 * @return the cell content
 *
 @dot
 digraph G {
    compound=true
    graph [ranksep=0.002];
    node[fontsize=10, arrowsize=.0005];
    start [shape=Mdiamond];
        n0 [label="Matrix_travel_time", shape=rect, color=green, height=0.4]
        n1  [label="Are identifiers valid?",shape=cds, color=blue, height=0.4];
        n2  [label="Return stored information",shape=rect, height=0.2];
        n3  [label="Return infinity",shape=rect, height=0.2];
    start -> n0 -> n1;
    n1:e -> n3 [label="no"]
    n3 -> end;
    n1 -> n2;
    n2 -> end;
    end  [shape=Mdiamond];
    error [shape=Mdiamond,color=red,label="caller error",fontcolor=red];
}
 @enddot
 */
TInterval
Matrix::travel_time(Id i, Id j, TTimestamp date_time_of_departure_from_i) const {
    if (m_multipliers.size() == 1) return at(get_index(i), get_index(j));

    /*
     * are ids valid?
     *
     * no -> return infinity
     */
    if (!has_id(i) || !has_id(j)) return (std::numeric_limits<TInterval>::max)();

    /* data */
    double tt_i_j {static_cast<double>(at(get_index(i), get_index(j)))};
    double c1(get_tdm(m_multipliers, date_time_of_departure_from_i));
    double c2(next_tdm(m_multipliers, date_time_of_departure_from_i));
    auto t_change(time_change(m_multipliers, date_time_of_departure_from_i));
    auto adjusted_value = static_cast<TInterval>(tt_i_j * c1);

    if (t_change >= (date_time_of_departure_from_i + adjusted_value) || c1 == c2) return adjusted_value;

    return static_cast<TInterval>(tt_i_j * std::min(c1, c2));
}

std::string
Matrix::multipliers_str() const {
    std::ostringstream log;
    log << std::fixed << std::setprecision(2);
    for (const auto &e : m_multipliers) {
        log << "\n" << std::get<0>(e) << "\t" << std::get<1>(e);
    }
    return log.str();
}

}  // namespace problem
}  // namespace vrprouting
