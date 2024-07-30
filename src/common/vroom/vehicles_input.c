/*PGR-GNU*****************************************************************
File: vehicles_input.c

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

#include "c_common/vroom/vehicles_input.h"

static
void fetch_vehicles(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_vehicle_t *vehicle,
    bool is_plain) {
  vehicle->id = get_Idx(tuple, tupdesc, info[0], 0);
  vehicle->start_id = get_MatrixIndex(tuple, tupdesc, info[1], -1);
  vehicle->end_id = get_MatrixIndex(tuple, tupdesc, info[2], -1);

  vehicle->capacity_size = 0;
  vehicle->capacity = column_found(info[3].colNumber) ?
    spi_getPositiveBigIntArr_allowEmpty(tuple, tupdesc, info[3], &vehicle->capacity_size)
    : NULL;

  vehicle->skills_size = 0;
  vehicle->skills = column_found(info[4].colNumber) ?
    spi_getPositiveIntArr_allowEmpty(tuple, tupdesc, info[4], &vehicle->skills_size)
    : NULL;

  if (is_plain) {
    vehicle->tw_open = get_Duration(tuple, tupdesc, info[5], 0);
    vehicle->tw_close = get_Duration(tuple, tupdesc, info[6], UINT_MAX);
  } else {
    vehicle->tw_open =
        (Duration)get_PositiveTTimestamp(tuple, tupdesc, info[5], 0);
    vehicle->tw_close =
        (Duration)get_PositiveTTimestamp(tuple, tupdesc, info[6], UINT_MAX);
  }

  if (vehicle->tw_open > vehicle->tw_close) {
    ereport(ERROR,
        (errmsg("Invalid time window (%d, %d)",
            vehicle->tw_open, vehicle->tw_close),
         errhint("Time window start time %d must be "
             "less than or equal to time window end time %d",
             vehicle->tw_open, vehicle->tw_close)));
  }

  vehicle->speed_factor = column_found(info[7].colNumber) ?
    spi_getFloat8(tuple, tupdesc, info[7])
    : 1.0;

  if (vehicle->speed_factor <= 0.0) {
    ereport(ERROR, (errmsg("Invalid speed_factor %lf", vehicle->speed_factor),
                    errhint("Speed factor must be greater than 0")));
  }

  vehicle->max_tasks = column_found(info[8].colNumber)
                           ? spi_getMaxTasks(tuple, tupdesc, info[8])
                           : INT_MAX;  // 2147483647

  vehicle->data = column_found(info[9].colNumber)
                      ? spi_getText(tuple, tupdesc, info[9])
                      : strdup("{}");
}


static
void db_get_vehicles(
    char *vehicles_sql,
    Vroom_vehicle_t **vehicles,
    size_t *total_vehicles,

    Column_info_t *info,
    const int column_count,
    bool is_plain) {
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
      /* Atleast one out of start_id or end_id must be present */
      info[1].colNumber = SPI_fnumber(SPI_tuptable->tupdesc, "start_id");
      info[2].colNumber = SPI_fnumber(SPI_tuptable->tupdesc, "end_id");
      if (!column_found(info[1].colNumber) && !column_found(info[2].colNumber)) {
        elog(ERROR, "At least one out of start_id or end_id must be present");
      }
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*vehicles) == NULL)
        (*vehicles) = (Vroom_vehicle_t *)palloc0(
            total_tuples * sizeof(Vroom_vehicle_t));
      else
        (*vehicles) = (Vroom_vehicle_t *)repalloc(
            (*vehicles),
            total_tuples * sizeof(Vroom_vehicle_t));

      if ((*vehicles) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        fetch_vehicles(&tuple, &tupdesc, info,
            &(*vehicles)[total_tuples - ntuples + t], is_plain);
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
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vroom_vehicles(
    char *sql,
    Vroom_vehicle_t **rows,
    size_t *total_rows,
    bool is_plain) {
  int kColumnCount = 10;
  Column_info_t info[kColumnCount];

  for (int i = 0; i < kColumnCount; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = false;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "start_id";
  info[2].name = "end_id";
  info[3].name = "capacity";
  info[4].name = "skills";
  info[5].name = "tw_open";
  info[6].name = "tw_close";
  info[7].name = "speed_factor";
  info[8].name = "max_tasks";
  info[9].name = "data";

  info[3].eType = ANY_INTEGER_ARRAY;  // capacity
  info[4].eType = INTEGER_ARRAY;      // skills
  info[5].eType = INTEGER;            // tw_open
  info[6].eType = INTEGER;            // tw_close
  info[7].eType = ANY_NUMERICAL;      // speed_factor
  info[8].eType = INTEGER;            // max_tasks
  info[9].eType = JSONB;              // data

  if (!is_plain) {
    info[5].eType = TIMESTAMP;        // tw_open
    info[6].eType = TIMESTAMP;        // tw_close
  }

  /**
   * id is mandatory.
   * At least one out of start_id or end_id must be present, but that is checked later.
   */
  info[0].strict = true;

  db_get_vehicles(sql, rows, total_rows, info, kColumnCount, is_plain);
}
