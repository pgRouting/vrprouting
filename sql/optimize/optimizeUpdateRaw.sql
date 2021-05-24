/*PGR-GNU*****************************************************************
File: optimizeUpdateRaw.sql

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

CREATE OR REPLACE PROCEDURE vrp_optimizeUpdateRaw(
  TEXT, -- orders sql (required)
  TEXT, -- vehicles sql (required)
  TEXT, -- time matrix sql (required)
  TEXT, -- time dependencies SQL

  vehicles_tbl    REGCLASS, -- vehicles table to update
  id_column       TEXT    DEFAULT 'id',
  stops_column    TEXT    DEFAULT 'stops',

  execution_date  BIGINT  DEFAULT 0,
  factor          FLOAT   DEFAULT 1.0,
  max_cycles      INTEGER DEFAULT 1,

  check_triangle_inequality BOOLEAN DEFAULT false,
  subdivision_kind          INTEGER DEFAULT '0'
)
AS
$BODY$
DECLARE
  r RECORD;
  old_v BIGINT = 0;
  stops BIGINT[];
BEGIN

  EXECUTE format($$
    WITH
    new_stops AS (
      SELECT vehicle_id, array_agg(order_id ORDER BY seq) AS stops
    FROM _vrp_optimize(
      %1$L, %2$L, %3$L, %4$L,
      %5$s, %6$s, %7$s,
      %8$L, %9$s,
      false)
    GROUP BY vehicle_id
  )
  UPDATE %10$I AS v
  SET %11$I = n.stops
  FROM new_stops AS n WHERE v.%12$I = n.vehicle_id
  $$,
  _pgr_get_statement($1),
  _pgr_get_statement($2),
  _pgr_get_statement($3),
  _pgr_get_statement($4),
  factor, max_cycles, execution_date,
  check_triangle_inequality, subdivision_kind,
  vehicles_tbl, stops_column, id_column);

  COMMIT;


END;
$BODY$
LANGUAGE plpgsql;


COMMENT ON PROCEDURE vrp_optimizeUpdateRaw(TEXT, TEXT, TEXT, TEXT, REGCLASS, TEXT, TEXT, BIGINT, FLOAT, INTEGER, BOOLEAN, INTEGER)
IS 'vrp_optimizeUpdateRaw
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_optimizeUpdateRaw.html
';
