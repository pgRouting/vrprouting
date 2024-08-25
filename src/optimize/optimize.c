/*PGR-GNU*****************************************************************
File: optimize.c

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2021 Celia Virginia Vergara Castillo
Copyright (c) 2021 Joseph Emile Honour Percival

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

#include "c_common/postgres_connection.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"
#include "c_types/short_vehicle_rt.h"
#include "drivers/optimize_driver.h"

PGDLLEXPORT Datum
_vrp_optimize(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_vrp_optimize);


static
void
process(
        char* pd_orders_sql,
        char* vehicles_sql,
        char* matrix_sql,
        char* multipliers_sql,

        double  factor,
        int     max_cycles,
        int64_t execution_date,

        bool    check_triangle_inequality,
        int     subdivision_kind,
        bool    use_timestamps,

        Short_vehicle_rt **result_tuples,
        size_t *result_count) {
    char *log_msg = NULL;
    char *notice_msg = NULL;
    char *err_msg = NULL;

    vrp_SPI_connect();

    clock_t start_t = clock();
    vrp_do_optimize(
            pd_orders_sql,
            vehicles_sql,
            matrix_sql,
            multipliers_sql,

            factor,
            max_cycles,
            execution_date,

            check_triangle_inequality,
            subdivision_kind,

            use_timestamps,
            false,  // is_euclidean
            false,  // with_stops

            result_tuples,
            result_count,

            &log_msg,
            &notice_msg,
            &err_msg);
    time_msg("vrp_optimize", start_t, clock());

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_count) = 0;
        (*result_tuples) = NULL;
    }
    vrp_global_report(&log_msg, &notice_msg, &err_msg);

    vrp_SPI_finish();
}


PGDLLEXPORT Datum
_vrp_optimize(PG_FUNCTION_ARGS) {
  FuncCallContext     *funcctx;
  TupleDesc            tuple_desc;

  Short_vehicle_rt *result_tuples = 0;
  size_t result_count = 0;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext   oldcontext;
    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

    process(
        text_to_cstring(PG_GETARG_TEXT_P(0)),
        text_to_cstring(PG_GETARG_TEXT_P(1)),
        text_to_cstring(PG_GETARG_TEXT_P(2)),
        text_to_cstring(PG_GETARG_TEXT_P(3)),

        PG_GETARG_FLOAT8(4),
        PG_GETARG_INT32(5),
        PG_GETARG_INT64(6),

        PG_GETARG_BOOL(7),
        PG_GETARG_INT32(8),
        PG_GETARG_BOOL(9),

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
  result_tuples = (Short_vehicle_rt*) funcctx->user_fctx;

  if (funcctx->call_cntr <  funcctx->max_calls) {
    HeapTuple   tuple;
    Datum       result;
    Datum       *values;
    bool*       nulls;
    size_t      call_cntr = funcctx->call_cntr;

    size_t numb = 3;
    values = palloc(numb * sizeof(Datum));
    nulls = palloc(numb * sizeof(bool));

    size_t i;
    for (i = 0; i < numb; ++i) {
      nulls[i] = false;
    }


    values[0] = Int32GetDatum(funcctx->call_cntr + 1);
    values[1] = Int64GetDatum(result_tuples[call_cntr].vehicle_id);
    values[2] = Int64GetDatum(result_tuples[call_cntr].order_id);

    tuple = heap_form_tuple(tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);

    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}
