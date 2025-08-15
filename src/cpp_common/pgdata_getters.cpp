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
