/*PGR-GNU*****************************************************************
File: shipments_input.c

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

#include "c_common/vroom/shipments_input.h"

static
void fetch_shipments(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_shipment_t *shipment,
    bool is_plain) {
  shipment->id = get_Idx(tuple, tupdesc, info[0], 0);

  shipment->p_location_id = get_MatrixIndex(tuple, tupdesc, info[1], 0);
  shipment->d_location_id = get_MatrixIndex(tuple, tupdesc, info[4], 0);

  if (is_plain) {
    shipment->p_setup = get_Duration(tuple, tupdesc, info[2], 0);
    shipment->p_service = get_Duration(tuple, tupdesc, info[3], 0);
    shipment->d_setup = get_Duration(tuple, tupdesc, info[5], 0);
    shipment->d_service = get_Duration(tuple, tupdesc, info[6], 0);
  } else {
    shipment->p_setup =
        (Duration)get_PositiveTInterval(tuple, tupdesc, info[2], 0);
    shipment->p_service =
        (Duration)get_PositiveTInterval(tuple, tupdesc, info[3], 0);
    shipment->d_setup =
        (Duration)get_PositiveTInterval(tuple, tupdesc, info[5], 0);
    shipment->d_service =
        (Duration)get_PositiveTInterval(tuple, tupdesc, info[6], 0);
  }

  shipment->amount_size = 0;
  shipment->amount = column_found(info[7].colNumber) ?
    spi_getPositiveBigIntArr_allowEmpty(tuple, tupdesc, info[7], &shipment->amount_size)
    : NULL;

  shipment->skills_size = 0;
  shipment->skills = column_found(info[8].colNumber) ?
    spi_getPositiveIntArr_allowEmpty(tuple, tupdesc, info[8], &shipment->skills_size)
    : NULL;

  shipment->priority = get_Priority(tuple, tupdesc, info[9], 0);

  shipment->p_data = column_found(info[10].colNumber)
                         ? spi_getText(tuple, tupdesc, info[10])
                         : strdup("{}");
  shipment->d_data = column_found(info[11].colNumber)
                         ? spi_getText(tuple, tupdesc, info[11])
                         : strdup("{}");
}


static
void db_get_shipments(
    char *shipments_sql,
    Vroom_shipment_t **shipments,
    size_t *total_shipments,

    Column_info_t *info,
    const int column_count,
    bool is_plain) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", shipments_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = vrp_SPI_prepare(shipments_sql);
  Portal SPIportal;
  SPIportal = vrp_SPI_cursor_open(SPIplan);

  bool moredata = true;
  (*total_shipments) = total_tuples = 0;

  /* on the first tuple get the column numbers */

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0) {
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*shipments) == NULL)
        (*shipments) = (Vroom_shipment_t *)palloc0(
            total_tuples * sizeof(Vroom_shipment_t));
      else
        (*shipments) = (Vroom_shipment_t *)repalloc(
            (*shipments),
            total_tuples * sizeof(Vroom_shipment_t));

      if ((*shipments) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        fetch_shipments(&tuple, &tupdesc, info,
            &(*shipments)[total_tuples - ntuples + t], is_plain);
      }
      SPI_freetuptable(tuptable);
    } else {
      moredata = false;
    }
  }

  SPI_cursor_close(SPIportal);

  if (total_tuples == 0) {
    (*total_shipments) = 0;
    return;
  }

  (*total_shipments) = total_tuples;
#ifdef PROFILE
  time_msg("reading shipments", start_t, clock());
#endif
}


/**
 * @param[in] sql SQL query to execute
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vroom_shipments(
    char *sql,
    Vroom_shipment_t **rows,
    size_t *total_rows,
    bool is_plain) {
  int kColumnCount = 12;
  Column_info_t info[kColumnCount];

  for (int i = 0; i < kColumnCount; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = false;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";

  /* pickup shipments */
  info[1].name = "p_location_id";
  info[2].name = "p_setup";
  info[3].name = "p_service";

  /* delivery shipments */
  info[4].name = "d_location_id";
  info[5].name = "d_setup";
  info[6].name = "d_service";

  info[7].name = "amount";
  info[8].name = "skills";
  info[9].name = "priority";
  info[10].name = "p_data";
  info[11].name = "d_data";

  info[2].eType = INTEGER;            // p_setup
  info[3].eType = INTEGER;            // p_service
  info[5].eType = INTEGER;            // d_setup
  info[6].eType = INTEGER;            // d_service
  info[7].eType = ANY_INTEGER_ARRAY;  // amount
  info[8].eType = INTEGER_ARRAY;      // skills
  info[9].eType = INTEGER;            // priority
  info[10].eType = JSONB;             // p_data
  info[11].eType = JSONB;             // d_data

  if (!is_plain) {
    info[2].eType = INTERVAL;         // p_setup
    info[3].eType = INTERVAL;         // p_service
    info[5].eType = INTERVAL;         // d_setup
    info[6].eType = INTERVAL;         // d_service
  }

  /* id and location_id of pickup and delivery are mandatory */
  info[0].strict = true;
  info[1].strict = true;
  info[4].strict = true;

  db_get_shipments(sql, rows, total_rows, info, kColumnCount, is_plain);
}
