/*PGR-GNU*****************************************************************
File: vrp_vroom_problem.hpp

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

#ifndef INCLUDE_CPP_COMMON_VRP_VROOM_PROBLEM_HPP_
#define INCLUDE_CPP_COMMON_VRP_VROOM_PROBLEM_HPP_
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <utility>

#include "c_types/vroom/vroom_job_t.h"
#include "c_types/vroom/vroom_rt.h"
#include "c_types/vroom/vroom_break_t.h"
#include "c_types/vroom/vroom_shipment_t.h"
#include "c_types/vroom/vroom_time_window_t.h"
#include "c_types/vroom/vroom_vehicle_t.h"
#include "c_types/matrix_cell_t.h"
#include "cpp_common/pgr_messages.h"
#include "cpp_common/base_matrix.h"

#include "structures/vroom/input/input.h"
#include "structures/vroom/job.h"
#include "structures/vroom/vehicle.h"

namespace vrprouting {

class Vrp_vroom_problem : public vrprouting::Pgr_messages {
 public:
  std::vector<vroom::Job> jobs() const { return m_jobs; }
  std::vector<std::pair<vroom::Job, vroom::Job>> shipments() const { return m_shipments; }
  std::vector<vroom::Vehicle> vehicles() const { return m_vehicles; }
  vrprouting::base::Base_Matrix matrix() const { return m_matrix; }

  /**
   * @name vroom time window wrapper
   */
  ///@{
  /**
   * @brief      Gets the vroom time window from the C-style struct
   *
   * @param[in]  time_window  The C-style time window struct
   *
   * @return     The vroom time window.
   */
  vroom::TimeWindow
  get_vroom_time_window(const Vroom_time_window_t &time_window) const {
    return
    vroom::TimeWindow(time_window.start_time,
                      time_window.end_time);
  }

  vroom::TimeWindow
  get_vroom_time_window(Duration start_time, Duration end_time) const {
    return vroom::TimeWindow(start_time, end_time);
  }

  std::vector<vroom::TimeWindow>
  get_vroom_time_windows(
      const std::vector<Vroom_time_window_t> &time_windows) const {
    std::vector < vroom::TimeWindow > tws;
    for (auto time_window : time_windows) {
      tws.push_back(get_vroom_time_window(time_window));
    }
    if (tws.size()) {
      return tws;
    } else {
      return std::vector<vroom::TimeWindow>(1, vroom::TimeWindow());
    }
  }
  ///@}


  /**
   * @name vroom amounts wrapper
   */
  ///@{
  /**
   * @brief      Gets the vroom amounts from C-style array
   *
   * @param[in]  amounts  The amounts array (pickup or delivery)
   *
   * @return     The vroom amounts.
   */
  vroom::Amount
  get_vroom_amounts(const std::vector <Amount> &amounts) const {
    vroom::Amount amt;
    if (amounts.size()) {
      for (auto amount : amounts) {
        amt.push_back(amount);
      }
    } else {
      const unsigned int amount_size =
          m_vehicles.size() ? static_cast<unsigned int>(m_vehicles[0].capacity.size()) : 0;
      // Default to zero amount with provided size.
      amt = vroom::Amount(amount_size);
      for (size_t i = 0; i < amounts.size(); i++) {
        amt[i] = amounts[i];
      }
    }
    return amt;
  }

  vroom::Amount
  get_vroom_amounts(const Amount *amounts, size_t count) const {
    return get_vroom_amounts(std::vector <Amount>(amounts, amounts + count));
  }
  ///@}


  /**
   * @name vroom skills wrapper
   */
  ///@{
  /**
   * @brief      Gets the vroom skills.
   *
   * @param[in]  skills  The skills array
   * @param[in]  count   The size of skills array
   *
   * @return     The vroom skills.
   */
  vroom::Skills
  get_vroom_skills(const Skill *skills, size_t count) const {
    return std::unordered_set <Skill>(skills, skills + count);
  }
  ///@}


  /**
   * @name vroom jobs wrapper
   */
  ///@{
  /**
   * @brief      Gets the vroom jobs.
   *
   * @param[in]  job      The job C-style struct
   * @param[in]  job_tws  The job time windows
   *
   * @return     The vroom job.
   */
  vroom::Job
  get_vroom_job(const Vroom_job_t &job,
                const std::vector<Vroom_time_window_t> &job_tws) const {
    vroom::Amount delivery =
        get_vroom_amounts(job.delivery, job.delivery_size);
    vroom::Amount pickup =
        get_vroom_amounts(job.pickup, job.pickup_size);
    vroom::Skills skills =
        get_vroom_skills(job.skills, job.skills_size);
    std::vector<vroom::TimeWindow> time_windows =
        get_vroom_time_windows(job_tws);
    vroom::Index location_index =
        static_cast<vroom::Index>(m_matrix.get_index(job.location_index));
    return vroom::Job(job.id, location_index, job.setup, job.service, delivery, pickup,
                      skills, job.priority, time_windows);
  }

  void problem_add_job(const Vroom_job_t &job,
                       const std::vector<Vroom_time_window_t> &job_tws) {
    m_jobs.push_back(get_vroom_job(job, job_tws));
  }

  void add_jobs(const std::vector<Vroom_job_t> &jobs,
                const std::vector<Vroom_time_window_t> &jobs_tws) {
    std::map<Idx, std::vector<Vroom_time_window_t>> job_tws_map;
    for (auto job_tw : jobs_tws) {
      Idx id = job_tw.id;
      if (job_tws_map.find(id) == job_tws_map.end()) {
        job_tws_map[id] = std::vector<Vroom_time_window_t>();
      }
      job_tws_map[id].push_back(job_tw);
    }
    for (auto job : jobs) {
      problem_add_job(job, job_tws_map[job.id]);
    }
  }

  void add_jobs(const Vroom_job_t *jobs, size_t count,
                const Vroom_time_window_t *jobs_tws, size_t total_jobs_tws) {
    add_jobs(
        std::vector<Vroom_job_t>(jobs, jobs + count),
        std::vector<Vroom_time_window_t>(jobs_tws, jobs_tws + total_jobs_tws));
  }
  ///@}


  /**
   * @name vroom shipments wrapper
   */
  ///@{
  /**
   * @brief      Gets the vroom shipments.
   *
   * @param[in]  shipment       The shipment C-style struct
   * @param[in]  pickup_tws     The pickup time windows
   * @param[in]  delivery_tws   The delivery time windows
   *
   * @return     The vroom shipment.
   */
  std::pair<vroom::Job, vroom::Job> get_vroom_shipment(
      const Vroom_shipment_t &shipment,
      const std::vector<Vroom_time_window_t> &pickup_tws,
      const std::vector<Vroom_time_window_t> &delivery_tws) const {
    vroom::Amount amount =
        get_vroom_amounts(shipment.amount, shipment.amount_size);
    vroom::Skills skills =
        get_vroom_skills(shipment.skills, shipment.skills_size);
    std::vector<vroom::TimeWindow> p_time_windows =
        get_vroom_time_windows(pickup_tws);
    std::vector<vroom::TimeWindow> d_time_windows =
        get_vroom_time_windows(delivery_tws);
    vroom::Index p_location_index = static_cast<vroom::Index>(
        m_matrix.get_index(shipment.p_location_index));
    vroom::Index d_location_index = static_cast<vroom::Index>(
        m_matrix.get_index(shipment.d_location_index));
    vroom::Job pickup = vroom::Job(
        shipment.id, vroom::JOB_TYPE::PICKUP, p_location_index,
        shipment.p_setup, shipment.p_service, amount,
        skills, shipment.priority, p_time_windows);
    vroom::Job delivery = vroom::Job(
        shipment.id, vroom::JOB_TYPE::DELIVERY, d_location_index,
        shipment.d_setup, shipment.d_service, amount,
        skills, shipment.priority, d_time_windows);
    return std::make_pair(pickup, delivery);
  }

  void problem_add_shipment(
      const Vroom_shipment_t &shipment,
      const std::vector<Vroom_time_window_t> &pickup_tws,
      const std::vector<Vroom_time_window_t> &delivery_tws) {
    m_shipments.push_back(
        get_vroom_shipment(shipment, pickup_tws, delivery_tws));
  }

  void add_shipments(const std::vector <Vroom_shipment_t> &shipments,
                     const std::vector <Vroom_time_window_t> &shipments_tws) {
    std::map<Idx, std::vector<Vroom_time_window_t>> pickup_tws_map;
    std::map<Idx, std::vector<Vroom_time_window_t>> delivery_tws_map;
    for (auto shipment_tw : shipments_tws) {
      Idx id = shipment_tw.id;
      if (shipment_tw.kind == 'p') {
        if (pickup_tws_map.find(id) == pickup_tws_map.end()) {
          pickup_tws_map[id] = std::vector<Vroom_time_window_t>();
        }
        pickup_tws_map[id].push_back(shipment_tw);
      } else if (shipment_tw.kind == 'd') {
        if (delivery_tws_map.find(id) == delivery_tws_map.end()) {
          delivery_tws_map[id] = std::vector<Vroom_time_window_t>();
        }
        delivery_tws_map[id].push_back(shipment_tw);
      }
    }
    for (auto shipment : shipments) {
      problem_add_shipment(shipment, pickup_tws_map[shipment.id],
                           delivery_tws_map[shipment.id]);
    }
  }

  void add_shipments(const Vroom_shipment_t *shipments, size_t count,
                     const Vroom_time_window_t *shipment_tws, size_t total_shipment_tws) {
    add_shipments(
        std::vector<Vroom_shipment_t>(shipments, shipments + count),
        std::vector<Vroom_time_window_t>(shipment_tws, shipment_tws + total_shipment_tws));
  }
  ///@}


  /**
   * @name vroom breaks wrapper
   */
  ///@{
  /**
   * @brief      Gets the vehicle breaks from C-style breaks struct
   *
   * @param[in]  v_break  The vehicle break struct
   *
   * @return     The vroom vehicle break.
   */
  vroom::Break
  get_vroom_break(
      const Vroom_break_t &v_break,
      const std::vector<Vroom_time_window_t> &break_tws) const {
    std::vector <vroom::TimeWindow> tws = get_vroom_time_windows(break_tws);
    return vroom::Break(v_break.id, tws, v_break.service);
  }

  std::vector < vroom::Break >
  get_vroom_breaks(
      const std::vector <Vroom_break_t> &breaks,
      const std::vector <Vroom_time_window_t> &breaks_tws) const {
    std::map<Idx, std::vector<Vroom_time_window_t>> breaks_tws_map;
    for (auto break_tw : breaks_tws) {
      Idx id = break_tw.id;
      if (breaks_tws_map.find(id) == breaks_tws_map.end()) {
        breaks_tws_map[id] = std::vector<Vroom_time_window_t>();
      }
      breaks_tws_map[id].push_back(break_tw);
    }
    std::vector < vroom::Break > v_breaks;
    for (auto v_break : breaks) {
      v_breaks.push_back(get_vroom_break(v_break, breaks_tws_map[v_break.id]));
    }
    return v_breaks;
  }
  ///@}


  /**
   * @name vroom vehicles wrapper
   */
  ///@{
  /**
   * @brief      Gets the vroom vehicles.
   *
   * @param[in]  vehicle        The vehicle C-style struct
   * @param[in]  breaks_tws     The breaks time windows
   *
   * @return     The vroom vehicle.
   */
  vroom::Vehicle get_vroom_vehicle(
      const Vroom_vehicle_t &vehicle,
      const std::vector<Vroom_break_t> &breaks,
      const std::vector<Vroom_time_window_t> &breaks_tws) const {
    vroom::Amount capacity =
        get_vroom_amounts(vehicle.capacity, vehicle.capacity_size);
    vroom::Skills skills =
        get_vroom_skills(vehicle.skills, vehicle.skills_size);
    vroom::TimeWindow time_window =
        get_vroom_time_window(vehicle.time_window_start,
                              vehicle.time_window_end);
    std::vector<vroom::Break> v_breaks = get_vroom_breaks(breaks, breaks_tws);

    std::optional<vroom::Location> start_index;
    std::optional<vroom::Location> end_index;
    // Set the value of start or end index only if they are present
    if (vehicle.start_index != -1) {
      start_index = static_cast<vroom::Index>(m_matrix.get_index(vehicle.start_index));
    }
    if (vehicle.end_index != -1) {
      end_index = static_cast<vroom::Index>(m_matrix.get_index(vehicle.end_index));
    }
    return vroom::Vehicle(vehicle.id, start_index, end_index,
                          vroom::DEFAULT_PROFILE, capacity, skills, time_window,
                          v_breaks, "", vehicle.speed_factor);
  }

  void problem_add_vehicle(
      const Vroom_vehicle_t &vehicle,
      const std::vector<Vroom_break_t> &breaks,
      const std::vector<Vroom_time_window_t> &breaks_tws) {
    m_vehicles.push_back(get_vroom_vehicle(vehicle, breaks, breaks_tws));
  }

  void add_vehicles(const std::vector<Vroom_vehicle_t> &vehicles,
                    const std::vector<Vroom_break_t> &breaks,
                    const std::vector<Vroom_time_window_t> &breaks_tws) {
    std::map<Idx, std::vector<Vroom_time_window_t>> breaks_tws_map;
    for (auto break_tw : breaks_tws) {
      Idx id = break_tw.id;
      if (breaks_tws_map.find(id) == breaks_tws_map.end()) {
        breaks_tws_map[id] = std::vector<Vroom_time_window_t>();
      }
      breaks_tws_map[id].push_back(break_tw);
    }

    std::map<Idx, std::vector<Vroom_break_t>> v_breaks_map;
    for (auto v_break : breaks) {
      Idx v_id = v_break.vehicle_id;
      if (v_breaks_map.find(v_id) == v_breaks_map.end()) {
        v_breaks_map[v_id] = std::vector<Vroom_break_t>();
      }
      v_breaks_map[v_id].push_back(v_break);
    }

    for (auto vehicle : vehicles) {
      std::vector<Vroom_break_t> v_breaks = v_breaks_map[vehicle.id];
      std::vector<Vroom_time_window_t> v_breaks_tws;
      for (auto v_break : v_breaks) {
        std::vector<Vroom_time_window_t> tws = breaks_tws_map[v_break.id];
        v_breaks_tws.insert(v_breaks_tws.end(), tws.begin(), tws.end());
      }
      problem_add_vehicle(vehicle, v_breaks, v_breaks_tws);
    }
  }

  void add_vehicles(const Vroom_vehicle_t *vehicles, size_t count,
                    const Vroom_break_t *breaks, size_t total_breaks,
                    const Vroom_time_window_t *breaks_tws, size_t total_breaks_tws) {
    add_vehicles(std::vector<Vroom_vehicle_t>(vehicles, vehicles + count),
                 std::vector<Vroom_break_t>(breaks, breaks + total_breaks),
                 std::vector<Vroom_time_window_t>(breaks_tws, breaks_tws + total_breaks_tws));
  }
  ///@}


  void add_matrix(vrprouting::base::Base_Matrix matrix) {
    m_matrix = matrix;
  }

  void get_amount(vroom::Amount vroom_amount, Amount **amount) {
    size_t amount_size = vroom_amount.size();
    for (size_t i = 0; i < amount_size; i++) {
      *((*amount) + i) = vroom_amount[i];
    }
  }

  std::vector < Vroom_rt > get_results(vroom::Solution solution) {
    std::vector < Vroom_rt > results;
    std::vector<vroom::Route> routes = solution.routes;
    Idx vehicle_seq = 1;
    for (auto route : routes) {
      Idx step_seq = 1;
      for (auto step : route.steps) {
        int32_t step_type = 0;
        Id task_id = static_cast<Id> (step.id);
        switch (step.step_type) {
          case vroom::STEP_TYPE::START:
            step_type = 1;
            task_id = -1;
            break;
          case vroom::STEP_TYPE::END:
            step_type = 6;
            task_id = -1;
            break;
          case vroom::STEP_TYPE::BREAK:
            step_type = 5;
            break;
          case vroom::STEP_TYPE::JOB:
            switch (step.job_type) {
              case vroom::JOB_TYPE::SINGLE:
                step_type = 2;
                break;
              case vroom::JOB_TYPE::PICKUP:
                step_type = 3;
                break;
              case vroom::JOB_TYPE::DELIVERY:
                step_type = 4;
                break;
            }
            break;
        }
        size_t load_size = step.load.size();
        Amount *load = reinterpret_cast<Amount*>(malloc(load_size * sizeof(Amount)));
        get_amount(step.load, &load);

        results.push_back({
          vehicle_seq,        // vehicles_seq
          route.vehicle,      // vehicles_id
          step_seq,           // step_seq
          step_type,          // step_type
          task_id,            // task_id
          step.arrival,       // arrival
          step.duration,      // duration
          step.service,       // service_time
          step.waiting_time,  // waiting_time
          load,               // load
          load_size           // load size
        });
        step_seq++;
      }
      vehicle_seq++;
    }
    return results;
  }

  std::vector < Vroom_rt > solve() {
    std::vector <Vroom_rt> results;
    try {
      const unsigned int amount_size =
          m_vehicles.size()
              ? static_cast<unsigned int>(m_vehicles[0].capacity.size())
              : 0;
      vroom::Input problem_instance(amount_size);

      for (const auto &vehicle : m_vehicles) {
        problem_instance.add_vehicle(vehicle);
      }
      for (const auto &job : m_jobs) {
        problem_instance.add_job(job);
      }
      for (const auto &shipment : m_shipments) {
        problem_instance.add_shipment(shipment.first, shipment.second);
      }
      vroom::Matrix<vroom::Duration> duration_matrix = m_matrix.get_vroom_duration_matrix();
      vroom::Matrix<vroom::Cost> cost_matrix = m_matrix.get_vroom_cost_matrix();
      problem_instance.set_durations_matrix(vroom::DEFAULT_PROFILE, std::move(duration_matrix));
      problem_instance.set_costs_matrix(vroom::DEFAULT_PROFILE, std::move(cost_matrix));

      auto solution = problem_instance.solve(5, 4);
      results = get_results(solution);
    } catch (const vroom::Exception &ex) {
      throw;
    } catch (const std::exception &ex) {
      throw;
    } catch (...) {
      throw;
    }
    return results;
  }

 private:
  std::vector<vroom::Job> m_jobs;
  std::vector<std::pair<vroom::Job, vroom::Job>> m_shipments;
  std::vector<vroom::Vehicle> m_vehicles;
  vrprouting::base::Base_Matrix m_matrix;
};

}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_VRP_VROOM_PROBLEM_HPP_
