/*PGR-GNU*****************************************************************
File: vroom.c

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

/** @file vroom.c
 * @brief Connecting code with postgres.
 *
 */

#include <assert.h>
#include <stdbool.h>

#include "c_common/postgres_connection.h"
#include <utils/array.h>  // NOLINT [build/include_order]

#include "catalog/pg_type.h"
#include "utils/lsyscache.h"

#ifndef INT8ARRAYOID
#define INT8ARRAYOID    1016
#endif

#include "c_common/debug_macro.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"

#include "c_types/vroom/vroom_rt.h"
#include "c_types/vroom/vroom_job_t.h"
#include "c_types/vroom/vroom_shipment_t.h"
#include "c_types/vroom/vroom_vehicle_t.h"
#include "c_types/vroom/vroom_matrix_t.h"

#include "c_common/vroom/jobs_input.h"
#include "c_common/vroom/breaks_input.h"
#include "c_common/vroom/time_windows_input.h"
#include "c_common/vroom/shipments_input.h"
#include "c_common/vroom/vehicles_input.h"
#include "c_common/vroom/matrix_input.h"

#include "drivers/vroom/vroom_driver.h"

PGDLLEXPORT Datum _vrp_vroom(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_vrp_vroom);

/** @brief Static function, loads the data from postgres to C types for further processing.
 *
 * It first connects the C function to the SPI manager. Then converts
 * the postgres array to C array and loads the edges belonging to the graph
 * in C types. Then it calls the function `do_vrp_vroom` defined
 * in the `vroom_driver.h` file for further processing.
 * Finally, it frees the memory and disconnects the C function to the SPI manager.
 *
 * @param jobs_sql          SQL query describing the jobs
 * @param jobs_tw_sql       SQL query describing the time window for jobs
 * @param shipments_sql     SQL query describing the shipments
 * @param shipments_tw_sql  SQL query describing the time windows for shipment
 * @param vehicles_sql      SQL query describing the vehicles
 * @param breaks_sql        SQL query describing the driver breaks.
 * @param breaks_tws_sql    SQL query describing the time windows for break start.
 * @param matrix_sql        SQL query describing the cells of the cost matrix
 * @param exploration_level Exploration level to use while solving.
 * @param timeout           Timeout value to stop the solving process.
 * @param fn                Value denoting the function used.
 * @param is_plain          Value denoting whether the plain/timestamp function is used.
 * @param result_tuples     the rows in the result
 * @param result_count      the count of rows in the result
 *
 * @returns void
 */
static
void
process(
    char *jobs_sql,
    char *jobs_tws_sql,
    char *shipments_sql,
    char *shipments_tws_sql,
    char *vehicles_sql,
    char *breaks_sql,
    char *breaks_tws_sql,
    char *matrix_sql,

    int16_t exploration_level,
    int32_t timeout,
    int16_t fn,
    bool is_plain,

    Vroom_rt **result_tuples,
    size_t *result_count) {
  clock_t start_loading = clock();
  pgr_SPI_connect();

  (*result_tuples) = NULL;
  (*result_count) = 0;

  Vroom_job_t *jobs = NULL;
  size_t total_jobs = 0;
  if (jobs_sql) {
    get_vroom_jobs(jobs_sql, &jobs, &total_jobs, is_plain);
  }

  Vroom_shipment_t *shipments = NULL;
  size_t total_shipments = 0;
  if (shipments_sql) {
    get_vroom_shipments(shipments_sql, &shipments, &total_shipments, is_plain);
  }

  if (total_jobs == 0 && total_shipments == 0) {
    if (fn == 0) {
      ereport(WARNING, (errmsg("Insufficient data found on Jobs SQL and Shipments SQL query."),
                        errhint("%s, %s", jobs_sql, shipments_sql)));
    } else if (fn == 1) {
      ereport(WARNING, (errmsg("Insufficient data found on Jobs SQL query."),
                        errhint("%s", jobs_sql)));
    } else if (fn == 2) {
      ereport(WARNING, (errmsg("Insufficient data found on Shipments SQL query."),
                        errhint("%s", shipments_sql)));
    }
    (*result_count) = 0;
    (*result_tuples) = NULL;
    pgr_SPI_finish();
    return;
  }

  Vroom_time_window_t *jobs_tws = NULL;
  size_t total_jobs_tws = 0;
  if (jobs_tws_sql) {
    get_vroom_time_windows(jobs_tws_sql, &jobs_tws, &total_jobs_tws,
                           is_plain);
  }

  Vroom_time_window_t *shipments_tws = NULL;
  size_t total_shipments_tws = 0;
  if (shipments_tws_sql) {
    get_vroom_shipments_time_windows(shipments_tws_sql, &shipments_tws,
                                     &total_shipments_tws, is_plain);
  }

  Vroom_vehicle_t *vehicles = NULL;
  size_t total_vehicles = 0;
  get_vroom_vehicles(vehicles_sql, &vehicles, &total_vehicles, is_plain);

  if (total_vehicles == 0) {
    ereport(WARNING, (errmsg("Insufficient data found on Vehicles SQL query."),
                      errhint("%s", vehicles_sql)));
    (*result_count) = 0;
    (*result_tuples) = NULL;
    pgr_SPI_finish();
    return;
  }

  Vroom_break_t *breaks = NULL;
  size_t total_breaks = 0;
  if (breaks_sql) {
    get_vroom_breaks(breaks_sql, &breaks, &total_breaks, is_plain);
  }

  Vroom_time_window_t *breaks_tws = NULL;
  size_t total_breaks_tws = 0;
  if (breaks_tws_sql) {
    get_vroom_time_windows(breaks_tws_sql, &breaks_tws, &total_breaks_tws,
                           is_plain);
  }

  Vroom_matrix_t *matrix_rows = NULL;
  size_t total_matrix_rows = 0;
  get_vroom_matrix(matrix_sql, &matrix_rows, &total_matrix_rows, is_plain);

  if (total_matrix_rows == 0) {
    ereport(WARNING, (errmsg("Insufficient data found on Matrix SQL query."),
                      errhint("%s", matrix_sql)));
    (*result_count) = 0;
    (*result_tuples) = NULL;
    pgr_SPI_finish();
    return;
  }

  clock_t start_t = clock();
  char *log_msg = NULL;
  char *notice_msg = NULL;
  char *err_msg = NULL;

  int32_t loading_time = (int)((clock() - start_loading) / CLOCKS_PER_SEC) * 1000;

  do_vrp_vroom(
    jobs, total_jobs,
    jobs_tws, total_jobs_tws,
    shipments, total_shipments,
    shipments_tws, total_shipments_tws,
    vehicles, total_vehicles,
    breaks, total_breaks,
    breaks_tws, total_breaks_tws,
    matrix_rows, total_matrix_rows,

    exploration_level,
    timeout,
    loading_time,

    result_tuples,
    result_count,

    &log_msg,
    &notice_msg,
    &err_msg);

  time_msg("processing vrp_vroom", start_t, clock());

  if (err_msg && (*result_tuples)) {
    pfree(*result_tuples);
    (*result_tuples) = NULL;
    (*result_count) = 0;
  }

  pgr_global_report(log_msg, notice_msg, err_msg);

  if (log_msg) pfree(log_msg);
  if (notice_msg) pfree(notice_msg);
  if (err_msg) pfree(err_msg);

  if (jobs) pfree(jobs);
  if (shipments) pfree(shipments);
  if (vehicles) pfree(vehicles);
  if (matrix_rows) pfree(matrix_rows);

  pgr_SPI_finish();
}


/** @brief Helps in converting postgres variables to C variables, and returns the result.
 *
 */

PGDLLEXPORT Datum _vrp_vroom(PG_FUNCTION_ARGS) {
  FuncCallContext   *funcctx;
  TupleDesc       tuple_desc;

  /**********************************************************************/
  Vroom_rt *result_tuples = NULL;
  size_t result_count = 0;
  /**********************************************************************/

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext   oldcontext;
    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

    /***********************************************************************
     *
     *   _vrp_vroom(
     *     jobs_sql TEXT,
     *     jobs_time_windows_sql TEXT,
     *     shipments_sql TEXT,
     *     shipments_time_windows_sql TEXT,
     *     vehicles_sql TEXT,
     *     breaks_sql TEXT,
     *     breaks_time_windows_sql TEXT,
     *     matrix_sql TEXT,
     *     exploration_level SMALLINT default 5,
     *     timeout INTEGER default -1,
     *     fn SMALLINT,
     *     is_plain BOOLEAN
     *   );
     *
     **********************************************************************/

    char *args[8];
    for (int i = 0; i < 8; i++) {
      if (PG_ARGISNULL(i)) {
        args[i] = NULL;
      } else {
        args[i] = text_to_cstring(PG_GETARG_TEXT_P(i));
      }
    }

    int16_t exploration_level = PG_GETARG_INT16(8);
    int32_t timeout = PG_GETARG_INT32(9);
    int16_t fn = PG_GETARG_INT16(10);
    bool is_plain = PG_GETARG_BOOL(11);

    // Verify that both jobs_sql and shipments_sql are not NULL
    if (args[0] == NULL && args[2] == NULL) {
      if (fn == 0) {
        elog(ERROR, "Both Jobs SQL and Shipments NULL must not be NULL");
      } else if (fn == 1) {
        elog(ERROR, "Jobs SQL must not be NULL");
      } else if (fn == 2) {
        elog(ERROR, "Shipments SQL must not be NULL");
      }
    }

    if (args[4] == NULL) {
      elog(ERROR, "Vehicles SQL must not be NULL");
    }

    if (args[7] == NULL) {
      elog(ERROR, "Matrix SQL must not be NULL");
    }

    process(
        args[0],
        args[1],
        args[2],
        args[3],
        args[4],
        args[5],
        args[6],
        args[7],
        exploration_level,
        timeout,
        fn,
        is_plain,
        &result_tuples,
        &result_count);

    /**********************************************************************/


#if PGSQL_VERSION > 95
    funcctx->max_calls = result_count;
#else
    funcctx->max_calls = (uint32_t)result_count;
#endif
    funcctx->user_fctx = result_tuples;
    if (get_call_result_type(fcinfo, NULL, &tuple_desc)
        != TYPEFUNC_COMPOSITE) {
      ereport(ERROR,
          (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
           errmsg("function returning record called in context "
             "that cannot accept type record")));
    }

    funcctx->tuple_desc = tuple_desc;
    MemoryContextSwitchTo(oldcontext);
  }

  funcctx = SRF_PERCALL_SETUP();
  tuple_desc = funcctx->tuple_desc;
  result_tuples = (Vroom_rt*) funcctx->user_fctx;

  if (funcctx->call_cntr < funcctx->max_calls) {
    HeapTuple  tuple;
    Datum      result;
    Datum      *values;
    bool*      nulls;
    int16      typlen;
    size_t     call_cntr = funcctx->call_cntr;

    /***********************************************************************
     *
     *   OUT seq BIGINT,
     *   OUT vehicles_seq BIGINT,
     *   OUT vehicles_id BIGINT,
     *   OUT step_seq BIGINT,
     *   OUT step_type INTEGER,
     *   OUT task_id BIGINT,
     *   OUT arrival INTEGER,
     *   OUT duration INTEGER,
     *   OUT service_time INTEGER,
     *   OUT waiting_time INTEGER,
     *   OUT load BIGINT
     *
     **********************************************************************/

    size_t num  = 11;
    values = palloc(num * sizeof(Datum));
    nulls = palloc(num * sizeof(bool));

    size_t i;
    for (i = 0; i < num; ++i) {
      nulls[i] = false;
    }

    size_t load_size = (size_t)result_tuples[call_cntr].load_size;
    Datum* load = (Datum*) palloc(sizeof(Datum) * (size_t)load_size);

    for (i = 0; i < load_size; ++i) {
      load[i] = Int64GetDatum(result_tuples[call_cntr].load[i]);
    }

    bool typbyval;
    char typalign;
    get_typlenbyvalalign(INT8OID, &typlen, &typbyval, &typalign);
    ArrayType* arrayType;
    /*
      https://doxygen.postgresql.org/arrayfuncs_8c.html
      ArrayType* construct_array(
        Datum*      elems,
        int         nelems,
        Oid         elmtype, int elmlen, bool elmbyval, char elmalign
      )
    */
    arrayType =  construct_array(load, (int)load_size, INT8OID,  typlen,
                                typbyval, typalign);
    /*
      void TupleDescInitEntry(
        TupleDesc       desc,
        AttrNumber      attributeNumber,
        const char *    attributeName,
        Oid             oidtypeid,
        int32           typmod,
        int             attdim
      )
    */
    TupleDescInitEntry(tuple_desc, (AttrNumber) 11, "load", INT8ARRAYOID, -1, 0);

    values[0] = Int64GetDatum(funcctx->call_cntr + 1);
    values[1] = Int32GetDatum(result_tuples[call_cntr].vehicle_seq);
    values[2] = Int32GetDatum(result_tuples[call_cntr].vehicle_id);
    values[3] = Int32GetDatum(result_tuples[call_cntr].step_seq);
    values[4] = Int32GetDatum(result_tuples[call_cntr].step_type);
    values[5] = Int32GetDatum(result_tuples[call_cntr].task_id);
    values[6] = Int32GetDatum(result_tuples[call_cntr].arrival_time);
    values[7] = Int32GetDatum(result_tuples[call_cntr].travel_time);
    values[8] = Int32GetDatum(result_tuples[call_cntr].service_time);
    values[9] = Int32GetDatum(result_tuples[call_cntr].waiting_time);
    values[10] = PointerGetDatum(arrayType);

    /**********************************************************************/

    tuple = heap_form_tuple(tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}
