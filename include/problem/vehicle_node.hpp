/*PGR-GNU*****************************************************************

FILE: vehicle_node.hpp

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

/** @file */

#ifndef INCLUDE_PROBLEM_VEHICLE_NODE_HPP_
#define INCLUDE_PROBLEM_VEHICLE_NODE_HPP_
#pragma once


#include <string>

#include "problem/tw_node.hpp"

typedef struct Solution_rt Solution_rt;

namespace vrprouting {
namespace problem {


/** @class Vehicle_node;
 * @brief Extend Tw_node to evaluate the vehicle at node level
 *
 * This class extends Twnode by adding attributes to store information
 * and to set and get these attribute values.
 */
class Vehicle_node: public Tw_node {
 public:
     /** @brief Print the contents of a Vehicle_node object. */
     friend std::ostream& operator<<(std::ostream &log, const Vehicle_node &v);

     /** @brief Construction without information is not allowed */
     Vehicle_node() = delete;

     /** @brief Construction of a Vehicle node based on a time windows node */
     explicit Vehicle_node(const Tw_node &node);

     /** @brief Vehicle's travel_time from previous node to this node. */
     inline TInterval travel_time() const {return m_travel_time;}

     /** @brief Vehicle's arrival_time to this node. */
     inline TTimestamp arrival_time() const {return m_arrival_time;}

     /** @brief Vehicle's wait_time at this node. */
     inline TInterval wait_time() const {return m_wait_time;}

     /** @brief Vehicle's departure_time from this node. */
     inline TTimestamp departure_time() const {return m_departure_time;}

     /** @name Accumulated evaluation */
     /** @{ */

     /** @brief Vehicle's total times it has violated time windows. */
     inline int twvTot() const {return m_twvTot;}

     /** @brief Vehicle's total times it has violated cargo limits. */
     inline int cvTot() const {return m_cvTot;}

     /** @brief Vehicle's total cargo after the node was served. */
     inline Amount cargo() const {return m_cargo;}

     /** @brief _time spent moving between nodes by the truck */
     inline TInterval total_travel_time() const {return m_tot_travel_time;}

     /** @brief _time spent by the truck waiting for nodes to open */
     inline TInterval total_wait_time() const {return m_tot_wait_time;}

     /** @brief _time spent by the truck servicing the nodes */
     inline TInterval total_service_time() const {return m_tot_service_time;}
     /** @} */

     /** @brief the basic objective function for this node */
     double objective() const;

     /** @brief evaluate this node */
     void evaluate(PAmount cargoLimit);

     void evaluate(const Vehicle_node &pred, PAmount, Speed = 1.0);

     Solution_rt get_postgres_result(int vid, int64_t v_id, int stop_seq) const;

 protected:
     /** @name evaluation */
     /** @{ */
     /** @brief does the node violates the time windows constraints? */
     bool has_twv() const;

     /** @brief does the node violates the capacity constraints? */
     bool has_cv(PAmount cargoLimit) const;

     inline TInterval total_time() const {return m_departure_time - m_arrival_time;}

     /** @brief delta_time = departure_time(this) - departure_time(previous) */
     inline TInterval delta_time() const {return m_delta_time;}

     /** @brief True when \b arrival_time + \b delta_time generates TWV.*/
     bool deltaGeneratesTWV(TInterval delta_time) const;

     /** @name State */
     /** @{ */
     /** \brief True when the total count for violations are 0 */
     bool feasible() const {return m_twvTot == 0 &&  m_cvTot == 0;}

     /** \brief True doesn't have twc nor cv (including total counts) */
     bool feasible(PAmount cargoLimit) const {
         return feasible() &&  !has_twv() &&  !has_cv(cargoLimit);
     }
     /** @} */


 private:
     /** @name Node evaluation */
     /** @{ */

     /** Travel time from last node */
     TInterval m_travel_time;

     /** Arrival time at this node */
     TTimestamp m_arrival_time;

     /** Wait time at this node
      * - 0 when arrived after the node opens
      * - >0 when arrived before the node opens
      */
     TInterval m_wait_time;

     /** Departure time from this node */
     TTimestamp m_departure_time;

     /** Departure time - last nodes departure time */
     TInterval m_delta_time;
     /** @} */


     /** @name Accumulated evaluation */
     /** @{ */

     /** Accumulated cargo */
     Amount m_cargo;

     /** Total count of TWV */
     int    m_twvTot;

     /** Total count of CV */
     int    m_cvTot;

     /** Accumulated wait time */
     TInterval m_tot_wait_time;

     /** Accumulated travel time */
     TInterval m_tot_travel_time;

     /** Accumulated service time */
     TInterval m_tot_service_time;
     /** @} */
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_VEHICLE_NODE_HPP_
