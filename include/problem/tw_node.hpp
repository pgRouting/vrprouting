/*PGR-GNU*****************************************************************

FILE: tw_node.hpp

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

#ifndef INCLUDE_PROBLEM_TW_NODE_HPP_
#define INCLUDE_PROBLEM_TW_NODE_HPP_
#pragma once

#include <string>
#include "c_types/typedefs.h"
#include "cpp_common/identifier.hpp"
#include "problem/node_types.hpp"


namespace vrprouting {
namespace problem {

class Matrix;

/** @class Tw_node
 * @brief Time window attributes of a node.

 * A Time Window node is a Node with addition attributes and methods to
 * to support Time Windows and to model a more complex Node need in many
 * vehicle routing problems.
 *
 * Most application specific code will extend this class and define the specific
 * values and requirements for @c type and @c streetid.
 *
 */
class Tw_node : public Identifier {
 public:
     /** @brief Creating a Tw_node is not permitted */
     Tw_node() = delete;

     /** @brief Creating a Tw_node from a postgreSQL order */
     Tw_node(
             size_t id,
             const Orders_t &data,
             const NodeType &type);

     /** @brief Creating a Tw_node from a postgreSQL vehicle */
     Tw_node(
             size_t id,
             const Vehicle_t &data,
             const NodeType &type);

     /** @brief Returns the order to which it belongs.*/
     inline int64_t order() const {return m_order;}

     /** @brief Returns the opening time.*/
     inline TTimestamp opens() const {return m_opens;}

     /** @brief Returns the closing time. */
     inline TTimestamp closes() const {return m_closes;}

     /** @brief Returns the service time for this node. */
     inline TInterval service_time() const {return m_service_time;}

     /** @brief Returns the type of this node. */
     inline auto type() const {return m_type;}

     /** @brief Returns the length of time between the opening and closing. */
     inline TInterval window_length() const {return m_closes - m_opens;}

     /** @brief Is the node a valid vehicle's starting node */
     bool is_start() const;

     /** @brief Is the node a valid order's pickup node */
     bool is_pickup() const;

     /** @brief Is the node a valid order's delivery node */
     bool is_delivery() const;

     /** @brief Is the node a valid vehicle's ending node */
     bool is_end() const;

     /** @brief Is the node a valid vehicle's dumping node */
     bool is_dump() const;

     /** @brief Is the node a valid vehicle's loading node */
     bool is_load() const;

     /** @brief returns a string code of the kind of node */
     std::string type_str() const;

     /** @brief Print the contents of a Twnode object. */
     friend std::ostream& operator<<(std::ostream &, const Tw_node &);

     /** @brief equality operator */
     bool operator ==(const Tw_node &rhs) const;


     /** @brief True when @b arrivalTime is after it @b closes */
     inline bool is_late_arrival(TTimestamp arrival_time) const {
         return arrival_time > m_closes;
     }

     /** @brief True when @b arrivalTime is before it @b opens */
     inline bool is_early_arrival(TTimestamp arrival_time) const {
         return arrival_time < m_opens;
     }

     /** @brief True when @b arrivalTime in the time window */
     inline bool is_on_time(TTimestamp arrival_time) const {
         return !is_early_arrival(arrival_time)
             && !is_late_arrival(arrival_time);
     }

     /** @brief travel time to other node. */
     TInterval travel_time_to(const Tw_node&, TTimestamp, Speed = 1.0) const;

     static const Matrix* m_time_matrix_ptr;

     /** is possible to arrive to @b this after visiting @b other? */
     bool is_compatible_IJ(const Tw_node &I, Speed = 1.0) const;

     /** @brief arrival time at @b This node, when arrived at I at opening time TODO refine description*/
     TTimestamp arrival_j_opens_i(const Tw_node &I, Speed = 1.0) const;

     /** @brief arrival time at @b This node, when arrived at I at closing time TODO refine description*/
     TTimestamp arrival_j_closes_i(const Tw_node &I, Speed = 1.0) const;

     /** @brief can arrive to @b this after visiting as late as possible @b I? TODO refine description*/
     bool is_partially_compatible_IJ(const Tw_node &I, Speed = 1.0) const;

     /** @brief can arrive to @b this after visiting as late as possible @b I? TODO refine description*/
     bool is_tight_compatible_IJ(const Tw_node &I, Speed = 1.0) const;

     /** @brief can arrive to @b this after visiting as late as possible @b I? TODO refine description*/
     bool is_partially_waitTime_compatible_IJ(const Tw_node &I, Speed = 1.0) const;

     /** @brief can arrive to @b this after visiting as late as possible @b I? TODO refine description*/
     bool is_waitTime_compatible_IJ(const Tw_node &I, Speed = 1.0) const;

     /** @brief Returns the demand associated with this node. */
     inline Amount demand() const {return m_demand;}

     /** @brief is the node valid? */
     bool is_valid() const;

 protected:
     /** @brief Sets the demand value to a new value */
     inline void demand(Amount value) {m_demand = value;}

 private:
     /** order to which it belongs (idx) */
     int64_t m_order { };

     /** opening time of the node */
     TTimestamp m_opens;

     /** closing time of the node */
     TTimestamp m_closes;

     /** time it takes to be served */
     TInterval m_service_time;

     /** The demand for the Node */
     Amount m_demand;

     /** The kind of Node */
     NodeType m_type;
};

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_TW_NODE_HPP_
