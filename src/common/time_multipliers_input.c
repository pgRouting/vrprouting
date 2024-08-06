/*PGR-GNU*****************************************************************
File: time_multipliers_input.c

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2021 Copyright (c) 2021 Joseph Emile Honour Percival

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

#include "c_common/time_multipliers_input.h"

#include "cpp_common/info.hpp"
#include "cpp_common/time_multipliers_t.hpp"
#include "cpp_common/get_check_data.hpp"

#ifdef PROFILE
#include "c_common/time_msg.h"
#include "c_common/debug_macro.h"
#endif


static
void fetch_raw(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info[2],
    Time_multipliers_t *row) {
  row->start_time = get_TTimestamp_plain(tuple, tupdesc, info[0], 0);
  row->multiplier = spi_getFloat8(tuple, tupdesc,  info[1]);
}

static
void fetch_timestamps(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info[2],
    Time_multipliers_t *row) {
  row->start_time = get_TTimestamp(tuple, tupdesc, info[0], 0);
  row->multiplier = spi_getFloat8(tuple, tupdesc,  info[1]);
}

/**
 * param [in] sql multipliers SQL
 * param [in,out] rows catptured information
 * param [in,out] total_rows total information captured
 */
static
void get_timeMultipliersGeneral(
    char *sql,
    Column_info_t *info,
    const int kind,
    Time_multipliers_t **rows,
    size_t *total_rows) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", sql);
#endif
  const int tuple_limit = 1000000;
  size_t total_tuples = 0;
  const int column_count = 2;

  void *SPIplan;
  SPIplan = vrp_SPI_prepare(sql);

  Portal SPIportal;
  SPIportal = vrp_SPI_cursor_open(SPIplan);


  bool moredata = true;
  (*total_rows) = total_tuples;

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0)
      pgr_fetch_column_info(info, column_count);

    size_t ntuples = SPI_processed;
    total_tuples += ntuples;

    if (ntuples > 0) {
      if ((*rows) == NULL)
        (*rows) = (Time_multipliers_t *)palloc0(
            total_tuples * sizeof(Time_multipliers_t));
      else
        (*rows) = (Time_multipliers_t *)repalloc(
            (*rows), total_tuples * sizeof(Time_multipliers_t));

      if ((*rows) == NULL) {
        elog(ERROR, "Out of memory");
      }

      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;

      for (size_t t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        switch (kind) {
          case 0 : fetch_timestamps(&tuple, &tupdesc, info,
                       &(*rows)[total_tuples - ntuples + t]);
                   break;
          case 1 : fetch_raw(&tuple, &tupdesc, info,
                       &(*rows)[total_tuples - ntuples + t]);
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
    (*total_rows) = 0;
    return;
  }

  (*total_rows) = total_tuples;
#ifdef PROFILE
  time_msg("reading time dependant multipliers", start_t, clock());
#endif
}

/**
  @param [in] sql query that has the following columns: start_time, multiplier
  @param [out] rows C Container that holds all the multipliers rows
  @param [out] total_rows Total rows recieved
  */
void get_timeMultipliers(
    char *sql,
    Time_multipliers_t **rows,
    size_t *total_rows) {
  Column_info_t info[2];

  int i;
  for (i = 0; i < 2; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = ANY_NUMERICAL;
  }
  info[0].name = "start_time";
  info[1].name = "multiplier";

  info[0].eType = TIMESTAMP;

  get_timeMultipliersGeneral(sql, info, 0, rows, total_rows);
}

/**
  @param [in] sql query that has the following columns: start_time, multiplier
  @param [out] rows C Container that holds all the multipliers
  @param [out] total_rows Total rows recieved
  */
void get_timeMultipliers_raw(
    char *sql,
    Time_multipliers_t **rows,
    size_t *total_rows) {
  Column_info_t info[2];

  int i;
  for (i = 0; i < 2; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = ANY_NUMERICAL;
  }
  info[0].name = "start_value";
  info[1].name = "multiplier";

  info[0].eType = ANY_INTEGER;

  get_timeMultipliersGeneral(sql, info, 1, rows, total_rows);
}
