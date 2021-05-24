/*PGR-GNU*****************************************************************
File: _oneDepot.sql

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2021 Celia Virginia Vergara Castillo

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

--------------------
-- _vrp_oneDepot
--------------------


--v0.0
CREATE FUNCTION _vrp_oneDepot(
    TEXT, -- customers_sql
    TEXT, -- vehicles_sql
    TEXT, -- matrix_sql
    INTEGER, -- depot_id

    OUT seq INTEGER,
    OUT vehicle_seq INTEGER,
    OUT vehicle_id BIGINT,
    OUT stop_seq INTEGER,
    OUT stop_type INTEGER,
    OUT stop_id BIGINT,
    OUT order_id BIGINT,
    OUT cargo BIGINT,
    OUT travel_time BIGINT,
    OUT arrival_time BIGINT,
    OUT wait_time BIGINT,
    OUT service_time BIGINT,
    OUT departure_time BIGINT
)
RETURNS SETOF RECORD AS
$BODY$
DECLARE
orders_sql TEXT;
trucks_sql TEXT;
matrix_sql TEXT;
final_sql TEXT;
BEGIN

    orders_sql = $$WITH
    vrp_orders AS ($$ || $1 || $$ ),
    pickups AS (
        SELECT id, x AS p_x, y AS p_y, open_time AS p_open, close_time AS p_close, service_time AS p_service
        FROM vrp_orders
        WHERE id = $$ || $4 || $$
    )
    SELECT vrp_orders.id AS id, order_unit AS demand, pickups.id AS p_node_id, p_x, p_y, p_open, p_close, p_service,
    vrp_orders.id AS d_node_id, x AS d_x, y AS d_y, open_time AS d_open, close_time AS d_close, service_time AS d_service
    FROM vrp_orders, pickups
    WHERE vrp_orders.id != $$ || $4;


    trucks_sql = $$ WITH
    vrp_orders AS ($$ || $1 || $$ ),
    vrp_vehicles AS ($$ || $2 || $$ ),
    starts AS (
        SELECT id AS start_node_id, x AS start_x, y AS start_y, open_time AS start_open, close_time AS start_close, service_time AS start_service
        FROM vrp_orders
        WHERE id = $$ || $4 || $$
    )
    SELECT vehicle_id AS id, capacity, starts.* FROM vrp_vehicles, starts;
    $$;

    final_sql = '
    SELECT * FROM _vrp_pgr_pickDeliver(
            $$' || orders_sql || '$$,
            $$' || trucks_sql || '$$,
            $$' || $3 || '$$,
            max_cycles := 3,
            initial_sol := 7 ); ';

    RAISE DEBUG '%', orders_sql;
    RAISE DEBUG '%', trucks_sql;
    RAISE DEBUG '%', $3;
    RAISE DEBUG '%', final_sql;

    RETURN QUERY EXECUTE final_sql;
END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;


-- COMMENTS


COMMENT ON FUNCTION _vrp_oneDepot(TEXT, TEXT, TEXT, INTEGER)
IS 'pgRouting internal function';
