/*PGR-GNU*****************************************************************
File: matrix_input.c

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

#include "c_common/vroom/matrix_input.h"

void fetch_matrix(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_matrix_t *matrix,
    bool is_plain) {
  matrix->start_id = get_MatrixIndex(tuple, tupdesc, info[0], -1);
  matrix->end_id = get_MatrixIndex(tuple, tupdesc, info[1], -1);

  if (is_plain) {
    matrix->duration = get_Duration(tuple, tupdesc, info[2], 0);
  } else {
    matrix->duration = (Duration)get_PositiveTInterval(tuple, tupdesc, info[2], 0);
  }

  // If unspecified, cost is same as the duration
  matrix->cost = get_Cost(tuple, tupdesc, info[3], matrix->duration);
}


static
void db_get_matrix(
    char *matrix_sql,
    Vroom_matrix_t **matrix,
    size_t *total_matrix_rows,

    Column_info_t *info,
    const int column_count,
    bool is_plain) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", matrix_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = vrp_SPI_prepare(matrix_sql);
  Portal SPIportal;
  SPIportal = vrp_SPI_cursor_open(SPIplan);

  bool moredata = true;
  (*total_matrix_rows) = total_tuples = 0;

  /* on the first tuple get the column numbers */

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0) {
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*matrix) == NULL)
        (*matrix) = (Vroom_matrix_t *)palloc0(
            total_tuples * sizeof(Vroom_matrix_t));
      else
        (*matrix) = (Vroom_matrix_t *)repalloc(
            (*matrix),
            total_tuples * sizeof(Vroom_matrix_t));

      if ((*matrix) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        fetch_matrix(&tuple, &tupdesc, info,
            &(*matrix)[total_tuples - ntuples + t], is_plain);
      }
      SPI_freetuptable(tuptable);
    } else {
      moredata = false;
    }
  }

  SPI_cursor_close(SPIportal);

  if (total_tuples == 0) {
    (*total_matrix_rows) = 0;
    return;
  }

  (*total_matrix_rows) = total_tuples;
#ifdef PROFILE
  time_msg("reading matrix", start_t, clock());
#endif
}


void
get_vroom_matrix(
    char *sql,
    Vroom_matrix_t **rows,
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
  info[0].name = "start_id";
  info[1].name = "end_id";
  info[2].name = "duration";
  info[3].name = "cost";

  info[2].eType = INTEGER;     // duration
  info[3].eType = INTEGER;     // cost

  if (!is_plain) {
    info[2].eType = INTERVAL;  // duration
  }

  /**
   * cost is not mandatory
   */
  info[3].strict = false;

  db_get_matrix(sql, rows, total_rows, info, kColumnCount, is_plain);
}
