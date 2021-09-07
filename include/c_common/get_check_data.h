/*PGR-GNU*****************************************************************
File: get_check_data.h

Copyright (c) 2015 Celia Virginia Vergara Castillo
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

#ifndef INCLUDE_C_COMMON_GET_CHECK_DATA_H_
#define INCLUDE_C_COMMON_GET_CHECK_DATA_H_
#pragma once

#include "c_common/postgres_connection.h"
#include "c_types/column_info_t.h"
#include "c_types/typedefs.h"

/** @brief  Check whether the colNumber represent any specific column or NULL (SPI_ERROR_NOATTRIBUTE). */
bool column_found(int colNumber);

/** @brief Function tells expected type of each column and then check the correspondence type of each column.  */
void pgr_fetch_column_info(
    Column_info_t info[],
    int info_size);

/*! @brief get value of specified column in char type. */
char
spi_getChar(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    char default_value);

/** @brief Function returns the values of specified columns in array.  */
int64_t*
spi_getBigIntArr(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size);

/** @brief Function returns the values of specified columns in array.  */
int64_t*
spi_getBigIntArr_allowEmpty(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size);

/** @brief Function returns the values of specified columns in array.  */
int64_t*
spi_getPositiveBigIntArr_allowEmpty(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size);

/** @brief Function returns the values of specified columns in array. */
uint32_t*
spi_getPositiveIntArr_allowEmpty(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size);

/** @brief gets value of specified column in double type. */
double
spi_getFloat8(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info);

/** @brief gets string representation of the value of specified column. */
char*
spi_getText(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info);


/** @name timestamp related
 * @{ */
/** @brief  Converts timestamp to timestamp without timezone */
TTimestamp timestamp_without_timezone(TTimestamp timestamp);

/** @brief gets a timestamp value from postgres type TIMESTAMP */
TTimestamp get_TTimestamp(HeapTuple*, TupleDesc*, Column_info_t, TTimestamp);

/** @brief gets a timestamp value from postgres type TIMESTAMP >= 1970-01-01 00:00:00*/
TTimestamp get_PositiveTTimestamp(HeapTuple*, TupleDesc*, Column_info_t, TTimestamp);

/** @brief gets a timestamp value from ANY-INTEGER */
TTimestamp get_TTimestamp_plain(HeapTuple*, TupleDesc*, Column_info_t, TTimestamp);

/** @brief gets a timestamp value from ANY-INTEGER > 0 */
TTimestamp get_PositiveTTimestamp_plain(HeapTuple*, TupleDesc*, Column_info_t, TTimestamp);
/* @} */

/** @name interval related
 * @{ */
/** @brief gets an interval value from postgres type INTERVAL */
TInterval get_TInterval(HeapTuple*, TupleDesc*, Column_info_t, TInterval);

/** @brief gets an interval value from postgres type INTERVAL > 0 */
TInterval get_PositiveTInterval(HeapTuple*, TupleDesc*, Column_info_t, TInterval);

/** @brief gets an interval value from ANY-INTEGER */
TInterval get_TInterval_plain(HeapTuple*, TupleDesc*, Column_info_t, TInterval);

/** @brief gets an interval value from ANY-INTEGER > 0 */
TInterval get_PositiveTInterval_plain(HeapTuple*, TupleDesc*, Column_info_t, TInterval);
/* @} */

/** get Id from data */
Id get_Id(HeapTuple*, TupleDesc*, Column_info_t, Id);

/** get Idx from data */
Idx get_Idx(HeapTuple*, TupleDesc*, Column_info_t, Idx);

/** get StepType from data */
StepType get_StepType(HeapTuple *, TupleDesc *, Column_info_t, StepType);

/** get MatrixIndex from data */
MatrixIndex get_MatrixIndex(HeapTuple*, TupleDesc*, Column_info_t, MatrixIndex);

/** get Duration from data */
Duration get_Duration(HeapTuple*, TupleDesc*, Column_info_t, Duration);

/** get Kind from data */
char get_Kind(HeapTuple*, TupleDesc*, Column_info_t, char);

/** get Priority from data */
Priority get_Priority(HeapTuple*, TupleDesc*, Column_info_t, Priority);

/** get Distance from data */
Distance get_Distance(HeapTuple*, TupleDesc*, Column_info_t, Distance);

/** get Amount from data */
Amount get_Amount(HeapTuple*, TupleDesc*, Column_info_t, Amount);

/** get positive Amount from data */
PAmount get_PositiveAmount(HeapTuple*, TupleDesc*, Column_info_t, PAmount);

/** get a coordinate value */
Coordinate spi_getCoordinate(HeapTuple*, TupleDesc*, Column_info_t, Coordinate);


#endif  // INCLUDE_C_COMMON_GET_CHECK_DATA_H_
