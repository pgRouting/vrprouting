/*PGR-GNU*****************************************************************

File: pgdata_getters.hpp

Copyright (c) 2024 pgRouting developers
Mail: pgrouting-dev@discourse.osgeo.org

Developer:
Copyright (c) 2024 Celia Virginia Vergara Castillo
Mail: vicky at erosion.dev

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

#ifndef INCLUDE_CPP_COMMON_PGDATA_GETTERS_HPP_
#define INCLUDE_CPP_COMMON_PGDATA_GETTERS_HPP_
#pragma once

extern "C" {
#include <postgres.h>
#include <utils/array.h>
}

#include <cstddef>
#include <cstdint>
#include <string>
#include <set>
#include <vector>
#include <map>
#include "cpp_common/undefPostgresDefine.hpp"

#include "cpp_common/matrix_cell_t.hpp"
#include "cpp_common/orders_t.hpp"
#include "cpp_common/time_multipliers_t.hpp"
#include "cpp_common/vehicle_t.hpp"

namespace vrprouting {
namespace pgget {
namespace pickdeliver {

/** @brief Get the matrix */
std::vector<Matrix_cell_t> get_matrix(const std::string&, bool);

/** @brief Reads the pick-Deliver shipments for timestams and intervals*/
std::vector<Orders_t> get_orders(const std::string&, bool, bool);

/** @brief Reads the vehicles information */
std::vector<Vehicle_t> get_vehicles(const std::string&, bool, bool, bool);

/** @brief Get the time multipliers using interval*/
std::vector<Time_multipliers_t> get_timeMultipliers(const std::string&, bool);

}  // namespace pickdeliver

}  // namespace pgget
}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_PGDATA_GETTERS_HPP_
