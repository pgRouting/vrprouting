/*PGR-GNU*****************************************************************
File: oneDepot.sql

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

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
-----------------------------------------------------------------------
-- Core function for vrp with sigle depot computation
-- See README for description
-----------------------------------------------------------------------


--------------------
-- vrp_oneDepot
--------------------


--v0.0
CREATE FUNCTION vrp_oneDepot(
    text,  -- order_sql
    text, -- vehicle_sql
    text, -- cost_sql
    integer, -- depot_id

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
    SELECT
      seq, vehicle_seq, vehicle_id, stop_seq, stop_type, stop_id, order_id, cargo, travel_time,
      arrival_time, wait_time, service_time, departure_time
    FROM _vrp_oneDepot($1, $2, $3, $4);
$BODY$
LANGUAGE SQL VOLATILE STRICT
COST 1000
ROWS 1000;


-- COMMENTS


COMMENT ON FUNCTION vrp_oneDepot(TEXT, TEXT, TEXT, INTEGER)
IS 'vrp_OneDepot
- EXPERIMENTAL
- Parameters
  - orders SQL with columns: id, x, y, demand, open_time, close_time, service_time
  - vehicle SQL with columns: vehicle_id, capacity
  - cost SQL with columns: src_id, dest_id, cost, distance, traveltime
  - depot id
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_oneDepot.html
';
