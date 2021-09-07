/*PGR-GNU*****************************************************************
File: compatibleVehiclesRaw.sql

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

CREATE OR REPLACE FUNCTION vrp_compatibleVehiclesRaw(
  TEXT, -- orders sql (required)
  TEXT, -- vehicles sql (required)
  TEXT, -- time matrix (required)
  TEXT, -- time dependant multipliers (required)

  BIGINT, -- order_id

  factor FLOAT DEFAULT 1,
  dryrun BOOLEAN DEFAULT false,

  OUT order_id BIGINT,
  OUT vehicle_id BIGINT,
  OUT cmd TEXT
)
RETURNS SETOF RECORD AS
$BODY$
DECLARE
  zero TIMESTAMP;
  the_msg TEXT;
  orders_sql TEXT;
  vehicles_sql TEXT;
  m BIGINT;

BEGIN

  orders_sql = format(
    $$SELECT * FROM (%1$s) AS __a__ WHERE id  = %2$s LIMIT 1$$,
    -- shipments SQL, shipment_id
    $1, $5
  );

  -- has the order? no -> exit
  EXECUTE format($$SELECT count(*) FROM (%1$s) AS __b__$$, orders_sql) INTO m;
  IF m = 0 THEN
      RAISE EXCEPTION 'Order % was not found', $4;
  END IF;

  -- has vehicles? no -> exit
  EXECUTE format($$SELECT count(*) FROM (%1$s) AS a$$, $2) INTO m;
  IF m = 0 THEN
      RAISE EXCEPTION 'No vehicles found';
  END IF;

  the_msg := format(
      $$
      SELECT *, NULL::TEXT FROM _vrp_compatibleVehicles(
        %1$L,
        %2$L,
        %3$L,
        %4$L,
        %5$s::FLOAT,
        false)
      $$,
      /* shipments, vehicles, matrix, tdm */
      orders_sql, $2, $3, $4,
      factor);


  IF dryrun THEN
  RETURN QUERY
      SELECT NULL::BIGINT, NULL::BIGINT, the_msg;
      RETURN;
  END IF;

  RETURN QUERY
  EXECUTE the_msg;

END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION vrp_compatibleVehiclesRaw(TEXT, TEXT, TEXT, TEXT, BIGINT, FLOAT, BOOLEAN)
IS 'vrp_compatibleVehiclesRaw
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_compatibleVehiclesRaw.html
';
