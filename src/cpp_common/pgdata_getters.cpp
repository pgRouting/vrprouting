/*PGR-GNU*****************************************************************

File: pgdata_getters.cpp

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

#include "cpp_common/pgdata_getters.hpp"

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "cpp_common/get_data.hpp"
#include "cpp_common/check_get_data.hpp"
#include "cpp_common/pgdata_fetchers.hpp"
#include "cpp_common/info.hpp"

namespace vrprouting {
namespace pgget {


namespace vroom {
/**
  ~~~~{.c}
  SELECT start_id, end_id, duration, cost
  FROM matrix;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param[in] use_timestamps When true postgres Time datatypes are used
 * @returns vector of Vroom_matrix_t containing the matrix cell contents
 */
std::vector<Vroom_matrix_t>
get_matrix(
        const std::string &sql,
        bool use_timestamps) {
    using vrprouting::Info;
    std::vector<Info> info{
        {-1, 0, true, "start_id", vrprouting::MATRIX_INDEX},
        {-1, 0, true, "end_id", vrprouting::MATRIX_INDEX},
        {-1, 0, true, "duration", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "cost", vrprouting::INTEGER}};

    return pgget::get_data<Vroom_matrix_t>(sql, use_timestamps, info, &fetch_matrix);
}

/**
  ~~~~{.c}
  SELECT id, vehicle_id, service, data
  FROM breaks;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param [in] use_timestamps When true postgres Time datatypes are used
 * @returns vector of Vroom_break_t containing the breaks information
 */
std::vector<Vroom_break_t>
get_breaks(
        const std::string &sql,
        bool use_timestamps) {
    if (sql.empty()) return std::vector<Vroom_break_t>();
    using vrprouting::Info;
    std::vector<Info> info{
        {-1, 0, true, "id", vrprouting::IDX},
        {-1, 0, true, "vehicle_id", vrprouting::IDX},
        {-1, 0, false, "service", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "data", vrprouting::JSONB}};

    return pgget::get_data<Vroom_break_t>(sql, use_timestamps, info, &fetch_breaks);
}

/**
  ~~~~{.c}
  SELECT id, tw_open, tw_close, kind
  FROM shipment_tws;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param [in] use_timestamps When true postgres Time datatypes are used
 * @param [in] is_shipment When true c$the kind is compulsory
 * @returns vector of Vroom_time_window_t containing the time windows information
 */
std::map<std::pair<Idx, char>, std::vector<::vroom::TimeWindow>>
get_timewindows(
        const std::string &sql,
        bool use_timestamps,
        bool is_shipment) {
    using vrprouting::Info;
    std::map<std::pair<Idx, char>, std::vector<::vroom::TimeWindow>> time_windows;
    if (sql.empty()) return time_windows;

    std::vector<Info> info{
        {-1, 0, true, "id", vrprouting::ANY_INTEGER},
        {-1, 0, true, "tw_open", use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, true, "tw_close", use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, is_shipment, "kind", vrprouting::CHAR1}};

    auto data = pgget::get_data<Vroom_time_window_t>(sql, is_shipment, info, &fetch_timewindows);

    for (const auto &tw : data) {
       time_windows[std::make_pair(tw.id, tw.kind)];
       time_windows[std::make_pair(tw.id, tw.kind)].push_back(tw.tw);
    }
    return time_windows;
}

/**
  ~~~~{.c}
  SELECT id, location_id, setup, service, delivery, delivery, skills, priority, data
  FROM jobs;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param [in] use_timestamps When true postgres Time datatypes are used
 * @returns vector of Vroom_job_t containing the jobs information
 */
std::vector<Vroom_job_t> get_jobs(
        const std::string &sql,
        bool use_timestamps) {
    if (sql.empty()) return std::vector<Vroom_job_t>();
    using vrprouting::Info;
    std::vector<Info> info {
        {-1, 0, true, "id", vrprouting::IDX},
        {-1, 0, true, "location_id", vrprouting::MATRIX_INDEX},
        {-1, 0, false, "setup",   use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "service", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "delivery", vrprouting::ANY_POSITIVE_ARRAY},
        {-1, 0, false, "pickup", vrprouting::ANY_POSITIVE_ARRAY},
        {-1, 0, false, "skills", vrprouting::ANY_UINT_ARRAY},
        {-1, 0, false, "priority", vrprouting::POSITIVE_INTEGER},
        {-1, 0, false, "data", vrprouting::JSONB}};

    return pgget::get_data<Vroom_job_t>(sql, use_timestamps, info, &fetch_jobs);
}

/**
  ~~~~{.c}
  SELECT id,
  p_location_id, p_setup, p_service, p_data,
  d_location_id, d_setup, d_service, d_data,
  amount, skills, priority, data
  FROM jobs;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param[in] use_timestamps When true postgres Time datatypes are used
 * @returns vector of Vroom_shipment_t containing the shipment information
 */
std::vector<Vroom_shipment_t>
get_shipments(
        const std::string &sql,
        bool use_timestamps) {
    if (sql.empty()) return std::vector<Vroom_shipment_t>();
    using vrprouting::Info;
    std::vector<Info> info{
        {-1, 0, true, "id", vrprouting::IDX},
        {-1, 0, true, "p_location_id", vrprouting::MATRIX_INDEX},
        {-1, 0, false, "p_setup", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "p_service", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, true, "d_location_id", vrprouting::MATRIX_INDEX},
        {-1, 0, false, "d_setup", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "d_service", use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},
        {-1, 0, false, "amount", vrprouting::ANY_POSITIVE_ARRAY},
        {-1, 0, false, "skills", vrprouting::ANY_UINT_ARRAY},
        {-1, 0, false, "priority", vrprouting::POSITIVE_INTEGER},
        {-1, 0, false, "p_data", vrprouting::JSONB},
        {-1, 0, false, "d_data", vrprouting::JSONB}};

    return pgget::get_data<Vroom_shipment_t>(sql, use_timestamps, info, &fetch_shipments);
}

/**
  ~~~~{.c}
  SELECT id,
  start_id, end_id, capacity,
  skills, tw_open, tw_close, speed_factor, max_tasks, data
  FROM jobs;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param[in] use_timestamps When true postgres Time datatypes are used
 * @returns vector of Vroom_vehicle_t containing the vehicle information
 */
std::vector<Vroom_vehicle_t>
get_vehicles(
        const std::string &sql,
        bool use_timestamps) {
    using vrprouting::Info;
    std::vector<Info> info{
        {-1, 0, true, "id", vrprouting::IDX},
        {-1, 0, false, "start_id", vrprouting::MATRIX_INDEX},
        {-1, 0, false, "end_id", vrprouting::MATRIX_INDEX},
        {-1, 0, false, "capacity", vrprouting::ANY_POSITIVE_ARRAY},
        {-1, 0, false, "skills", vrprouting::ANY_UINT_ARRAY},
        {-1, 0, false, "tw_open", use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false, "tw_close", use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false, "speed_factor", vrprouting::ANY_NUMERICAL},
        {-1, 0, false, "max_tasks", vrprouting::POSITIVE_INTEGER},
        {-1, 0, false, "data", vrprouting::JSONB}};

    return pgget::get_data<Vroom_vehicle_t>(sql, use_timestamps, info, &fetch_vehicles);
}

}  // namespace vroom


namespace pickdeliver {

std::vector<Time_multipliers_t> get_timeMultipliers(
        const std::string &sql,
        bool use_timestamps) {
    using vrprouting::Info;
    std::vector<Info> info{
        {-1, 0, true,
            use_timestamps? "start_time" :  "start_value",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, true, "multiplier", vrprouting::ANY_NUMERICAL}};

    return pgget::get_data<Time_multipliers_t>(sql, use_timestamps, info, &fetch_timeMultipliers);
}

/**
  ~~~~{.c}
  SELECT start_vid, end_vid, [travel_time|agg_cost]
  FROM matrix;
  ~~~~
 * @param[in] sql SQL query to execute
 * @param [in] use_timestamps When true postgres Time datatypes are used
 * @returns vector of Matrix_cell_t containing the matrix cell contents
 */
std::vector<Matrix_cell_t> get_matrix(
        const std::string &sql,
        bool use_timestamps) {
    using vrprouting::Info;
    std::vector<Info> info{
        {-1, 0, true, "start_vid", vrprouting::ID},
        {-1, 0, true, "end_vid", vrprouting::ID},
        {-1, 0, true,
            use_timestamps? "travel_time" : "agg_cost",
            use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL}};

    return pgget::get_data<Matrix_cell_t>(sql, use_timestamps, info, &fetch_matrix);
}


/**
  For queries comming from pgRouting
  ~~~~{.c}
  SELECT id, demand
  [p_id | p_x, p_y], p_open, p_close, p_service,
  [d_id | d_x, d_y], d_open, d_close, d_service,
  FROM orders;
  ~~~~
  For timestamps
  ~~~~{.c}
  SELECT id, demand
  [p_id | p_x, p_y], p_tw_open, p_tw_close, p_t_service,
  [d_id | d_x, d_y], d_tw_open, d_tw_close, d_t_service,
  FROM orders;
  ~~~~

  @param[in] sql The orders query
  @param [in]  is_euclidean When true coordintes are going to be used
  @param [in]  use_timestamps When true data use postgres timestamps
  @returns vector of Orders_t
  */
std::vector<Orders_t> get_orders(
        const std::string &sql,
        bool is_euclidean,
        bool use_timestamps) {
    using vrprouting::Info;

    std::vector<Info> info{
        {-1, 0, true, "id", vrprouting::ID},
        {-1, 0, true, "amount", vrprouting::PAMOUNT},

        {-1, 0, !is_euclidean, "p_id", vrprouting::ID},
        {-1, 0, is_euclidean, "p_x", vrprouting::COORDINATE},
        {-1, 0, is_euclidean, "p_y", vrprouting::COORDINATE},

        {-1, 0, true,
            use_timestamps? "p_tw_open" : "p_open",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, true,
            use_timestamps? "p_tw_close" : "p_close",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false,
            use_timestamps? "p_t_service" : "p_service",
            use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},

        {-1, 0, !is_euclidean, "d_id", vrprouting::ID},
        {-1, 0, is_euclidean, "d_x", vrprouting::COORDINATE},
        {-1, 0, is_euclidean, "d_y", vrprouting::COORDINATE},
        {-1, 0, true,
            use_timestamps? "d_tw_open" : "d_open",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, true,
            use_timestamps? "d_tw_close" : "d_close",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false,
            use_timestamps? "d_t_service" : "d_service",
            use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL}};

    return pgget::get_data<Orders_t>(sql, is_euclidean, info, &fetch_orders);
}


/**

  For queries of the type:
  ~~~~{.c}
  SELECT id, capacity, speed, number
  [s_id | s_x, s_y], s_open, s_close, s_service,
  [e_id | e_x, e_y], e_open, e_close, e_service,
  FROM orders;
  ~~~~

  for timestamps:
  ~~~~{.c}
  SELECT id, capacity, speed, number
  [s_id | s_x, s_y], s_tw_open, s_tw_close, s_t_service,
  [e_id | e_x, e_y], e_tw_open, e_tw_close, e_t_service,
  FROM orders;
  ~~~~

  @param[in] sql The vehicles query
  @param[in] is_euclidean when true: use coordinates
  @param[in] use_timestamps When true data use postgres timestamps
  @param[in] with_stops use stops information otherwise ignore
  @returns vector of Vehicle_t
  */
std::vector<Vehicle_t> get_vehicles(
        const std::string &sql,
        bool is_euclidean,
        bool use_timestamps,
        bool with_stops) {
    using vrprouting::Info;

    std::vector<Info> info{
        {-1, 0, true, "id", vrprouting::ID},
        {-1, 0, true, "capacity", vrprouting::PAMOUNT},
        {-1, 0, false, "number", vrprouting::PAMOUNT},
        {-1, 0, false, "speed", vrprouting::SPEED},

        {-1, 0, !is_euclidean, "s_id", vrprouting::ID},
        {-1, 0, is_euclidean, "s_x", vrprouting::COORDINATE},
        {-1, 0, is_euclidean, "s_y", vrprouting::COORDINATE},
        {-1, 0, false,
            use_timestamps? "s_tw_open" : "s_open",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false,
            use_timestamps? "s_tw_close" : "s_close",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false,
            use_timestamps? "s_t_service" : "s_service",
            use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},

        {-1, 0, false, "e_id", vrprouting::ID},
        {-1, 0, false, "e_x", vrprouting::COORDINATE},
        {-1, 0, false, "e_y", vrprouting::COORDINATE},
        {-1, 0, false,
            use_timestamps? "e_tw_open" : "e_open",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false,
            use_timestamps? "e_tw_close" : "e_close",
            use_timestamps? vrprouting::TIMESTAMP : vrprouting::TTIMESTAMP},
        {-1, 0, false,
            use_timestamps? "e_t_service" : "e_service",
            use_timestamps? vrprouting::INTERVAL : vrprouting::TINTERVAL},

        {-1, 0, with_stops, "stops", vrprouting::ANY_POSITIVE_ARRAY}};

    return get_data<Vehicle_t>(sql, is_euclidean, info, &fetch_vehicles);
}
}  // namespace pickdeliver

}  // namespace pgget
}  // namespace vrprouting
