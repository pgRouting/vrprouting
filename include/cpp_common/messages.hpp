/*PGR-GNU*****************************************************************

FILE: messages.hpp

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

/** @file
File from pgRouting
*/

#ifndef INCLUDE_CPP_COMMON_MESSAGES_HPP_
#define INCLUDE_CPP_COMMON_MESSAGES_HPP_
#pragma once



#include <string>
#include <sstream>

namespace vrprouting {


/** @brief Messages Handling
 *
 * A common structure to be used on the C++ code that stores
 * messages intended for PostgreSQL
 *
 * Messages currently handled:
 * log
 * notice
 * error
 */
class Messages {
 public:
    Messages() = default;
    Messages(const Messages&) {}
    Messages& operator=(const Messages&) {return *this;}

    /*! @brief gets the contents of @b log message
     *
     * @returns the current contents of @b log
     *
     */
    std::string get_log() const;

    /*! @brief gets the contents of @b notice message
     *
     * @returns the current contents of @b notice
     *
     */
    std::string get_notice() const;

    /*! @brief gets the contents of notice message
     *
     * @returns true when @b error message is not empty
     */
    bool has_error() const;

    /*! @brief gets the contents of @b error message
     *
     * @returns the current contents of @b error
     *
     */
    std::string get_error() const;

    /*! @brief Clears @b all the messages */
    void clear();


 public:
    /*! Stores the hint information*/
    mutable std::ostringstream log;
    /*! Stores the notice information*/
    mutable std::ostringstream notice;
    /*! Stores the error information*/
    mutable std::ostringstream error;
};



#if defined(__MINGW32__) || defined(_MSC_VER)
#define ENTERING(x)
#define EXITING(x)
#else
#define ENTERING(x) x.log << "\n--> " << __PRETTY_FUNCTION__ << "\n"
#define EXITING(x) x.log << "\n<-- " << __PRETTY_FUNCTION__ << "\n"
#endif



}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_MESSAGES_HPP_
