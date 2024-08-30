/*PGR-GNU*****************************************************************
File: vroom.cpp

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2021 Ashish Kumar
Mail: ashishkr23438@gmail.com
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

#include "vroom/vroom.hpp"

#include <structures/vroom/input/input.h>
#include <structures/vroom/job.h>
#include <structures/vroom/vehicle.h>

#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>


#include "c_types/vroom_rt.h"
#include "cpp_common/matrix_cell_t.hpp"
#include "cpp_common/vroom_break_t.hpp"
#include "cpp_common/vroom_job_t.hpp"
#include "cpp_common/vroom_shipment_t.hpp"
#include "cpp_common/vroom_time_window_t.hpp"
#include "cpp_common/vroom_vehicle_t.hpp"

#include "cpp_common/vroom_matrix.hpp"
#include "cpp_common/interruption.hpp"
#include "cpp_common/messages.hpp"

namespace vrprouting {
namespace problem {

/*
 * param[in] jobs The vector container of Vroom_job_t
 * param[in] jobs_tws The vector container of Vroom_time_window_t
 */
void
Vroom::add_jobs(
        const std::vector<Vroom_job_t> &jobs,
        const std::map<std::pair<Idx, char>, std::vector<::vroom::TimeWindow>> &job_tws) {
    std::vector<::vroom::TimeWindow> default_tw(1, ::vroom::TimeWindow());
    auto default_amount = ::vroom::Amount(m_vehicles.size() ? m_vehicles[0].capacity.size() : 0);

    for (auto job : jobs) {
        auto job_tw = job_tws.find(std::make_pair(job.id, ' ')) == job_tws.end() ?
            default_tw
            : job_tws.at(std::make_pair(job.id, ' '));

        auto pickup = job.pickup.empty() ?  default_amount : job.pickup;
        auto delivery = job.delivery.empty() ?  default_amount : job.delivery;

        auto location_id = static_cast<::vroom::Index>(m_matrix.get_index(job.location_id));

        auto vjob = ::vroom::Job(
                job.id, location_id, job.setup, job.service,
                delivery, pickup, job.skills, job.priority,
                job_tw, job.data);
        m_jobs.push_back(vjob);
     }
}

/*
 * param[in] shipments The vector container of Vroom_shipment_t
 * param[in] shipments_tws Shipments time windows. The vector container of Vroom_time_window_t
 */
void
Vroom::add_shipments(
        const std::vector<Vroom_shipment_t> &shipments,
        const std::map<std::pair<Idx, char>, std::vector<::vroom::TimeWindow>> &shipments_tws) {
    std::vector<::vroom::TimeWindow> default_tw(1, ::vroom::TimeWindow());
    for (auto shipment : shipments) {
        auto pick_tw = shipments_tws.find(std::make_pair(shipment.id, 'p')) == shipments_tws.end() ?
            default_tw
            : shipments_tws.at(std::make_pair(shipment.id, 'p'));
        auto drop_tw = shipments_tws.find(std::make_pair(shipment.id, 'd')) == shipments_tws.end() ?
            default_tw
            : shipments_tws.at(std::make_pair(shipment.id, 'd'));

        auto p_location_id = static_cast<::vroom::Index>(m_matrix.get_index(shipment.p_location_id));
        auto d_location_id = static_cast<::vroom::Index>(m_matrix.get_index(shipment.d_location_id));
        auto pickup = ::vroom::Job(
                shipment.id, ::vroom::JOB_TYPE::PICKUP, p_location_id,
                shipment.p_setup, shipment.p_service, shipment.amount,
                shipment.skills, shipment.priority, pick_tw, shipment.p_data);
        auto delivery = ::vroom::Job(
                shipment.id, ::vroom::JOB_TYPE::DELIVERY, d_location_id,
                shipment.d_setup, shipment.d_service, shipment.amount,
                shipment.skills, shipment.priority, drop_tw, shipment.d_data);

        m_shipments.push_back(std::make_pair(pickup, delivery));
    }
}

/*
 * param[in] vehicles The vector container of Vroom_vehicle_t
 * param[in] breaks Vehicle breaks. The vector container of Vroom_break_t
 * param[in] breaks_tws_map Vehicle breaks time windows. The vector container of Vroom_time_window_t
 */
void
Vroom::add_vehicles(
        const std::vector<Vroom_vehicle_t> &vehicles,
        const std::vector<Vroom_break_t> &breaks,
        const std::map<std::pair<Idx, char>, std::vector<::vroom::TimeWindow>> &breaks_tws) {
    std::vector<::vroom::TimeWindow> default_tw(1, ::vroom::TimeWindow());

    std::map<Idx, std::vector<Vroom_break_t>> v_breaks_map;
    for (auto v_break : breaks) {
        Idx v_id = v_break.vehicle_id;
        if (v_breaks_map.find(v_id) == v_breaks_map.end()) {
            v_breaks_map[v_id] = std::vector<Vroom_break_t>();
        }
        v_breaks_map[v_id].push_back(v_break);
    }

    auto default_capacity = ::vroom::Amount(m_vehicles.size() ? m_vehicles[0].capacity.size() : 0);

    for (auto vehicle : vehicles) {
        std::vector<Vroom_break_t> vehicle_breaks = v_breaks_map[vehicle.id];
        /*
         * Builds the vroom::Break collection
         */
        std::vector<::vroom::Break> vroom_breaks;
        for (const auto &vb : vehicle_breaks) {
            auto tws = breaks_tws.find(std::make_pair(vb.id, ' ')) == breaks_tws.end() ?
                default_tw
                : breaks_tws.at(std::make_pair(vb.id, ' '));
            vroom_breaks.push_back(::vroom::Break(vb.id, tws, vb.service, vb.data));
        }

        auto capacity = vehicle.capacity.empty() ?  default_capacity : vehicle.capacity;

        std::optional<::vroom::Location> start_id;
        std::optional<::vroom::Location> end_id;
        // Set the value of start or end index only if they are present
        if (vehicle.start_id != -1) {
            start_id = static_cast<::vroom::Index>(m_matrix.get_index(vehicle.start_id));
        }
        if (vehicle.end_id != -1) {
            end_id = static_cast<::vroom::Index>(m_matrix.get_index(vehicle.end_id));
        }

        auto vroom_vehicle = ::vroom::Vehicle(
                vehicle.id, start_id, end_id,
                ::vroom::DEFAULT_PROFILE, capacity, vehicle.skills, vehicle.tw,
                vroom_breaks, vehicle.data, vehicle.speed_factor,
                static_cast<size_t>(vehicle.max_tasks));
        m_vehicles.push_back(vroom_vehicle);
    }
}

/*
 * param[in] matrix The matrix
 */
void
Vroom::add_matrix(const vrprouting::vroom::Matrix &matrix) {
    m_matrix = matrix;
}

void
Vroom::get_amount(const ::vroom::Amount &vroom_amount, Amount **amount) {
    size_t amount_size = vroom_amount.size();
    for (size_t i = 0; i < amount_size; i++) {
        *((*amount) + i) = vroom_amount[i];
    }
}

StepType
Vroom::get_job_step_type(const ::vroom::JOB_TYPE &vroom_job_type) {
    StepType step_type;
    switch (vroom_job_type) {
        case ::vroom::JOB_TYPE::SINGLE:
            step_type = 2;
            break;
        case ::vroom::JOB_TYPE::PICKUP:
            step_type = 3;
            break;
        case ::vroom::JOB_TYPE::DELIVERY:
            step_type = 4;
            break;
    }
    return step_type;
}

StepType
Vroom::get_step_type(const ::vroom::Step &step) {
    StepType step_type = 0;
    switch (step.step_type) {
        case ::vroom::STEP_TYPE::START:
            step_type = 1;
            break;
        case ::vroom::STEP_TYPE::END:
            step_type = 6;
            break;
        case ::vroom::STEP_TYPE::BREAK:
            step_type = 5;
            break;
        case ::vroom::STEP_TYPE::JOB:
            step_type = get_job_step_type(step.job_type);
            break;
    }
    return step_type;
}

std::vector<Vroom_rt>
Vroom::get_results(const ::vroom::Solution &solution) {
    std::vector<Vroom_rt> results;
    auto routes = solution.routes;
    Idx vehicle_seq = 1;
    char *empty_desc = strdup("{}");
    for (auto route : routes) {
        Idx step_seq = 1;
        Duration prev_duration = 0;
        char *vehicle_data = strdup(route.description.c_str());
        for (auto step : route.steps) {
            Idx task_id = step.id;
            MatrixIndex location_id = m_matrix.get_original_id(step.location.index());
            char *task_data = strdup(step.description.c_str());
            StepType step_type = get_step_type(step);
            if (step_type == 1 || step_type == 6) {
                task_id = static_cast<Idx>(-1);
                task_data = empty_desc;
            }

            size_t load_size = step.load.size();
            Amount *load = reinterpret_cast<Amount*>(malloc(load_size * sizeof(Amount)));
            get_amount(step.load, &load);

            Duration travel_time = step.duration - prev_duration;
            prev_duration = step.duration;
            Duration departure = step.arrival + step.setup + step.service + step.waiting_time;
            results.push_back({
                    vehicle_seq,        // vehicles_seq
                    route.vehicle,      // vehicles_id
                    vehicle_data,       // vehicle_data
                    step_seq,           // step_seq
                    step_type,          // step_type
                    task_id,            // task_id
                    location_id,        // location_id
                    task_data,          // task_data
                    step.arrival,       // arrival
                    travel_time,        // travel_time
                    step.setup,         // setup_time
                    step.service,       // service_time
                    step.waiting_time,  // waiting_time
                    departure,          // departure
                    load,               // load
                    load_size           // load size
                    });
            step_seq++;
        }
        // The summary of this route
        Idx task_id = 0;
        results.push_back({
                vehicle_seq,         // vehicles_seq
                route.vehicle,       // vehicles_id
                vehicle_data,        // vehicle_data
                0,                   // step_seq = 0 for route summary
                0,                   // step_type = 0 for route summary
                task_id,             // task_id = 0 for route summary
                0,                   // location_id = 0 for route summary
                empty_desc,          // task_data
                0,                   // No arrival time
                route.duration,      // Total travel time
                route.setup,         // Total setup time
                route.service,       // Total service time
                route.waiting_time,  // Total waiting time
                0,                   // No departure time
                {},                  // load
                0                    // load size
                });
        vehicle_seq++;
    }

    auto unassigned = solution.unassigned;
    Idx step_seq = 1;
    for (auto job : unassigned) {
        StepType job_step = get_job_step_type(job.type);
        Idx vehicle_id = static_cast<Idx>(-1);
        Idx job_id = job.id;
        auto location_id = m_matrix.get_original_id(job.location.index());
        char *task_data = strdup(job.description.c_str());
        results.push_back({
                vehicle_seq,      // vehicles_seq
                vehicle_id,       // vehicles_id = -1 for unassigned jobs
                empty_desc,       // vehicle_data
                step_seq,         // step_seq
                job_step,         // step_type
                job_id,           // task_id
                location_id,      // location_id
                task_data,        // task_data
                0,                // No arrival time
                0,                // No travel_time
                0,                // No setup_time
                0,                // No service_time
                0,                // No waiting_time
                0,                // No departure time
                {},               // load
                0                 // load size
                });
        step_seq++;
    }

    // The summary of the entire problem
    auto summary = solution.summary;
    Idx vehicle_id = 0;
    Idx job_id = 0;
    results.push_back({
            0,                     // vehicles_seq = 0 for problem summary
            vehicle_id,            // vehicles_id = 0 for problem summary
            empty_desc,            // vehicle_data
            0,                     // step_seq = 0 for problem summary
            0,                     // step_type = 0 for problem summary
            job_id,                // task_id = 0 for problem summary
            0,                     // location_id = 0 for problem summary
            empty_desc,            // task_data
            0,                     // No arrival time
            summary.duration,      // Total travel time
            summary.setup,         // Total setup time
            summary.service,       // Total service time
            summary.waiting_time,  // Total waiting time
            0,                     // No departure time
            {},                    // load
            0                      // load size
            });
    return results;
}

/*
 * param[in] exploration_level
 * param[in] timeout
 * param[in] loading_timex
 *
 * @returns The vroom results. A vector of Vroom_rt
 */
std::vector<Vroom_rt>
Vroom::solve(
        int32_t exploration_level,
        int32_t timeout,
        int64_t loading_time) {
    std::vector<Vroom_rt> results;

    /* abort in case an interruption occurs (e.g. the query is being cancelled) */
    CHECK_FOR_INTERRUPTS();
    try {
        const unsigned int amount_size = m_vehicles.size() ?
            static_cast<unsigned int>(m_vehicles[0].capacity.size())
            : 0;
        ::vroom::Input problem_instance;
        problem_instance.set_amount_size(amount_size);

        for (const auto &vehicle : m_vehicles) {
            problem_instance.add_vehicle(vehicle);
        }
        for (const auto &job : m_jobs) {
            problem_instance.add_job(job);
        }
        for (const auto &shipment : m_shipments) {
            problem_instance.add_shipment(shipment.first, shipment.second);
        }
        auto duration_matrix = m_matrix.get_vroom_duration_matrix();
        auto cost_matrix = m_matrix.get_vroom_cost_matrix();
        problem_instance.set_durations_matrix(::vroom::DEFAULT_PROFILE, std::move(duration_matrix));
        problem_instance.set_costs_matrix(::vroom::DEFAULT_PROFILE, std::move(cost_matrix));

        unsigned threads = 4;
        if (timeout < 0) {
            auto solution = problem_instance.solve(
                    static_cast<unsigned>(exploration_level), threads);
            results = get_results(solution);
        } else {
            auto timeout_ms = (loading_time <= timeout * 1000) ? (timeout * 1000 - loading_time) : 0;
            auto solution = problem_instance.solve(
                    static_cast<unsigned>(exploration_level), threads, timeout_ms);
            results = get_results(solution);
        }
    } catch (const ::vroom::Exception &ex) {
        throw;
    } catch (const std::exception &ex) {
        throw;
    } catch (...) {
        throw;
    }
    return results;
}

}  // namespace problem
}  // namespace vrprouting
