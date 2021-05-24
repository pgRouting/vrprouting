/*PGR-GNU*****************************************************************

FILE: tw_node.h

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

#ifndef INCLUDE_PROBLEM_NODE_TYPES_H_
#define INCLUDE_PROBLEM_NODE_TYPES_H_
#pragma once

namespace vrprouting {
namespace problem {

  enum NodeType {
    kStart = 0,    /**< starting site */
    kPickup,       /**< pickup site */
    kDelivery,     /**< delivery site */
    kDump,         /**< dump site, empties truck */
    kLoad,         /**< load site, fills the truck */
    kEnd           /**< ending site */
  };

}  //  namespace problem
}  //  namespace vrprouting

#endif  // INCLUDE_PROBLEM_NODE_TYPES_H_
