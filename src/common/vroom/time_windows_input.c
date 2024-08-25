/*PGR-GNU*****************************************************************
File: time_windows_input.c

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

#include "c_common/vroom/time_windows_input.h"

#include "cpp_common/vroom_time_window_t.hpp"
#include "cpp_common/info.hpp"

#include "c_common/debug_macro.h"
#include "cpp_common/get_check_data.hpp"

static
void fetch_time_windows(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_time_window_t *time_window,
    bool is_shipment,
    bool is_plain) {

  time_window->id = get_Idx(tuple, tupdesc, info[0], 0);

  if (is_shipment) {
    char kind = get_Kind(tuple, tupdesc, info[1], ' ');
    if (kind != 'p' && kind != 'd') {
      ereport(ERROR, (errmsg("Invalid kind %c", kind),
                      errhint("Kind must be either 'p' or 'd'")));
    }
    time_window->kind = kind;
    if (is_plain) {
      time_window->tw_open = get_Duration(tuple, tupdesc, info[2], 0);
      time_window->tw_close = get_Duration(tuple, tupdesc, info[3], 0);
    } else {
      time_window->tw_open =
          (Duration)get_PositiveTTimestamp(tuple, tupdesc, info[2], 0);
      time_window->tw_close =
          (Duration)get_PositiveTTimestamp(tuple, tupdesc, info[3], 0);
    }
  } else {
    if (is_plain) {
      time_window->tw_open = get_Duration(tuple, tupdesc, info[1], 0);
      time_window->tw_close = get_Duration(tuple, tupdesc, info[2], 0);
    } else {
      time_window->tw_open =
          (Duration)get_PositiveTTimestamp(tuple, tupdesc, info[1], 0);
      time_window->tw_close =
          (Duration)get_PositiveTTimestamp(tuple, tupdesc, info[2], 0);
    }
  }

  if (time_window->tw_open > time_window->tw_close) {
    ereport(ERROR,
        (errmsg("Invalid time window (%d, %d)",
            time_window->tw_open, time_window->tw_close),
         errhint("Time window start time %d must be "
             "less than or equal to time window end time %d",
             time_window->tw_open, time_window->tw_close)));
  }
}


static
void db_get_time_windows(
    char *time_windows_sql,
    Vroom_time_window_t **time_windows,
    size_t *total_time_windows,

    Column_info_t *info,
    const int column_count,
    bool is_shipment,
    bool is_plain) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", time_windows_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = vrp_SPI_prepare(time_windows_sql);
  Portal SPIportal;
  SPIportal = vrp_SPI_cursor_open(SPIplan);

  bool moredata = true;
  (*total_time_windows) = total_tuples = 0;

  /* on the first tuple get the column numbers */

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0) {
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*time_windows) == NULL)
        (*time_windows) = (Vroom_time_window_t *)palloc0(
            total_tuples * sizeof(Vroom_time_window_t));
      else
        (*time_windows) = (Vroom_time_window_t *)repalloc(
            (*time_windows),
            total_tuples * sizeof(Vroom_time_window_t));

      if ((*time_windows) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        fetch_time_windows(&tuple, &tupdesc, info,
            &(*time_windows)[total_tuples - ntuples + t],
            is_shipment, is_plain);
      }
      SPI_freetuptable(tuptable);
    } else {
      moredata = false;
    }
  }

  SPI_cursor_close(SPIportal);

  if (total_tuples == 0) {
    (*total_time_windows) = 0;
    return;
  }

  (*total_time_windows) = total_tuples;
#ifdef PROFILE
  time_msg("reading time windows", start_t, clock());
#endif
}


void
get_vroom_time_windows(
    char *sql,
    Vroom_time_window_t **rows,
    size_t *total_rows,
    bool is_plain) {
  int kColumnCount = 3;
  Column_info_t info[kColumnCount];

  for (int i = 0; i < kColumnCount; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = INTEGER;
  }

  info[0].name = "id";
  info[1].name = "tw_open";
  info[2].name = "tw_close";

  info[0].eType = ANY_INTEGER;  // id

  if (!is_plain) {
    info[1].eType = TIMESTAMP;  // tw_open
    info[2].eType = TIMESTAMP;  // tw_close
  }

  db_get_time_windows(sql, rows, total_rows, info, kColumnCount, 0, is_plain);
}

void
get_vroom_shipments_time_windows(
    char *sql,
    Vroom_time_window_t **rows,
    size_t *total_rows,
    bool is_plain) {
  int kColumnCount = 4;
  Column_info_t info[kColumnCount];

  for (int i = 0; i < kColumnCount; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = INTEGER;
  }

  info[0].name = "id";
  info[1].name = "kind";
  info[2].name = "tw_open";
  info[3].name = "tw_close";

  info[0].eType = ANY_INTEGER;  // id
  info[1].eType = CHAR1;        // kind

  if (!is_plain) {
    info[2].eType = TIMESTAMP;  // tw_open
    info[3].eType = TIMESTAMP;  // tw_close
  }

  db_get_time_windows(sql, rows, total_rows, info, kColumnCount, 1, is_plain);
}
