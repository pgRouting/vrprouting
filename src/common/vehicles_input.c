/*PGR-GNU*****************************************************************
File: vehicles_input.c

Copyright (c) 2016 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2016 Celia Virginia Vergara Castillo

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

#include "c_common/vehicles_input.h"

#include <limits.h>
#include <float.h>


#include "cpp_common/info.hpp"
#include "cpp_common/vehicle_t.hpp"

#include "cpp_common/get_check_data.hpp"

#ifdef PROFILE
#include "c_common/time_msg.h"
#include "c_common/debug_macro.h"
#endif

static
void check_pairs(Column_info_t lhs, Column_info_t rhs) {
    if (!(column_found(lhs.colNumber)) && column_found(rhs.colNumber)) {
        ereport(ERROR,
                (errmsg("Column \'%s\' not Found", lhs.name),
                 errhint("%s was found, also column is expected %s ",
                     rhs.name, lhs.name)));
    }
}

static
void fetch_euclidean(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vehicle_t *vehicle,
    bool with_stops) {
  bool with_id = false;
  /*
   * s_tw_open, s_tw_close must exist or non at all
   */
  check_pairs(info[4], info[5]);
  check_pairs(info[5], info[4]);
  /*
   * e_tw_open, e_tw_close must exist or non at all
   */
  check_pairs(info[6], info[7]);
  check_pairs(info[7], info[6]);

  /*
   * e_x, e_y must exist or non at all
   */
  check_pairs(info[13], info[14]);
  check_pairs(info[14], info[13]);

  vehicle->id = get_Id(tuple, tupdesc, info[0], -1);
  vehicle->capacity = get_PositiveAmount(tuple, tupdesc, info[1], 0);
  vehicle->cant_v =  get_PositiveAmount(tuple, tupdesc, info[2], 1);
  vehicle->speed  = column_found(info[3].colNumber) ?  spi_getFloat8(tuple, tupdesc, info[3]) : 1;

  /*
   * start values
   */
  vehicle->start_open_t = get_TTimestamp_plain(tuple, tupdesc, info[4], 0);
  vehicle->start_close_t = get_TTimestamp_plain(tuple, tupdesc, info[5], INT64_MAX);

  /*
   * end values
   */
  vehicle->end_open_t = get_TTimestamp_plain(tuple, tupdesc, info[6], vehicle->start_open_t);
  vehicle->end_close_t = get_TTimestamp_plain(tuple, tupdesc, info[7], vehicle->start_close_t);

  /*
   * service time values
   */
  vehicle->start_service_t = get_PositiveTInterval_plain(tuple, tupdesc, info[9], 0);
  vehicle->end_service_t = get_PositiveTInterval_plain(tuple, tupdesc, info[10], 0);

  /*
   * stops
   */
  vehicle->stops = NULL;
  vehicle->stops_size = 0;
  if (with_stops && column_found(info[8].colNumber)) {
    vehicle->stops = spi_getBigIntArr_allowEmpty(tuple, tupdesc, info[8], &vehicle->stops_size);
  }

  /*
   * Values for eucledian
   */
  vehicle->start_x = with_id ? 0 : spi_getCoordinate(tuple, tupdesc, info[11], 0);
  vehicle->start_y = with_id ? 0 : spi_getCoordinate(tuple, tupdesc, info[12], 0);
  vehicle->end_x =   with_id ? 0 : spi_getCoordinate(tuple, tupdesc, info[13], vehicle->start_x);
  vehicle->end_y =   with_id ? 0 : spi_getCoordinate(tuple, tupdesc, info[14], vehicle->start_y);
}

static
void fetch_raw(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vehicle_t *vehicle,
    bool with_stops) {
  /*
   * s_tw_open, s_tw_close must exist or non at all
   */
  check_pairs(info[6], info[7]);
  check_pairs(info[7], info[6]);
  /*
   * e_tw_open, e_tw_close must exist or non at all
   */
  check_pairs(info[10], info[11]);
  check_pairs(info[10], info[11]);

  vehicle->id = get_Id(tuple, tupdesc, info[0], -1);
  vehicle->capacity = get_PositiveAmount(tuple, tupdesc, info[1], 0);
  vehicle->cant_v =  get_PositiveAmount(tuple, tupdesc, info[2], 1);
  vehicle->speed  = column_found(info[3].colNumber) ?  spi_getFloat8(tuple, tupdesc, info[3]) : 1;
  vehicle->stops = NULL;
  vehicle->stops_size = 0;
  if (with_stops && column_found(info[4].colNumber)) {
    vehicle->stops = spi_getBigIntArr_allowEmpty(tuple, tupdesc, info[4], &vehicle->stops_size);
  }

  /*
   * start values
   */
  vehicle->start_node_id = get_Id(tuple, tupdesc, info[5], -1);
  vehicle->start_open_t = get_TTimestamp_plain(tuple, tupdesc, info[6], 0);
  vehicle->start_close_t = get_TTimestamp_plain(tuple, tupdesc, info[7], INT64_MAX);
  vehicle->start_service_t = get_PositiveTInterval_plain(tuple, tupdesc, info[8], 0);

  /*
   * end values
   */
  vehicle->end_node_id   = get_Id(tuple, tupdesc, info[9], vehicle->start_node_id);
  vehicle->end_open_t = get_TTimestamp_plain(tuple, tupdesc, info[10], vehicle->start_open_t);
  vehicle->end_close_t = get_TTimestamp_plain(tuple, tupdesc, info[11], vehicle->start_close_t);
  vehicle->end_service_t   = get_PositiveTInterval_plain(tuple, tupdesc, info[12], 0);

  /*
   * Ignored values
   */
  vehicle->start_x = 0;
  vehicle->start_y = 0;
  vehicle->end_x =   0;
  vehicle->end_y =   0;
}

static
void fetch_timestamps(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vehicle_t *vehicle,
    bool with_stops) {
  /*
   * s_tw_open, s_tw_close must exist or non at all
   */
  check_pairs(info[6], info[7]);
  check_pairs(info[7], info[6]);
  /*
   * e_tw_open, e_tw_close must exist or non at all
   */
  check_pairs(info[10], info[11]);
  check_pairs(info[10], info[11]);

  vehicle->id = get_Id(tuple, tupdesc, info[0], -1);
  vehicle->capacity = get_PositiveAmount(tuple, tupdesc, info[1], 0);
  vehicle->cant_v =  get_PositiveAmount(tuple, tupdesc, info[2], 1);
  vehicle->speed  = column_found(info[3].colNumber) ?  spi_getFloat8(tuple, tupdesc, info[3]) : 1;
  vehicle->stops = NULL;
  vehicle->stops_size = 0;
  if (with_stops && column_found(info[4].colNumber)) {
    vehicle->stops = spi_getBigIntArr_allowEmpty(tuple, tupdesc, info[4], &vehicle->stops_size);
  }

  /*
   * start values
   */
  vehicle->start_node_id   = get_Id(tuple, tupdesc, info[5], -1);
  vehicle->start_open_t    = get_TTimestamp(tuple, tupdesc, info[6], 0);
  vehicle->start_close_t   = get_TTimestamp(tuple, tupdesc, info[7], INT64_MAX);
  vehicle->start_service_t = get_PositiveTInterval(tuple, tupdesc, info[8], 0);

  /*
   * end values
   */
  vehicle->end_node_id   = get_Id(tuple, tupdesc, info[9], vehicle->start_node_id);
  vehicle->end_open_t    = get_TTimestamp(tuple, tupdesc, info[10], vehicle->start_open_t);
  vehicle->end_close_t   = get_TTimestamp(tuple, tupdesc, info[11], vehicle->start_close_t);
  vehicle->end_service_t = get_PositiveTInterval(tuple, tupdesc, info[12], 0);

  /*
   * Ignored values
   */
  vehicle->start_x = 0;
  vehicle->start_y = 0;
  vehicle->end_x =   0;
  vehicle->end_y =   0;
}


static
void db_get_vehicles(
    char *vehicles_sql,
    Vehicle_t **vehicles,
    size_t *total_vehicles,

    Column_info_t *info,
    const int column_count,

    int kind,
    bool with_stops) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", vehicles_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = pgr_SPI_prepare(vehicles_sql);
  Portal SPIportal;
  SPIportal = pgr_SPI_cursor_open(SPIplan);

  bool moredata = true;
  (*total_vehicles) = total_tuples = 0;

  /* on the first tuple get the column numbers */

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0) {
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*vehicles) == NULL)
        (*vehicles) = (Vehicle_t *)palloc0(
            total_tuples * sizeof(Vehicle_t));
      else
        (*vehicles) = (Vehicle_t *)repalloc(
            (*vehicles),
            total_tuples * sizeof(Vehicle_t));

      if ((*vehicles) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        switch (kind) {
          case 0 : fetch_timestamps(&tuple, &tupdesc, info,
                       &(*vehicles)[total_tuples - ntuples + t], with_stops);
                   break;
          case 1 : fetch_raw(&tuple, &tupdesc, info,
                       &(*vehicles)[total_tuples - ntuples + t], with_stops);
                   break;
          case 2 : fetch_euclidean(&tuple, &tupdesc, info,
                       &(*vehicles)[total_tuples - ntuples + t], with_stops);
                   break;
        }
      }
      SPI_freetuptable(tuptable);
    } else {
      moredata = false;
    }
  }

  SPI_cursor_close(SPIportal);

  if (total_tuples == 0) {
    (*total_vehicles) = 0;
    return;
  }

  (*total_vehicles) = total_tuples;
#ifdef PROFILE
  time_msg("reading vehicles", start_t, clock());
#endif
}

/**
 * @param[in] sql SQL query to execute
 * @param[in] with_stops do not ignore stops column
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vehicles(
    char *sql,
    Vehicle_t **rows,
    size_t *total_rows,
    bool with_stops) {
  const int column_count = 13;
  Column_info_t info[13];

  for (int i = 0; i < column_count; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = false;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "capacity";
  info[2].name = "number";
  info[3].name = "speed";
  info[4].name = "stops";
  info[5].name = "s_id";
  info[6].name = "s_tw_open";
  info[7].name = "s_tw_close";
  info[8].name = "s_t_service";
  info[9].name = "e_id";
  info[10].name = "e_tw_open";
  info[11].name = "e_tw_close";
  info[12].name = "e_t_service";

  info[6].eType = TIMESTAMP;
  info[7].eType = TIMESTAMP;
  info[10].eType = TIMESTAMP;
  info[11].eType = TIMESTAMP;
  info[8].eType = INTERVAL;
  info[12].eType = INTERVAL;

  info[4].eType = ANY_INTEGER_ARRAY;  // stops
  info[3].eType = ANY_NUMERICAL;      // speed

  info[0].strict = true;
  info[1].strict = true;
  info[5].strict = true;

  db_get_vehicles(sql, rows, total_rows, info, column_count, 0, with_stops);
}

/**
 * @param[in] sql SQL query to execute
 * @param[in] with_stops do not ignore stops column
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vehicles_raw(
    char *sql,
    Vehicle_t **rows,
    size_t *total_rows,
    bool with_stops) {
  const int column_count = 13;
  Column_info_t info[13];

  for (int i = 0; i < column_count; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = false;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "capacity";
  info[2].name = "number";
  info[3].name = "speed";
  info[4].name = "stops";
  info[5].name = "s_id";
  info[6].name = "s_open";
  info[7].name = "s_close";
  info[8].name = "s_service";
  info[9].name = "e_id";
  info[10].name = "e_open";
  info[11].name = "e_close";
  info[12].name = "e_service";


  info[4].eType = ANY_INTEGER_ARRAY;  // stops
  info[3].eType = ANY_NUMERICAL;      // speed

  info[0].strict = true;
  info[1].strict = true;
  info[5].strict = true;

  db_get_vehicles(sql, rows, total_rows, info, column_count, 1, with_stops);
}

/**
 * @param[in] sql SQL query to execute
 * @param[in] with_stops do not ignore stops column
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vehicles_euclidean(
    char *sql,
    Vehicle_t **rows,
    size_t *total_rows,
    bool with_stops) {
  const int column_count = 15;
  Column_info_t info[15];

  for (int i = 0; i < column_count; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = false;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "capacity";
  info[2].name = "number";
  info[3].name = "speed";
  info[4].name = "s_open";
  info[5].name = "s_close";
  info[6].name = "e_open";
  info[7].name = "e_close";
  info[8].name = "stops";
  info[9].name = "s_service";
  info[10].name = "e_service";
  info[11].name = "s_x";
  info[12].name = "s_y";
  info[13].name = "e_x";
  info[14].name = "e_y";


  info[8].eType = ANY_INTEGER_ARRAY;  // stops
  info[11].eType = ANY_NUMERICAL;  // s_x
  info[12].eType = ANY_NUMERICAL;  // s_y
  info[13].eType = ANY_NUMERICAL;  // e_x
  info[14].eType = ANY_NUMERICAL;  // e_y

  info[0].strict = true;
  info[1].strict = true;
  info[11].strict = true;
  info[12].strict = true;

  db_get_vehicles(sql, rows, total_rows, info, column_count, 2, with_stops);
}
