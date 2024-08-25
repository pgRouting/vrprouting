/*PGR-GNU*****************************************************************

File: get_data.hpp

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

#ifndef INCLUDE_CPP_COMMON_GET_DATA_HPP_
#define INCLUDE_CPP_COMMON_GET_DATA_HPP_
#pragma once

#include <vector>
#include <string>

#include "c_common/postgres_connection.h"
#include "cpp_common/info.hpp"
#include "cpp_common/check_get_data.hpp"
#include "cpp_common/alloc.hpp"

namespace vrprouting {
namespace pgget {

/** @brief Retrives the tuples
 * @tparam Data_type Scructure of data
 * @tparam Func fetcher function
 * @param[in] sql  Query to be processed
 * @param[in] flag useful flag depending on data
 * @param[in] info information about the data
 * @param[in] func fetcher function to be used
 */
template <typename Data_type, typename Func>
std::vector<Data_type>
get_data(const std::string& sql, bool flag, std::vector<Info> info, Func func) {
    const int tuple_limit = 1000000;

    size_t total_tuples = 0;

    auto SPIplan = vrp_SPI_prepare(sql.c_str());
    auto SPIportal = vrp_SPI_cursor_open(SPIplan);

    bool moredata = true;
    std::vector<Data_type> tuples;

    while (moredata == true) {
        SPI_cursor_fetch(SPIportal, true, tuple_limit);
        auto tuptable = SPI_tuptable;
        auto tupdesc = SPI_tuptable->tupdesc;
        if (total_tuples == 0) fetch_column_info(tupdesc, info);

        size_t ntuples = SPI_processed;
        total_tuples += ntuples;

        if (ntuples > 0) {
            tuples.reserve(total_tuples);
            for (size_t t = 0; t < ntuples; t++) {
                tuples.push_back(func(tuptable->vals[t], tupdesc, info, flag));
            }
            SPI_freetuptable(tuptable);
        } else {
            moredata = false;
        }
    }

    SPI_cursor_close(SPIportal);
    return tuples;
}

}  // namespace pgget
}  // namespace vrprouting

#endif  // INCLUDE_CPP_COMMON_GET_DATA_HPP_
