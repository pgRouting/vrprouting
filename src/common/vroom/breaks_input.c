/*PGR-GNU*****************************************************************
File: breaks_input.c

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

#include "c_common/vroom/breaks_input.h"

#include "cpp_common/vroom_break_t.hpp"
#include "cpp_common/info.hpp"

#include "c_common/debug_macro.h"
#include "cpp_common/get_check_data.hpp"

static
void fetch_breaks(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_break_t *vroom_break,
    bool is_plain) {
  vroom_break->id = get_Idx(tuple, tupdesc, info[0], 0);
  vroom_break->vehicle_id = get_Idx(tuple, tupdesc, info[1], 0);
  if (is_plain) {
    vroom_break->service = get_Duration(tuple, tupdesc, info[2], 0);
  } else {
    vroom_break->service =
        (Duration)get_PositiveTInterval(tuple, tupdesc, info[2], 0);
  }
  vroom_break->data = column_found(info[3].colNumber)
                          ? spi_getText(tuple, tupdesc, info[3])
                          : strdup("{}");
}


static
void db_get_breaks(
    char *breaks_sql,
    Vroom_break_t **breaks,
    size_t *total_breaks,

    Column_info_t *info,
    const int column_count,
    bool is_plain) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", breaks_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = vrp_SPI_prepare(breaks_sql);
  Portal SPIportal;
  SPIportal = vrp_SPI_cursor_open(SPIplan);

  bool moredata = true;
  (*total_breaks) = total_tuples = 0;

  /* on the first tuple get the column numbers */

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0) {
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*breaks) == NULL)
        (*breaks) = (Vroom_break_t *)palloc0(
            total_tuples * sizeof(Vroom_break_t));
      else
        (*breaks) = (Vroom_break_t *)repalloc(
            (*breaks),
            total_tuples * sizeof(Vroom_break_t));

      if ((*breaks) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        fetch_breaks(&tuple, &tupdesc, info,
            &(*breaks)[total_tuples - ntuples + t], is_plain);
      }
      SPI_freetuptable(tuptable);
    } else {
      moredata = false;
    }
  }

  SPI_cursor_close(SPIportal);

  if (total_tuples == 0) {
    (*total_breaks) = 0;
    return;
  }

  (*total_breaks) = total_tuples;
#ifdef PROFILE
  time_msg("reading breaks", start_t, clock());
#endif
}


void
get_vroom_breaks(
    char *sql,
    Vroom_break_t **rows,
    size_t *total_rows,
    bool is_plain) {
  int kColumnCount = 4;
  Column_info_t info[kColumnCount];

  for (int i = 0; i < kColumnCount; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "vehicle_id";
  info[2].name = "service";
  info[3].name = "data";

  info[2].eType = INTEGER;  // service
  info[3].eType = JSONB;    // data

  if (!is_plain) {
    info[2].eType = INTERVAL;  // service
  }

  /* service and data are not mandatory */
  info[2].strict = false;
  info[3].strict = false;

  db_get_breaks(sql, rows, total_rows, info, kColumnCount, is_plain);
}
