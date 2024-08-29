/*PGR-GNU*****************************************************************
File: pickDeliverEuclidean.c

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo

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
#include "utils/array.h"

#include "c_common/debug_macro.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"
#include "c_common/orders_input.h"
#include "c_common/vehicles_input.h"
#include "cpp_common/orders_t.hpp"
#include "c_types/solution_rt.h"

#include "drivers/pgr_pickDeliverEuclidean_driver.h"

PGDLLEXPORT Datum _vrp_pgr_pickdelivereuclidean(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_vrp_pgr_pickdelivereuclidean);


static
void
process(
        char* pd_orders_sql,
        char* vehicles_sql,
        double factor,
        int max_cycles,
        int initial_solution_id,
        Solution_rt **result_tuples,
        size_t *result_count) {
    if (factor <= 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("Illegal value in parameter: factor"),
                 errhint("Value found: %f <= 0", factor)));
        (*result_count) = 0;
        (*result_tuples) = NULL;
        return;
    }

    if (max_cycles < 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("Illegal value in parameter: max_cycles"),
                 errhint("Negative value found: max_cycles: %d ", max_cycles)));
        (*result_count) = 0;
        (*result_tuples) = NULL;
        return;
    }

    if (initial_solution_id <= 0 || initial_solution_id > 6) {
        elog(ERROR, "Illegal value in parameter: initial_sol");
        (*result_count) = 0;
        (*result_tuples) = NULL;
        return;
    }

    vrp_SPI_connect();

    PGR_DBG("Load orders");
    struct Orders_t *pd_orders_arr = NULL;
    size_t total_pd_orders = 0;
    get_shipments_euclidean(pd_orders_sql,
           &pd_orders_arr, &total_pd_orders);

    PGR_DBG("Load vehicles");
    Vehicle_t *vehicles_arr = NULL;
    size_t total_vehicles = 0;
    get_vehicles_euclidean(vehicles_sql,
           &vehicles_arr, &total_vehicles,
           false);
    PGR_DBG("total vehicles %ld", total_vehicles);

#if 0
    for (size_t i = 0; i < total_pd_orders; i++) {
        PGR_DBG("%ld %f pick %f %f %ld - "
                "%ld %ld %ld deliver %f %f %ld - %ld %ld %ld ",
                pd_orders_arr[i].id,
                pd_orders_arr[i].demand,

                pd_orders_arr[i].pick_x,
                pd_orders_arr[i].pick_y,
                pd_orders_arr[i].pick_node_id,

                pd_orders_arr[i].pick_open_t,
                pd_orders_arr[i].pick_close_t,
                pd_orders_arr[i].pick_service_t,

                pd_orders_arr[i].deliver_x,
                pd_orders_arr[i].deliver_y,
                pd_orders_arr[i].deliver_node_id,

                pd_orders_arr[i].deliver_open_t,
                pd_orders_arr[i].deliver_close_t,
                pd_orders_arr[i].deliver_service_t);
    }



    for (size_t i = 0; i < total_vehicles; i++) {
        PGR_DBG("%ld %f %f , start %f %f %ld %ld %ld "
                "end %f %f %ld %ld %ld number %ld ",
               vehicles_arr[i].id,
               vehicles_arr[i].capacity,
               vehicles_arr[i].speed,

               vehicles_arr[i].start_x,
               vehicles_arr[i].start_y,
               vehicles_arr[i].start_open_t,
               vehicles_arr[i].start_close_t,
               vehicles_arr[i].start_service_t,

               vehicles_arr[i].end_x,
               vehicles_arr[i].end_y,
               vehicles_arr[i].end_open_t,
               vehicles_arr[i].end_close_t,
               vehicles_arr[i].end_service_t,

               vehicles_arr[i].cant_v);
    }
#endif

    if (total_pd_orders == 0 || total_vehicles == 0) {
        (*result_count) = 0;
        (*result_tuples) = NULL;
        vrp_SPI_finish();
        return;
    }
    PGR_DBG("Total %ld orders in query:", total_pd_orders);

    PGR_DBG("Starting processing");
    clock_t start_t = clock();
    char *log_msg = NULL;
    char *notice_msg = NULL;
    char *err_msg = NULL;
    vrp_do_pgr_pickDeliverEuclidean(
            pd_orders_arr, total_pd_orders,
            vehicles_arr, total_vehicles,

            factor,
            max_cycles,
            initial_solution_id,

            result_tuples,
            result_count,

            &log_msg,
            &notice_msg,
            &err_msg);
    time_msg("_pgr_pickDeliverEuclidean", start_t, clock());

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_count) = 0;
        (*result_tuples) = NULL;
    }

    vrp_global_report(&log_msg, &notice_msg, &err_msg);

    if (pd_orders_arr) pfree(pd_orders_arr);
    if (vehicles_arr) pfree(vehicles_arr);

    vrp_SPI_finish();
}

PGDLLEXPORT Datum
_vrp_pgr_pickdelivereuclidean(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    TupleDesc            tuple_desc;

    Solution_rt *result_tuples = 0;
    size_t result_count = 0;

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        PGR_DBG("Calling process");
        process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                text_to_cstring(PG_GETARG_TEXT_P(1)),
                PG_GETARG_FLOAT8(2),
                PG_GETARG_INT32(3),
                PG_GETARG_INT32(4),
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
    result_tuples = (Solution_rt*) funcctx->user_fctx;

    if (funcctx->call_cntr <  funcctx->max_calls) {
        HeapTuple   tuple;
        Datum       result;
        Datum       *values;
        bool*       nulls;
        size_t      call_cntr = funcctx->call_cntr;

        size_t numb = 12;
        values = palloc(numb * sizeof(Datum));
        nulls = palloc(numb * sizeof(bool));

        size_t i;
        for (i = 0; i < numb; ++i) {
            nulls[i] = false;
        }

        values[0] = Int32GetDatum(funcctx->call_cntr + 1);
        values[1] = Int32GetDatum(result_tuples[call_cntr].vehicle_seq);
        values[2] = Int64GetDatum(result_tuples[call_cntr].vehicle_id);
        values[3] = Int32GetDatum(result_tuples[call_cntr].stop_seq);
        values[4] = Int32GetDatum(result_tuples[call_cntr].stop_type + 1);
        values[5] = Int64GetDatum(result_tuples[call_cntr].order_id);
        values[6] = Int64GetDatum(result_tuples[call_cntr].cargo);
        values[7] = Int64GetDatum(result_tuples[call_cntr].travelTime);
        values[8] = Int64GetDatum(result_tuples[call_cntr].arrivalTime);
        values[9] = Int64GetDatum(result_tuples[call_cntr].waitDuration);
        values[10] = Int64GetDatum(result_tuples[call_cntr].serviceDuration);
        values[11] = Int64GetDatum(result_tuples[call_cntr].departureTime);

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        SRF_RETURN_DONE(funcctx);
    }
}
