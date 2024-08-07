/*PGR-GNU*****************************************************************
File: pd_orders_input.c

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

#include "c_common/orders_input.h"

#include "cpp_common/orders_t.hpp"
#include "cpp_common/info.hpp"

#include "c_common/debug_macro.h"
#include "cpp_common/get_check_data.hpp"
#ifdef PROFILE
#include "c_common/time_msg.h"
#endif


static
void fetch_euclidean(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Orders_t *pd_order) {
  pd_order->id = get_Id(tuple, tupdesc, info[0], -1);
  pd_order->demand = get_PositiveAmount(tuple, tupdesc, info[1], 0);

  /*
   * the pickups
   */
  pd_order->pick_open_t    = get_TTimestamp_plain(tuple, tupdesc, info[2], -1);
  pd_order->pick_close_t   = get_TTimestamp_plain(tuple, tupdesc, info[3], -1);
  pd_order->pick_service_t = get_TInterval_plain(tuple, tupdesc, info[4], 0);

  /*
   * the deliveries
   */
  pd_order->deliver_open_t    = get_TTimestamp_plain(tuple, tupdesc, info[5], -1);
  pd_order->deliver_close_t   = get_TTimestamp_plain(tuple, tupdesc, info[6], -1);
  pd_order->deliver_service_t = get_TInterval_plain(tuple, tupdesc, info[7], 0);

  pd_order->pick_x =  spi_getCoordinate(tuple, tupdesc, info[8], 0);
  pd_order->pick_y =  spi_getCoordinate(tuple, tupdesc, info[9], 0);
  pd_order->deliver_x =  spi_getCoordinate(tuple, tupdesc, info[10], 0);
  pd_order->deliver_y =  spi_getCoordinate(tuple, tupdesc, info[11], 0);

  /*
   * ignored information
   */
  pd_order->pick_node_id = 0;
  pd_order->deliver_node_id = 0;
}


static
void fetch_raw(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Orders_t *pd_order) {
  pd_order->id = get_Id(tuple, tupdesc, info[0], -1);
  pd_order->demand = get_PositiveAmount(tuple, tupdesc, info[1], 0);

  /*
   * the pickups
   */
  pd_order->pick_node_id   = get_Id(tuple, tupdesc, info[8], -1);
  pd_order->pick_open_t    = get_TTimestamp_plain(tuple, tupdesc, info[2], -1);
  pd_order->pick_close_t   = get_TTimestamp_plain(tuple, tupdesc, info[3], -1);
  pd_order->pick_service_t = get_TInterval_plain(tuple, tupdesc, info[4], 0);

  /*
   * the deliveries
   */
  pd_order->deliver_node_id   = get_Id(tuple, tupdesc, info[9], -1);
  pd_order->deliver_open_t    = get_TTimestamp_plain(tuple, tupdesc, info[5], -1);
  pd_order->deliver_close_t   = get_TTimestamp_plain(tuple, tupdesc, info[6], -1);
  pd_order->deliver_service_t = get_TInterval_plain(tuple, tupdesc, info[7], 0);


  /*
   * Ignored information
   */
  pd_order->pick_x =  0;
  pd_order->pick_y =  0;
  pd_order->deliver_x =  0;
  pd_order->deliver_y =  0;
}

static
void fetch_timestamps(
        HeapTuple *tuple,
        TupleDesc *tupdesc,
        Column_info_t *info,
        Orders_t *pd_order) {
    pd_order->id = get_Id(tuple, tupdesc, info[0], -1);
    pd_order->demand = get_PositiveAmount(tuple, tupdesc, info[1], 0);

    /*
     * the pickups
     */
    pd_order->pick_node_id   = get_Id(tuple, tupdesc, info[2], -1);
    pd_order->pick_open_t    = get_TTimestamp(tuple, tupdesc, info[3], -1);
    pd_order->pick_close_t   = get_TTimestamp(tuple, tupdesc, info[4], -1);
    pd_order->pick_service_t = get_TInterval(tuple, tupdesc, info[5], 0);

    /*
     * the deliveries
     */
    pd_order->deliver_node_id   = get_Id(tuple, tupdesc, info[6], -1);
    pd_order->deliver_open_t    = get_TTimestamp(tuple, tupdesc, info[7], -1);
    pd_order->deliver_close_t   = get_TTimestamp(tuple, tupdesc, info[8], -1);
    pd_order->deliver_service_t = get_TInterval(tuple, tupdesc, info[9], 0);

    PGR_DBG("pick_service_t %ld deliver_service_t %ld", pd_order->pick_service_t, pd_order->deliver_service_t);

    /*
     * Ignored information
     */
    pd_order->pick_x =  0;
    pd_order->pick_y =  0;
    pd_order->deliver_x =  0;
    pd_order->deliver_y =  0;
}



static
void
pgr_get_pd_orders_general(
        char *pd_orders_sql,
        Orders_t **pd_orders,
        size_t *total_pd_orders,

        Column_info_t *info,
        const int column_count,

        int kind) {
#ifdef PROFILE
    clock_t start_t = clock();
    PGR_DBG("%s", pd_orders_sql);
#endif

    const int tuple_limit = 1000000;


    size_t total_tuples;

    void *SPIplan;
    SPIplan = vrp_SPI_prepare(pd_orders_sql);
    Portal SPIportal;
    SPIportal = vrp_SPI_cursor_open(SPIplan);

    bool moredata = true;
    (*total_pd_orders) = total_tuples = 0;

    /* on the first tuple get the column numbers */

    while (moredata == true) {
        SPI_cursor_fetch(SPIportal, true, tuple_limit);
        if (total_tuples == 0) {
            pgr_fetch_column_info(info, column_count);
        }
        size_t ntuples = SPI_processed;
        total_tuples += ntuples;
        if (ntuples > 0) {
            if ((*pd_orders) == NULL)
                (*pd_orders) = (Orders_t *)palloc0(
                        total_tuples * sizeof(Orders_t));
            else
                (*pd_orders) = (Orders_t *)repalloc(
                        (*pd_orders),
                        total_tuples * sizeof(Orders_t));

            if ((*pd_orders) == NULL) {
                elog(ERROR, "Out of memory");
            }

            size_t t;
            SPITupleTable *tuptable = SPI_tuptable;
            TupleDesc tupdesc = SPI_tuptable->tupdesc;
            for (t = 0; t < ntuples; t++) {
                HeapTuple tuple = tuptable->vals[t];
                switch (kind) {
                case 0 : fetch_timestamps(&tuple, &tupdesc, info,
                        &(*pd_orders)[total_tuples - ntuples + t]);
                    break;
                case 1 : fetch_raw(&tuple, &tupdesc, info,
                        &(*pd_orders)[total_tuples - ntuples + t]);
                    break;
                case 2 : fetch_euclidean(&tuple, &tupdesc, info,
                        &(*pd_orders)[total_tuples - ntuples + t]);
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
        (*total_pd_orders) = 0;
        return;
    }

    (*total_pd_orders) = total_tuples;
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
get_shipments(
    char *sql,
    Orders_t **rows,
    size_t *total_rows) {
  const int column_count = 10;
  Column_info_t info[10];

  for (int i = 0; i < column_count; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "amount";
  info[2].name = "p_id";
  info[3].name = "p_tw_open";
  info[4].name = "p_tw_close";
  info[5].name = "p_t_service";
  info[6].name = "d_id";
  info[7].name = "d_tw_open";
  info[8].name = "d_tw_close";
  info[9].name = "d_t_service";

  info[3].eType = TIMESTAMP;
  info[4].eType = TIMESTAMP;
  info[7].eType = TIMESTAMP;
  info[8].eType = TIMESTAMP;
  info[5].eType = INTERVAL;
  info[9].eType = INTERVAL;

  /* service is optional*/
  info[5].strict = false;
  info[9].strict = false;

  pgr_get_pd_orders_general(sql, rows, total_rows, info, column_count, 0);
}

/**
 * @param[in] sql SQL query to execute
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_shipments_raw(
    char *sql,
    Orders_t **rows,
    size_t *total_rows) {
  const int column_count = 10;
  Column_info_t info[10];

  for (int i = 0; i < column_count; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "amount";
  info[2].name = "p_open";
  info[3].name = "p_close";
  info[4].name = "p_service";
  info[5].name = "d_open";
  info[6].name = "d_close";
  info[7].name = "d_service";
  info[8].name = "p_id";
  info[9].name = "d_id";

  /* service is optional*/
  info[4].strict = false;
  info[7].strict = false;

  pgr_get_pd_orders_general(sql, rows, total_rows, info, column_count, 1);
}

/**
 * @param[in] sql SQL query to execute
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_shipments_euclidean(
    char *sql,
    Orders_t **rows,
    size_t *total_rows) {
  const int column_count = 12;
  Column_info_t info[12];

  for (int i = 0; i < column_count; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = true;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "amount";
  info[2].name = "p_open";
  info[3].name = "p_close";
  info[4].name = "p_service";
  info[5].name = "d_open";
  info[6].name = "d_close";
  info[7].name = "d_service";
  info[8].name = "p_x";
  info[9].name = "p_y";
  info[10].name = "d_x";
  info[11].name = "d_y";

  /* service is optional*/
  info[4].strict = false;
  info[7].strict = false;

  /* (x,y) are ignored*/
  info[8].eType = ANY_NUMERICAL;
  info[9].eType = ANY_NUMERICAL;
  info[10].eType = ANY_NUMERICAL;
  info[11].eType = ANY_NUMERICAL;

  pgr_get_pd_orders_general(sql, rows, total_rows, info, column_count, 2);
}
