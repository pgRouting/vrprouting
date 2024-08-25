/*PGR-GNU*****************************************************************

File: pgdata_fetchers.cpp

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

#include "cpp_common/pgdata_fetchers.hpp"

#include <structures/vroom/input/input.h>
#include <structures/vroom/job.h>
#include <structures/vroom/vehicle.h>

#include <string>
#include <climits>
#include <vector>

#include "cpp_common/info.hpp"
#include "cpp_common/check_get_data.hpp"

#include "cpp_common/orders_t.hpp"
#include "cpp_common/matrix_cell_t.hpp"
#include "cpp_common/time_multipliers_t.hpp"
#include "cpp_common/vehicle_t.hpp"

#include "cpp_common/vroom_break_t.hpp"
#include "cpp_common/vroom_job_t.hpp"
#include "cpp_common/vroom_matrix_t.hpp"
#include "cpp_common/vroom_shipment_t.hpp"
#include "cpp_common/vroom_time_window_t.hpp"
#include "cpp_common/vroom_vehicle_t.hpp"

namespace {

void check_pairs(vrprouting::Info lhs, vrprouting::Info rhs) {
    if (!(vrprouting::column_found(lhs)) && vrprouting::column_found(rhs)) {
        throw std::string("Column found: '") + rhs.name + "', missing column: '" + lhs.name + "'";
    } else if (!(vrprouting::column_found(rhs)) && vrprouting::column_found(lhs)) {
        throw std::string("Column found: '") + lhs.name + "', missing column: '" + rhs.name + "'";
    }
}

}  // namespace

namespace vrprouting {
namespace pgget {

namespace vroom {

Vroom_break_t
fetch_breaks(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Vroom_break_t vroom_break;
    vroom_break.id = get_value<Idx>(tuple, tupdesc, info[0], 0);
    vroom_break.vehicle_id = get_value<Idx>(tuple, tupdesc, info[1], 0);
    vroom_break.service = get_value<Duration>(tuple, tupdesc, info[2], 0);
    vroom_break.data = get_jsonb(tuple, tupdesc, info[3]);
    return vroom_break;
}

Vroom_matrix_t
fetch_matrix(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Vroom_matrix_t matrix;
    matrix.start_id = get_value<MatrixIndex>(tuple, tupdesc, info[0], -1);
    matrix.end_id = get_value<MatrixIndex>(tuple, tupdesc, info[1], -1);
    matrix.duration = get_value<Duration>(tuple, tupdesc, info[2], 0);
    matrix.cost = get_value<TravelCost>(tuple, tupdesc, info[3], matrix.duration);
    return matrix;
}

Vroom_time_window_t
fetch_timewindows(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool is_shipment) {
    Vroom_time_window_t time_window;

    time_window.id = get_value<Idx>(tuple, tupdesc, info[0], 0);
    time_window.kind = is_shipment? get_char(tuple, tupdesc, info[3], ' ') : ' ';

    if (is_shipment) {
        if (time_window.kind != 'p' && time_window.kind != 'd') {
            throw std::string("Invalid kind '") + time_window.kind + "', Expecting 'p' or 'd'";
        }
    }

    Duration tw_open = get_value<Duration>(tuple, tupdesc, info[1], 0);
    Duration tw_close = get_value<Duration>(tuple, tupdesc, info[2], 0);

    if (tw_open > tw_close) {
        throw std::string("Invalid time window found: '") + info[2].name + "' < '" + info[1].name + "'";
    }

    time_window.tw = ::vroom::TimeWindow(tw_open, tw_close);

    return time_window;
}

Vroom_job_t
fetch_jobs(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Vroom_job_t job;

    job.id = get_value<Idx>(tuple, tupdesc, info[0], 0);
    job.location_id = get_value<MatrixIndex>(tuple, tupdesc, info[1], 0);

    job.setup = get_value<Duration>(tuple, tupdesc, info[2], 0);
    job.service = get_value<Duration>(tuple, tupdesc, info[3], 0);

    auto pickup = get_array<Amount>(tuple, tupdesc, info[5]);
    auto delivery = get_array<Amount>(tuple, tupdesc, info[4]);

    for (const auto &e : pickup) {
        job.pickup.push_back(e);
    }

    for (const auto &e : delivery) {
        job.delivery.push_back(e);
    }

    job.skills = get_uint_unordered_set(tuple, tupdesc, info[6]);
    job.priority = get_value<Priority>(tuple, tupdesc, info[7], 0);
    job.data = get_jsonb(tuple, tupdesc, info[8]);

    if (job.priority > 100) {
        throw std::string("Invalid value in column '") + info[7].name + "'. Maximum value allowed 100";
    }
    return job;
}

Vroom_shipment_t
fetch_shipments(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Vroom_shipment_t shipment;

    shipment.id = get_value<Idx>(tuple, tupdesc, info[0], 0);

    shipment.p_location_id = get_value<MatrixIndex>(tuple, tupdesc, info[1], 0);
    shipment.d_location_id = get_value<MatrixIndex>(tuple, tupdesc, info[4], 0);

    shipment.p_setup = get_value<Duration>(tuple, tupdesc, info[2], 0);
    shipment.p_service = get_value<Duration>(tuple, tupdesc, info[3], 0);
    shipment.d_setup = get_value<Duration>(tuple, tupdesc, info[5], 0);
    shipment.d_service = get_value<Duration>(tuple, tupdesc, info[6], 0);

    auto amount = get_array<Amount>(tuple, tupdesc, info[7]);

    for (const auto &a : amount) {
        shipment.amount.push_back(a);
    }

    shipment.skills = get_uint_unordered_set(tuple, tupdesc, info[8]);

    shipment.priority = get_value<Priority>(tuple, tupdesc, info[9], 0);

    shipment.p_data = get_jsonb(tuple, tupdesc, info[10]);
    shipment.d_data = get_jsonb(tuple, tupdesc, info[11]);

    if (shipment.priority > 100) {
        throw std::string("Invalid value in column '") + info[9].name + "'. Maximum value allowed 100";
    }
    return shipment;
}

Vroom_vehicle_t
fetch_vehicles(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Vroom_vehicle_t vehicle;
    vehicle.id = get_value<Idx>(tuple, tupdesc, info[0], 0);
    vehicle.start_id = get_value<MatrixIndex>(tuple, tupdesc, info[1], -1);
    vehicle.end_id = get_value<MatrixIndex>(tuple, tupdesc, info[2], -1);

    auto capacity = get_array<Amount>(tuple, tupdesc, info[3]);

    for (const auto &c : capacity) {
        vehicle.capacity.push_back(c);
    }

    vehicle.skills = get_uint_unordered_set(tuple, tupdesc, info[4]);

    Duration tw_open = get_value<Duration>(tuple, tupdesc, info[5], 0);
    Duration tw_close = get_value<Duration>(tuple, tupdesc, info[6], UINT_MAX);

    if (tw_open > tw_close) {
        throw std::string("Invalid time window found: '") + info[6].name + "' < '" + info[5].name + "'";
    }

    vehicle.tw = ::vroom::TimeWindow(tw_open, tw_close);

    vehicle.speed_factor = get_anynumerical(tuple, tupdesc, info[7], 1.0);
    vehicle.max_tasks = get_value<int32_t>(tuple, tupdesc, info[8], INT_MAX);
    vehicle.data = get_jsonb(tuple, tupdesc, info[9]);

    if (!(column_found(info[1]) || column_found(info[2]))) {
        throw std::string("Missing column(s): '") + info[1].name + "' and/or '" + info[2].name + "' must exist";
    }

    if (vehicle.speed_factor <= 0.0) {
        throw std::string("Invalid negative or zero value in column '") + info[7].name + "'";
    }
    return vehicle;
}


}  // namespace vroom

namespace pickdeliver {

Matrix_cell_t
fetch_matrix(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Matrix_cell_t row;
    row.from_vid = get_value<Id>(tuple, tupdesc,  info[0], -1);
    row.to_vid = get_value<Id>(tuple, tupdesc,  info[1], -1);
    row.cost = get_value<TInterval>(tuple, tupdesc, info[2], 0);
    return row;
}

Time_multipliers_t
fetch_timeMultipliers(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool) {
    Time_multipliers_t row;
    row.start_time = get_value<TTimestamp>(tuple, tupdesc, info[0], 0);
    row.multiplier = get_anynumerical(tuple, tupdesc,  info[1], 1);
    return row;
}

Orders_t
fetch_orders(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool is_euclidean) {
    Orders_t pd_order;

    if (is_euclidean) {
        check_pairs(info[3], info[4]);
        check_pairs(info[9], info[10]);
    }

    pd_order.id = get_value<Id>(tuple, tupdesc, info[0], -1);
    pd_order.demand = get_value<PAmount>(tuple, tupdesc, info[1], 0);
    if (pd_order.demand == 0) {
        throw std::make_pair(
                std::string("Unexpected zero value found on column'") + info[1].name + "' of orders",
                std::string("Check order id #:") + std::to_string(pd_order.id));
    }

    pd_order.pick_node_id = is_euclidean? 0 : get_value<Id>(tuple, tupdesc, info[2], -1);
    pd_order.pick_x = is_euclidean? get_anynumerical(tuple, tupdesc, info[3], 0) : 0;
    pd_order.pick_y = is_euclidean? get_anynumerical(tuple, tupdesc, info[4], 0) : 0;
    pd_order.pick_open_t    = get_value<TTimestamp>(tuple, tupdesc, info[5], -1);
    pd_order.pick_close_t   = get_value<TTimestamp>(tuple, tupdesc, info[6], -1);
    if (pd_order.pick_close_t < pd_order.pick_open_t) {
        throw std::make_pair(
                std::string("Invalid time window found: '") + info[6].name + "' < '" + info[5].name + "'",
                std::string("Check order id #:") + std::to_string(pd_order.id));
    }

    pd_order.pick_service_t = get_value<TInterval>(tuple, tupdesc, info[7], 0);
    if (pd_order.pick_service_t < 0) {
        throw std::make_pair(
                std::string("Unexpected negative value found on column'") + info[7].name + "' of orders",
                std::string("Check order id #:") + std::to_string(pd_order.id));
    }

    pd_order.deliver_node_id   = is_euclidean? 0 : get_value<Id>(tuple, tupdesc, info[8], -1);
    pd_order.deliver_x =   is_euclidean? get_anynumerical(tuple, tupdesc, info[9], 0) : 0;
    pd_order.deliver_y =   is_euclidean? get_anynumerical(tuple, tupdesc, info[10], 0) : 0;
    pd_order.deliver_open_t    = get_value<TTimestamp>(tuple, tupdesc, info[11], -1);
    pd_order.deliver_close_t   = get_value<TTimestamp>(tuple, tupdesc, info[12], -1);
    if (pd_order.deliver_close_t < pd_order.deliver_open_t) {
        throw std::make_pair(
                std::string("Invalid time window found: '") + info[12].name + "' < '" + info[11].name + "'",
                std::string("Check order id #:") + std::to_string(pd_order.id));
    }

    pd_order.deliver_service_t = get_value<TInterval>(tuple, tupdesc, info[13], 0);
    if (pd_order.pick_service_t < 0) {
        throw std::make_pair(
                std::string("Unexpected negative value found on column'") + info[13].name + "' of orders",
                std::string("Check order id #:") + std::to_string(pd_order.id));
    }

    return pd_order;
}

Vehicle_t
fetch_vehicles(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const std::vector<Info> &info,
        bool is_euclidean) {
    Vehicle_t vehicle;

    if (is_euclidean) {
        check_pairs(info[5], info[6]);
        check_pairs(info[11], info[12]);
    }

    vehicle.id = get_value<Id>(tuple, tupdesc, info[0], -1);
    vehicle.capacity = get_value<PAmount>(tuple, tupdesc, info[1], UINT32_MAX);
    vehicle.cant_v =  get_value<PAmount>(tuple, tupdesc, info[2], 1);
    vehicle.speed  =  get_anynumerical(tuple, tupdesc, info[3], 1);

    /*
     * start values
     */
    vehicle.start_node_id = is_euclidean? 0 : get_value<Id>(tuple, tupdesc, info[4], -1);
    vehicle.start_x = is_euclidean? get_anynumerical(tuple, tupdesc, info[5], 0) : 0;
    vehicle.start_y = is_euclidean? get_anynumerical(tuple, tupdesc, info[6], 0) : 0;
    vehicle.start_open_t = get_value<TTimestamp>(tuple, tupdesc, info[7], 0);
    vehicle.start_close_t = get_value<TTimestamp>(tuple, tupdesc, info[8], INT64_MAX);
    vehicle.start_service_t = get_value<TInterval>(tuple, tupdesc, info[9], 0);

    /*
     * end values
     */
    vehicle.end_node_id   = is_euclidean? 0 : get_value<Id>(tuple, tupdesc, info[10], vehicle.start_node_id);
    vehicle.end_x =   is_euclidean? get_anynumerical(tuple, tupdesc, info[11], vehicle.start_x) : 0;
    vehicle.end_y =   is_euclidean? get_anynumerical(tuple, tupdesc, info[12], vehicle.start_y) : 0;
    vehicle.end_open_t = get_value<TTimestamp>(tuple, tupdesc, info[13], vehicle.start_open_t);
    vehicle.end_close_t = get_value<TTimestamp>(tuple, tupdesc, info[14], vehicle.start_close_t);
    vehicle.end_service_t   = get_value<TInterval>(tuple, tupdesc, info[15], 0);

    vehicle.stops = get_array<Id>(tuple, tupdesc, info[16]);

    return vehicle;
}

}   // namespace pickdeliver

}   // namespace pgget
}   // namespace vrprouting
