/*PGR-GNU*****************************************************************
File: optimize.sql

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

CREATE OR REPLACE PROCEDURE vrp_optimize(
  TEXT, -- orders sql (required)
  TEXT, -- vehicles sql (required)
  TEXT, -- time matrix sql (required)
  TEXT, -- time dependencies SQL
  vehicles_tbl REGCLASS, -- vehicles table to update

  execution_date TIMESTAMP,
  factor         FLOAT,
  max_cycles     INTEGER
)
AS
$BODY$
DECLARE
  curr_time RECORD;
  curr_vehicles BIGINT[];
  prev_vehicles BIGINT[];
  ok_orders BIGINT [];

  orders_sql      TEXT;
  vehicles_sql    TEXT;

BEGIN
  DROP TABLE IF EXISTS __results;


  /** cycle to extract the times where the vehicle opens or ends **/
  FOR curr_time IN EXECUTE format($$
    SELECT s_tw_open AS start_time
      FROM (%1$s) a
      WHERE date_trunc('day', %2$L::TIMESTAMP) <= s_tw_open

      UNION

      SELECT e_tw_close
      FROM  (%1$s) b
      WHERE date_trunc('day', %2$L::TIMESTAMP) <= s_tw_open

      ORDER BY start_time $$,
      $2, execution_date)
  LOOP

    IF (curr_time.start_time < date_trunc('day', execution_date)) THEN
      RAISE EXCEPTION 'NOT OPTIMIZING % Belongs to the past, should never appear because the cycle does not allow to  optimize on the past', curr_time;
      CONTINUE;
    END IF;

    SELECT array_agg(id) FROM _vrp_vehiclesAtTime($2, curr_time.start_time, dryrun => false) INTO curr_vehicles;

    IF prev_vehicles = curr_vehicles THEN
      RAISE DEBUG 'NOT OPTIMIZING  %   vehicles % set of vehicles did not change', curr_time, curr_vehicles;
      CONTINUE;
    END IF;


    SELECT cmd FROM _vrp_vehiclesAtTime($2, curr_time.start_time, dryrun => true) LIMIT 1 INTO vehicles_sql;


    EXECUTE format($$
      WITH
      v AS (%1$s),
      orders_q AS (
          SELECT unnest(stops) AS orders_id FROM v
      )
      SELECT array_agg(DISTINCT orders_id)
      FROM orders_q WHERE orders_id IS NOT NULL
      $$,
      vehicles_sql)
    INTO ok_orders;


    orders_sql = format($$
      SELECT *
      FROM (%1$s) a
      WHERE id ::BIGINT IN (SELECT unnest('%2$s'::BIGINT[]))
      $$,
      $1, ok_orders);

    IF ok_orders IS NULL THEN
      RAISE WARNING 'NOT OPTIMIZING % No orders found %', curr_time, curr_vehicles;
      CONTINUE;
    END IF;


    RAISE NOTICE 'OPTIMIZING time:%   vehicles %', curr_time, curr_vehicles;

    BEGIN
      CREATE TEMP TABLE __results ON COMMIT DROP AS
      SELECT *
      FROM vrp_pickDeliver(
        orders_sql,
        vehicles_sql,
        $3,
        $4,
        execution_date,
        optimize => true,
        factor => factor,
        max_cycles => max_cycles,
        stop_on_all_served => false);

      EXCEPTION WHEN OTHERS THEN
          RAISE WARNING 'COULD NOT OPTIMIZE %', curr_time;
          RAISE WARNING 'SQLERRM %', SQLERRM;
          CONTINUE;
    END;

    EXECUTE format($$
      WITH
      new_stops AS (
          SELECT vehicle_id AS id, array_remove(array_agg(shipment_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
          FROM __results
          GROUP BY vehicle_id
      )
      UPDATE %1$I AS v
        SET stops = n.stops
        FROM new_stops AS n WHERE v.id = n.id
        $$, vehicles_tbl);

    COMMIT;

    prev_vehicles := curr_vehicles;
  END LOOP;

END;
$BODY$
LANGUAGE plpgsql;

COMMENT ON PROCEDURE vrp_optimize(TEXT, TEXT, TEXT, TEXT, REGCLASS, TIMESTAMP, FLOAT, INTEGER)
IS 'vrp_optimize
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_optimize.html
';
