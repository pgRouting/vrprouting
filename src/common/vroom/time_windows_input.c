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

/*
.. vrp_vroom start

A ``SELECT`` statement that returns the following columns:

::

    id [, kind], tw_open, tw_close

====================  ====================================== =====================================================
Column                Type                                   Description
====================  ====================================== =====================================================
**id**                ``ANY-INTEGER``                         Non-negative unique identifier of the job,
                                                              pickup/delivery shipment, or break.

**kind**              ``CHAR``                                **Only required for shipments**. Value in ['p', 'd']
                                                              indicating whether the time window is for:

                                                              - Pickup shipment, or
                                                              - Delivery shipment.

**tw_open**           ``INTEGER``                             Time window opening time.

**tw_close**          ``INTEGER``                             Time window closing time.
====================  ====================================== =====================================================

**Note**:

- All timing are in seconds.
- Every row must satisfy the condition: :code:`tw_open ≤ tw_close`.
- It is up to users to decide how to describe time windows:

  - **Relative values**, e.g. [0, 14400] for a 4 hours time window starting at the beginning of the planning horizon. In that case all times reported in output with the arrival column are relative to the start of the planning horizon.
  - **Absolute values**, "real" timestamps. In that case all times reported in output with the arrival column can be interpreted as timestamps.

.. vrp_vroom end
*/

static
void fetch_time_windows(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_time_window_t *time_window,
    bool is_shipment) {

  time_window->id = get_Idx(tuple, tupdesc, info[0], 0);

  if (is_shipment) {
    char kind = get_Kind(tuple, tupdesc, info[1], ' ');
    if (kind != 'p' && kind != 'd') {
      ereport(ERROR, (errmsg("Invalid kind %c", kind),
                      errhint("Kind must be either 'p' or 'd'")));
    }
    time_window->kind = kind;
    time_window->start_time = get_Duration(tuple, tupdesc, info[2], 0);
    time_window->end_time = get_Duration(tuple, tupdesc, info[3], 0);
  } else {
    time_window->start_time = get_Duration(tuple, tupdesc, info[1], 0);
    time_window->end_time = get_Duration(tuple, tupdesc, info[2], 0);
  }

  if (time_window->start_time > time_window->end_time) {
    ereport(ERROR,
        (errmsg("Invalid time window (%d, %d)",
            time_window->start_time, time_window->end_time),
         errhint("Time window start time %d must be "
             "less than or equal to time window end time %d",
             time_window->start_time, time_window->end_time)));
  }
}


static
void db_get_time_windows(
    char *time_windows_sql,
    Vroom_time_window_t **time_windows,
    size_t *total_time_windows,

    Column_info_t *info,
    const int column_count,
    bool is_shipment) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", time_windows_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = pgr_SPI_prepare(time_windows_sql);
  Portal SPIportal;
  SPIportal = pgr_SPI_cursor_open(SPIplan);

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
            &(*time_windows)[total_tuples - ntuples + t], is_shipment);
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


/**
 * @param[in] sql SQL query to execute
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vroom_time_windows(
    char *sql,
    Vroom_time_window_t **rows,
    size_t *total_rows) {
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

  db_get_time_windows(sql, rows, total_rows, info, kColumnCount, 0);
}

/**
 * @param[in] sql SQL query to execute
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vroom_shipments_time_windows(
    char *sql,
    Vroom_time_window_t **rows,
    size_t *total_rows) {
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

  db_get_time_windows(sql, rows, total_rows, info, kColumnCount, 1);
}
