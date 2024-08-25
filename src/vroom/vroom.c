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

#include "c_types/vroom_rt.h"

#include "drivers/vroom_driver.h"

PGDLLEXPORT Datum _vrp_vroom(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_vrp_vroom);

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

        int32_t exploration_level,
        int32_t timeout,
        int16_t fn,
        bool use_timestamps,

        Vroom_rt **result_tuples,
        size_t *result_count) {
    char *log_msg = NULL;
    char *notice_msg = NULL;
    char *err_msg = NULL;

    vrp_SPI_connect();

    clock_t start_t = clock();
    vrp_do_vroom(
            jobs_sql,
            jobs_tws_sql,
            shipments_sql,
            shipments_tws_sql,
            vehicles_sql,
            breaks_sql,
            breaks_tws_sql,
            matrix_sql,

            exploration_level,
            timeout,
            fn,
            use_timestamps,

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

    vrp_global_report(&log_msg, &notice_msg, &err_msg);

    vrp_SPI_finish();
}


PGDLLEXPORT Datum _vrp_vroom(PG_FUNCTION_ARGS) {
  FuncCallContext   *funcctx;
  TupleDesc       tuple_desc;

  Vroom_rt *result_tuples = NULL;
  size_t result_count = 0;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext   oldcontext;
    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

    char *args[8];
    for (int i = 0; i < 8; i++) {
      if (PG_ARGISNULL(i)) {
        args[i] = NULL;
      } else {
        args[i] = text_to_cstring(PG_GETARG_TEXT_P(i));
      }
    }

    int32_t exploration_level = PG_GETARG_INT32(8);
    int32_t timeout = PG_GETARG_INT32(9);
    int16_t fn = PG_GETARG_INT16(10);
    bool is_plain = PG_GETARG_BOOL(11);


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
        !is_plain,
        &result_tuples,
        &result_count);

    funcctx->max_calls = result_count;
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

    size_t num  = 16;
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
    TupleDescInitEntry(tuple_desc, (AttrNumber) 16, "load", INT8ARRAYOID, -1, 0);

    values[0] = Int64GetDatum(funcctx->call_cntr + 1);
    values[1] = Int64GetDatum(result_tuples[call_cntr].vehicle_seq);
    values[2] = Int64GetDatum(result_tuples[call_cntr].vehicle_id);
    values[3] = CStringGetTextDatum(result_tuples[call_cntr].vehicle_data);
    values[4] = Int64GetDatum(result_tuples[call_cntr].step_seq);
    values[5] = Int32GetDatum(result_tuples[call_cntr].step_type);
    values[6] = Int64GetDatum(result_tuples[call_cntr].task_id);
    values[7] = Int64GetDatum(result_tuples[call_cntr].location_id);
    values[8] = CStringGetTextDatum(result_tuples[call_cntr].task_data);
    values[9] = Int32GetDatum(result_tuples[call_cntr].arrival_time);
    values[10] = Int32GetDatum(result_tuples[call_cntr].travel_time);
    values[11] = Int32GetDatum(result_tuples[call_cntr].setup_time);
    values[12] = Int32GetDatum(result_tuples[call_cntr].service_time);
    values[13] = Int32GetDatum(result_tuples[call_cntr].waiting_time);
    values[14] = Int32GetDatum(result_tuples[call_cntr].departure_time);
    values[15] = PointerGetDatum(arrayType);

    tuple = heap_form_tuple(tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}
