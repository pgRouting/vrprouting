/*PGR-GNU*****************************************************************
 *
FILE: check_get_data.hpp

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

#ifndef INCLUDE_CPP_COMMON_GET_CHECK_DATA_HPP_
#define INCLUDE_CPP_COMMON_GET_CHECK_DATA_HPP_
#pragma once


extern "C" {
#include <postgres.h>
#include <utils/array.h>
#include <access/htup_details.h>
#include <catalog/pg_type.h>
}

#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include "cpp_common/undefPostgresDefine.hpp"

#include "c_types/typedefs.h"
#include "cpp_common/info.hpp"


namespace vrprouting {

/** @brief  Function will check whether the colNumber represent any specific column or NULL (SPI_ERROR_NOATTRIBUTE).  */
bool column_found(const Info&);

TTimestamp get_timestamp_without_timezone(TTimestamp timestamp);

namespace detail {

std::vector<int64_t> get_any_positive_array(const HeapTuple, const TupleDesc&, const Info&);
std::vector<uint32_t> get_uint_array(const HeapTuple, const TupleDesc&, const Info&);
TInterval get_interval(const HeapTuple, const TupleDesc&, const Info&, TInterval);
TTimestamp get_timestamp(const HeapTuple, const TupleDesc&, const Info&, TTimestamp);
int64_t get_anyinteger(const HeapTuple, const TupleDesc&, const Info&, int64_t);

template <typename T>
T get_integral(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, T opt_value) {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return static_cast<T>(get_anyinteger(tuple, tupdesc, info, static_cast<int64_t>(opt_value)));
}

template <typename T>
T get_positive(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, T opt_value) {
    static_assert(std::is_integral<T>::value, "Integral required.");

    if (!column_found(info)) return opt_value;

    auto value = get_anyinteger(tuple, tupdesc, info, 0);
    if (value < 0) throw std::string("Unexpected negative value in column '") + info.name + "'";
    return static_cast<T>(value);
}

}  // namespace detail



/** @brief Function tells expected type of each column and then check the correspondence type of each column.  */
void fetch_column_info(const TupleDesc&, std::vector<Info>&);

/** @brief Function gets the C string of a JSONB */
std::string get_jsonb(const HeapTuple, const TupleDesc&, const Info&);

/** @brief Function gets the @b double of a Postgres floating point */
double get_anynumerical(const HeapTuple, const TupleDesc&, const Info&, double);

/** @brief Function get a char of a CHAR*/
char get_char(const HeapTuple, const TupleDesc&, const Info&, char);

/** @brief Function get an unordered_set of uint32_t*/
std::unordered_set<uint32_t> get_uint_unordered_set(const HeapTuple, const TupleDesc&, const Info&);

template <typename T>
T get_value(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, T opt_value) {
  switch (info.eType) {
    case ANY_INTEGER :
      return static_cast<T>(detail::get_integral<int64_t>(tuple, tupdesc,  info, static_cast<int64_t>(opt_value)));
      break;
    case INTEGER:
      return static_cast<T>(detail::get_integral<T>(tuple, tupdesc,  info, opt_value));
      break;
    case ANY_UINT :
    case TINTERVAL :
    case POSITIVE_INTEGER:
      return static_cast<T>(detail::get_positive<T>(tuple, tupdesc,  info, opt_value));
      break;
    case TIMESTAMP :
      return static_cast<T>(detail::get_timestamp(tuple, tupdesc,  info, static_cast<TTimestamp>(opt_value)));
      break;
    case INTERVAL :
      return static_cast<T>(detail::get_interval(tuple, tupdesc,  info, static_cast<TInterval>(opt_value)));
      break;
    default:
      throw std::string("Missing case value ") + info.name;
      break;
  }
}


template <typename T>
std::vector<T> get_array(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info) {
  switch (info.eType) {
    case ANY_POSITIVE_ARRAY:
      return detail::get_any_positive_array(tuple, tupdesc, info);
      break;
    default:
      throw std::string("Missing case value on array ") + info.name;
      break;
  }
}

template <typename T>
std::vector<T> get_uint_array(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info) {
  switch (info.eType) {
    case ANY_UINT_ARRAY:
      return detail::get_uint_array(tuple, tupdesc, info);
      break;
    default:
      throw std::string("Missing case value on Uint array ") + info.name;
      break;
  }
}


}  // namespace vrprouting


#endif  // INCLUDE_CPP_COMMON_GET_CHECK_DATA_HPP_
