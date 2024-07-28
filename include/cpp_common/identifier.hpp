/*PGR-GNU*****************************************************************

FILE: identifier.hpp

Copyright (c) 2017 Celia Virginia Vergara Castillo
vicky_vergara@hotmail.com

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

/** @file
File from pgRouting
*/

#ifndef INCLUDE_CPP_COMMON_IDENTIFIER_HPP_
#define INCLUDE_CPP_COMMON_IDENTIFIER_HPP_
#pragma once

#if defined(__MINGW32__) || defined(_MSC_VER)
#include <stdint.h>
#endif

#include <ostream>

namespace vrprouting {

/** @brief Class that stores the information about identifiers

@dot
digraph structs {
    Object [shape=record,label="{Identifier | id: user's identifier | idx: internal index} "];
}
@enddot

 * Data comes with an identifier
 * - does not check for uniquenes of those identifiers
 *   - it is responsibility of the database mantainers to give unique identifiers

 * Example use:
 * ~~~ {.c}
 * struct data_t {
 *  int64_t d_id;  // identifier value
 *  // other structure members
 * }
 * vector<data_t> foo;
 * foo.push_back({1234, ... })
 * Identifier i(0, 1234); // original identifier 1234 is located on position 0 of foo
 *
 * ~~~
 */
class Identifier {
 public:
     Identifier() = default;
     Identifier(size_t _idx, int64_t _id);

     /** @name Inherited functions from Identifier
      * @{
      */
     /** @brief get the original id */
     int64_t id() const;
     /** @brief get the internal index */
     size_t idx() const;
     /** change the original id **/
     void reset_id(int64_t);
     /** @} */

     /** print the Identifier information */
     friend std::ostream& operator<<(std::ostream& log, const Identifier&);

 private:
     size_t  m_idx;
     int64_t m_id;
};


}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_IDENTIFIER_HPP_
