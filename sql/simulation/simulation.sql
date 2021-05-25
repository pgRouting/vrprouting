/*PGR-GNU*****************************************************************
File: simulation.sql

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
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

CREATE OR REPLACE PROCEDURE vrp_simulation(
  TEXT, -- sim_orders_tbl
  TEXT, -- sim_vehicles_tbl
  TEXT, -- sim_travelTime_tbl
  TEXT, -- timeDependant

  factor FLOAT DEFAULT 1.0,
  -- max_cycles
  maxCyclesAdd INTEGER DEFAULT 1.0, -- cycles for optimization when: adding an order
  maxCyclesFull INTEGER DEFAULT 3.0, -- cycles for optimization when: full optimizing orders

  book_date TIMESTAMP DEFAULT (to_timestamp(0) at time zone 'UTC')::TIMESTAMP,
  print INTEGER DEFAULT 2147483646,
  lim INTEGER DEFAULT 2147483646,
  optimize BOOLEAN DEFAULT true,
  fullOptimizeTimes TIME[] DEFAULT ARRAY['23:00:00']::TIME[] -- times when full optimization will be done in a day
)
AS
$BODY$
DECLARE
  order_row RECORD;
  ok_orders BIGINT[];

  not_ok_orders_could_not_insert BIGINT[];
  not_ok_orders_bad_booking_date BIGINT[];
  not_ok_orders_bad_nodes BIGINT[];
  this_orders int[];
  v_id BIGINT;
  vehicle_row RECORD;
  optimize_date TIMESTAMP;

  i INTEGER := 0;

  curr_date TIMESTAMP := NULL;
  is_straddling BOOLEAN := FALSE;
  optimize_time TIME;

  time1 time;
  time2 time;
  deltaTime time;
  sum_optimize time = '00:00:00';
  sum_insertOrder time = '00:00:00';

BEGIN
  DROP TABLE IF EXISTS sim_vehicles_tbl;
  EXECUTE format($$ CREATE TABLE sim_vehicles_tbl AS SELECT * FROM (%1$s) AS v ORDER BY id$$, $2);
  UPDATE sim_vehicles_tbl SET stops = NULL;
  COMMIT;

  -- ordering the shipments by "booking date"
  FOR order_row IN EXECUTE format($$SELECT * FROM (%1$s) AS a WHERE booking_date>= '%2$s'::TIMESTAMP  ORDER BY booking_date$$, $1, book_date)   LOOP
    i := i + 1;
    EXIT WHEN i > lim;

    IF optimize AND maxCyclesFull != 0 THEN
      -- process to simulate cycle of optimization
      book_date = order_row.booking_date ;

      is_straddling := FALSE;
      IF curr_date IS NOT NULL AND array_length(fullOptimizeTimes, 1) > 0 THEN
        FOREACH optimize_time IN ARRAY fullOptimizeTimes
        LOOP
          optimize_date := curr_date::DATE + optimize_time;
          IF optimize_date > curr_date AND optimize_date <= book_date THEN
            is_straddling := TRUE;
            EXIT;
          END IF;
        END LOOP;
      END IF;

      IF is_straddling THEN

        RAISE NOTICE '**********************************************************************';
        RAISE NOTICE 'optimize % times (following vehicles have orders)', maxCyclesFull;
        FOR vehicle_row IN
          SELECT * FROM sim_vehicles_tbl
          WHERE stops IS NOT NULL AND cardinality(stops) != 0
          ORDER BY s_tw_open, id
        LOOP
          RAISE NOTICE 'UPDATE sim_vehicles_tbl SET stops = % WHERE id = %;',
          vehicle_row.stops, vehicle_row.id;
        END LOOP;

        WITH
        orders_q AS (
          SELECT unnest(stops) AS orders_id
          FROM sim_vehicles_tbl
        )
        SELECT array_agg(DISTINCT orders_id)::BIGINT[] || order_row.id ::BIGINT
        FROM orders_q
        INTO ok_orders;


        time1 := clock_timestamp();
        CALL vrp_optimize(
          $1,
          $$SELECT * FROM sim_vehicles_tbl ORDER BY id$$::TEXT,
          $3,
          $4,
          'sim_vehicles_tbl',
          execution_date => optimize_date,
          factor => factor,
          max_cycles => maxCyclesFull);

        time2 := clock_timestamp();
        deltaTime := time2 - time1;
        sum_optimize = sum_optimize + deltaTime::interval;

        RAISE NOTICE '% optimization', optimize_date;
        RAISE NOTICE '========> Optimization results <==================';
        FOR vehicle_row IN
          SELECT * FROM sim_vehicles_tbl
          WHERE stops IS NOT NULL AND cardinality(stops) != 0
          ORDER BY s_tw_open, id
        LOOP
          RAISE NOTICE 'start_time:%  vehicle: %  stops:%',
          vehicle_row.s_tw_open, vehicle_row.id, vehicle_row.stops;
        END LOOP;

        RAISE NOTICE '**********************************************************************';
        curr_date := optimize_date;
      ELSE
        curr_date := book_date;
      END IF;
    END IF;

    RAISE NOTICE ' **** %th: order % booking date % p_tw_open %', i, order_row.id , order_row.booking_date , order_row.p_tw_open;

    time1 := clock_timestamp();

    ---------------------------------------------
    -- process to get the schedule with one additional order and update the vehicles's "stops" column
    ---------------------------------------------
    WITH
    results AS (
      -- adding the order
      SELECT *
      FROM vrp_pickDeliverAdd(
        $1,
        $$SELECT * FROM sim_vehicles_tbl ORDER BY id$$::TEXT,
        $3,
        $4,
        order_row.id,
        order_row.booking_date,
        optimize => optimize,
        factor => factor::FLOAT,
        max_cycles => maxCyclesAdd,
        stop_on_all_served => true)
    ),

    stop_history AS (
      SELECT vehicle_id, array_remove(array_agg(shipment_id), -1::BIGINT) v_stops
      FROM results
      GROUP BY vehicle_id
    ),

    vehicles_in_solution AS (
      SELECT DISTINCT vehicle_id
      FROM results
      WHERE vehicle_id > 0
    ),

    orders_in_vehicles AS (
      SELECT DISTINCT unnest(stops) AS orders
      FROM sim_vehicles_tbl AS v
      JOIN vehicles_in_solution AS s ON (v.id = s.vehicle_id)
    ),

    no_orders_in_phony AS (
      -- The order can be inserted when no orders are in phony vehicles
      SELECT count(*) = 0 AS not_in_phony_ok -- including this leads to reduction of orders with increased cycles
      FROM results
      WHERE shipment_id > 0 AND shipment_id != order_row.id  AND vehicle_id = -1
    ),

    no_orders_in_phony2 AS (
      -- The order can be inserted when no orders are in phony vehicles
      SELECT count(*) = 2 AS not_in_phony_ok2 -- including this leads to reduction of orders with increased cycles
      FROM results
      WHERE shipment_id = order_row.id  AND vehicle_id = -1
    ),

    orders_in_real AS (
      SELECT DISTINCT shipment_id
      FROM results
      WHERE shipment_id > 0 AND vehicle_id != -1
    ),

    no_order_is_lost1 AS (
      SELECT count(*) = (SELECT count(*) FROM orders_in_vehicles) AS no_lost_ok1
      FROM orders_in_real
    ),

    no_order_is_lost2 AS (
      SELECT count(*) = (SELECT count(*) FROM orders_in_vehicles) + 1 AS no_lost_ok2
      FROM orders_in_real
    )

    UPDATE sim_vehicles_tbl AS v
    SET stops = v_stops
    FROM stop_history AS h, no_order_is_lost1, no_order_is_lost2, no_orders_in_phony
    WHERE v.id = h.vehicle_id AND (no_lost_ok1 OR no_lost_ok2) AND not_in_phony_ok;


    time2 := clock_timestamp();
    deltaTime := time2-time1;
    sum_insertOrder = sum_insertOrder + deltaTime::interval;

    -- RAISE NOTICE 'insert order time: %', deltaTime::interval;
    -- check if the order was added
    SELECT id
    FROM sim_vehicles_tbl
    WHERE stops::BIGINT[] @> ARRAY[order_row.id ]::BIGINT[]
    INTO v_id;

    IF v_id IS NOT NULL THEN
      RAISE WARNING 'Order % Inserted on vehicle %', order_row.id , v_id;
    ELSE
      RAISE WARNING 'Order % REJECTED Could not insert', order_row.id ;
      not_ok_orders_could_not_insert := order_row.id ::BIGINT || not_ok_orders_could_not_insert;
      CONTINUE;
    END IF;

    IF mod(i, print) = 0 THEN
      RAISE NOTICE '-------------------------------------';
      FOR vehicle_row IN
        SELECT * FROM sim_vehicles_tbl
        WHERE stops IS NOT NULL AND cardinality(stops) != 0
        ORDER BY s_tw_open, id LOOP
        RAISE NOTICE 'start_time:%  vehcile: %  stops:%',
        vehicle_row.s_tw_open,
        vehicle_row.id,
        vehicle_row.stops;
      END LOOP;
          RAISE NOTICE '-------------------------------------';
    END IF;

  END LOOP;

  SET client_min_messages TO NOTICE;
  RAISE DEBUG 'Sum optimize time: %', sum_optimize;
  RAISE DEBUG 'Sum insert order time: %', sum_insertOrder;
  RAISE NOTICE 'COULD NOT INSERT orders: %', not_ok_orders_could_not_insert;
  RAISE NOTICE 'WRONG REQUESTED TIME: %', not_ok_orders_bad_booking_date;
  RAISE NOTICE 'WRONG NODES: %', not_ok_orders_bad_nodes;

END;
$BODY$
LANGUAGE plpgsql;
/** [vrp_simulation_code] */


COMMENT ON PROCEDURE vrp_simulation(
  TEXT, TEXT, TEXT, TEXT,
  FLOAT,
  INTEGER, INTEGER, TIMESTAMP,
  INTEGER, INTEGER, BOOLEAN, TIME[])
IS 'vrp_simulation
- Documentation:
- ${PROJECT_DOC_LINK}/vrp_simulation.html
';
