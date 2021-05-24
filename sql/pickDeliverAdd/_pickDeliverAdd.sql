/*PGR-GNU*****************************************************************
File: _pickDeliverAdd.sql

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

CREATE OR REPLACE FUNCTION _vrp_pickDeliverAdd(
  TEXT,   -- orders SQL
  TEXT,   -- vehicles SQL
  TEXT,   -- time matrix SQL
  TEXT,   -- time dependant multipliers SQL
  BIGINT, -- order id

  execution_date TIMESTAMP DEFAULT (to_timestamp(0) at time zone 'UTC')::TIMESTAMP,
  optimize BOOLEAN DEFAULT true,
  factor FLOAT DEFAULT 1,
  max_cycles INTEGER DEFAULT 1,
  stop_on_all_served BOOLEAN DEFAULT true,

  OUT seq INTEGER,
  OUT vehicle_seq INTEGER,
  OUT vehicle_id BIGINT,
  OUT stop_seq INTEGER,
  OUT stop_type INTEGER,
  OUT stop_id BIGINT,
  OUT order_id BIGINT,
  OUT cargo BIGINT,
  OUT travel_fd BIGINT,
  OUT arrival_ft BIGINT,
  OUT wait_fd BIGINT,
  OUT schedule_ft BIGINT,
  OUT service_fd BIGINT,
  OUT departure_ft BIGINT,
  OUT cvTot INTEGER,
  OUT twvToT INTEGER
)

RETURNS SETOF RECORD AS
$BODY$
DECLARE
the_msg TEXT;
vehicles_sql TEXT;
orders_sql TEXT;
stops_sql TEXT;
stops_arr BIGINT[];
compatible_vehicles_arr BIGINT[];

BEGIN
  -- get the compatible vehicles of the order
  SELECT array_agg(a.vehicle_id)
  FROM vrp_compatibleVehicles($1, $2, $3, $4, $5, factor, dryrun => false) a
  INTO compatible_vehicles_arr;

  -- Where there compatible vehicles? no -> warning -> exit
  IF compatible_vehicles_arr IS NULL THEN
    RAISE EXCEPTION 'Shipment without compatible vehicles' USING HINT = 'Shipment id: ' || $5;
  END IF;

  -- Get the orders involved in the compatible vehicles + new order
  stops_sql := format($$
    WITH
    the_vehicles AS (%1$s),
    stops_orders AS (
      SELECT DISTINCT unnest(stops) AS orders
      FROM the_vehicles
      JOIN (SELECT unnest('%2$s'::BIGINT[]) AS id) AS compat
      USING (id)
    ),
    new_order AS (
      SELECT %3$s::BIGINT AS orders
    ),
    all_orders AS (
      SELECT orders FROM stops_orders
      UNION
      SELECT orders FROM new_order
    )
    SELECT array_agg(orders)
    FROM all_orders
    $$,
    $2, compatible_vehicles_arr, $5); -- 1,2 3

  EXECUTE stops_sql INTO stops_arr;

  -- RAISE NOTICE 'stops_arr %', stops_arr;
  -- RAISE NOTICE 'stops_sql %', stops_sql;

  -- prepare vehicle's query: only get the compatible vehicles
  vehicles_sql := format($$
    SELECT *
    FROM (%1$s) a JOIN (SELECT unnest('%2$s'::INTEGER[]) AS id) b USING(id)
    $$,
    $2, compatible_vehicles_arr);

  -- prepare order's query: only get the orders that exist on the vehicles + new order
  orders_sql := format($$
    SELECT *
    FROM (%1$s) a JOIN (SELECT unnest('%2$s'::INTEGER[]) AS o_id) b ON (a.id  = b.o_id)
    $$,
    $1, stops_arr);

  -- call main code
  RETURN QUERY
  SELECT * FROM _vrp_pickDeliver(
    orders_sql,
    vehicles_sql,
    $3,
    $4,
    optimize, factor, max_cycles, stop_on_all_served,
    execution_date);

  EXCEPTION
    WHEN OTHERS THEN
      RAISE WARNING 'COULD NOT ADD order %', $5;
      RAISE WARNING 'SQLERRM %', SQLERRM;
      -- RAISE WARNING 'orders_sql %', orders_sql;
      -- RAISE WARNING 'vehicles_sql %', vehicles_sql;
      -- RAISE WARNING 'compatible vehicles %', compatible_vehicles_arr;
END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION _vrp_pickDeliverAdd(TEXT, TEXT, TEXT, TEXT, BIGINT, TIMESTAMP, BOOLEAN, FLOAT, INTEGER, BOOLEAN)
IS '_vrp_pickDeliverAdd is internal function';
