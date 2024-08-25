/*PGR-GNU*****************************************************************

File: pgdata_fetchers.hpp

Copyright (c) 2024 pgRouting developers
Mail: pgrouting-dev@discourse.osgeo.org

Developer:
Copyright (c) 2024 Celia Virginia Vergara Castillo
Mail: vicky at erosion.dev

----

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

#ifndef INCLUDE_CPP_COMMON_PGDATA_FETCHERS_HPP_
#define INCLUDE_CPP_COMMON_PGDATA_FETCHERS_HPP_
#pragma once

/* for HeapTuple, TupleDesc */
extern "C" {
#include <postgres.h>
#include <access/htup_details.h>
}

#include <vector>
#include "cpp_common/undefPostgresDefine.hpp"

namespace vrprouting {

class Info;
class Matrix_cell_t;
class Orders_t;
class Vehicle_t;
class Time_multipliers_t;
class Vroom_break_t;
class Vroom_job_t;
class Vroom_matrix_t;
class Vroom_shipment_t;
class Vroom_time_window_t;
class Vroom_vehicle_t;

namespace pgget {


namespace pickdeliver {

Matrix_cell_t fetch_matrix(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Orders_t fetch_orders(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Time_multipliers_t fetch_timeMultipliers(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Vehicle_t fetch_vehicles(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);

}  // namespace pickdeliver

namespace vroom {

Vroom_matrix_t fetch_matrix(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Vroom_time_window_t fetch_timewindows(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Vroom_job_t fetch_jobs(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Vroom_break_t fetch_breaks(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Vroom_shipment_t fetch_shipments(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);
Vroom_vehicle_t fetch_vehicles(const HeapTuple, const TupleDesc&, const std::vector<Info>&, bool);

}  // namespace vroom

}  // namespace pgget
}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_PGDATA_FETCHERS_HPP_
