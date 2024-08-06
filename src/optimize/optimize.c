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

#include <stdbool.h>
#include "c_common/postgres_connection.h"
#include "c_common/debug_macro.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"
#include "c_common/orders_input.h"
#include "c_common/vehicles_input.h"
#include "c_common/matrixRows_input.h"
#include "c_common/time_multipliers_input.h"
#include "c_types/short_vehicle_rt.h"
#include "cpp_common/orders_t.hpp"
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
    //! [Factor must be postive]
    if (factor <= 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("Illegal value in parameter: factor"),
                 errhint("Value found: %f <= 0", factor)));
    }

    //! [max_cycles must be postive]
    if (max_cycles < 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("Illegal value in parameter: max_cycles"),
                 errhint("Value found: %d <= 0", max_cycles)));
    }

    //! [subdivision_kind can be: 0, 1, or 2]
    if (subdivision_kind < 0 || subdivision_kind > 2) {
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("Illegal value in parameter: subdivision_kind"),
                 errhint("Value found: %d", max_cycles)));
    }

    vrp_SPI_connect();

    Orders_t *pd_orders_arr = NULL;
    size_t total_pd_orders = 0;
    if (use_timestamps) {
        get_shipments(pd_orders_sql, &pd_orders_arr, &total_pd_orders);
    } else {
        get_shipments_raw(pd_orders_sql, &pd_orders_arr, &total_pd_orders);
    }

    if (total_pd_orders == 0) {
        (*result_count) = 0;
        (*result_tuples) = NULL;

        /* freeing memory before return */
        if (pd_orders_arr) {
            pfree(pd_orders_arr); pd_orders_arr = NULL;
        }

        vrp_SPI_finish();
        return;
    }

    Vehicle_t *vehicles_arr = NULL;
    size_t total_vehicles = 0;
    if (use_timestamps) {
        get_vehicles(vehicles_sql, &vehicles_arr, &total_vehicles, true);
    } else {
        get_vehicles_raw(vehicles_sql, &vehicles_arr, &total_vehicles, true);
    }

    if (total_vehicles == 0) {
        (*result_count) = 0;
        (*result_tuples) = NULL;

        /* freeing memory before return */
        if (pd_orders_arr) {
            pfree(pd_orders_arr); pd_orders_arr = NULL;
        }
        if (vehicles_arr) {
            pfree(vehicles_arr); vehicles_arr = NULL;
        }

        ereport(WARNING,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("No vehicles found")));

        vrp_SPI_finish();
        return;
    }

    Time_multipliers_t *multipliers_arr = NULL;
    size_t total_multipliers_arr = 0;
    if (use_timestamps) {
        get_timeMultipliers(multipliers_sql, &multipliers_arr, &total_multipliers_arr);
    } else {
        get_timeMultipliers_raw(multipliers_sql, &multipliers_arr, &total_multipliers_arr);
    }

    if (total_multipliers_arr == 0) {
        ereport(WARNING,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("No matrix found")));
        (*result_count) = 0;
        (*result_tuples) = NULL;

        /* freeing memory before return */
        if (pd_orders_arr) {
            pfree(pd_orders_arr); pd_orders_arr = NULL;
        }
        if (vehicles_arr) {
            pfree(vehicles_arr); vehicles_arr = NULL;
        }
        if (multipliers_arr) {
            pfree(multipliers_arr); multipliers_arr = NULL;
        }

        vrp_SPI_finish();
        return;
    }

    Matrix_cell_t *matrix_cells_arr = NULL;
    size_t total_cells = 0;
    if (use_timestamps) {
        get_matrixRows(matrix_sql, &matrix_cells_arr, &total_cells);
    } else {
        get_matrixRows_plain(matrix_sql, &matrix_cells_arr, &total_cells);
    }

    if (total_cells == 0) {
        (*result_count) = 0;
        (*result_tuples) = NULL;

        /* freeing memory before return */
        if (pd_orders_arr) {
            pfree(pd_orders_arr); pd_orders_arr = NULL;
        }
        if (vehicles_arr) {
            pfree(vehicles_arr); vehicles_arr = NULL;
        }
        if (multipliers_arr) {
            pfree(multipliers_arr); multipliers_arr = NULL;
        }
        if (matrix_cells_arr) {
            pfree(matrix_cells_arr); matrix_cells_arr = NULL;
        }

        ereport(WARNING,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("No matrix found")));
        vrp_SPI_finish();
        return;
    }

    PGR_DBG("Total %ld orders in query:", total_pd_orders);
    PGR_DBG("Total %ld vehicles in query:", total_vehicles);
    PGR_DBG("Total %ld matrix cells in query:", total_cells);
    PGR_DBG("Total %ld time dependant multipliers:", total_multipliers_arr);

    clock_t start_t = clock();
    char *log_msg = NULL;
    char *notice_msg = NULL;
    char *err_msg = NULL;

    do_optimize(
            pd_orders_arr,    total_pd_orders,
            vehicles_arr,     total_vehicles,
            matrix_cells_arr, total_cells,
            multipliers_arr,      total_multipliers_arr,

            factor,
            max_cycles,
            execution_date,

            check_triangle_inequality,
            subdivision_kind != 0,
            subdivision_kind == 1,

            result_tuples,
            result_count,

            &log_msg,
            &notice_msg,
            &err_msg);

    time_msg("pgr_pickDeliver", start_t, clock());

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_count) = 0;
        (*result_tuples) = NULL;
    }
    vrp_global_report(&log_msg, &notice_msg, &err_msg);

    /* freeing memory before return */
    if (pd_orders_arr) {
        pfree(pd_orders_arr); pd_orders_arr = NULL;
    }
    if (vehicles_arr) {
        pfree(vehicles_arr); vehicles_arr = NULL;
    }
    if (multipliers_arr) {
        pfree(multipliers_arr); multipliers_arr = NULL;
    }
    if (matrix_cells_arr) {
        pfree(matrix_cells_arr); matrix_cells_arr = NULL;
    }

    vrp_SPI_finish();
}



/******************************************************************************/


PGDLLEXPORT Datum
_vrp_optimize(PG_FUNCTION_ARGS) {
  FuncCallContext     *funcctx;
  TupleDesc            tuple_desc;

  /**************************************************************************/
  Short_vehicle_rt *result_tuples = 0;
  size_t result_count = 0;
  /**************************************************************************/

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

    /*********************************************************************/

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

    /*********************************************************************/

    tuple = heap_form_tuple(tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);

    pfree(values); values = NULL;
    pfree(nulls); nulls = NULL;

    SRF_RETURN_NEXT(funcctx, result);
  } else {
    if (result_tuples) {
      pfree(result_tuples); result_tuples = NULL;
    }
    funcctx->user_fctx = NULL;
    SRF_RETURN_DONE(funcctx);
  }
}
