/*PGR-GNU*****************************************************************
File: viewRoute.sql

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

CREATE OR REPLACE FUNCTION vrp_viewRoute(
  TEXT, -- orders SQL
  TEXT, -- vehicles sql
  TEXT, -- Time matrix sql
  TEXT, -- time dependant multipliers SQL

  BIGINT, -- vehicle id

  factor FLOAT,

  OUT seq           INTEGER,
  OUT vehicle_seq   INTEGER,
  OUT vehicle_id    BIGINT,
  OUT stop_seq      INTEGER,
  OUT stop_type     INTEGER,
  OUT stop_id       BIGINT,
  OUT shipment_id   BIGINT,
  OUT load          INTEGER,
  OUT travel        INTERVAL,
  OUT arrival       TIMESTAMP,
  OUT waiting_time  INTERVAL,
  OUT schedule      TIMESTAMP,
  OUT service       INTERVAL,
  OUT departure     TIMESTAMP,
  OUT cvTot         INTEGER,
  OUT twvTot        INTEGER
)
RETURNS SETOF RECORD AS
$BODY$
DECLARE
main_query TEXT;

BEGIN
  main_query := format($_q_$
    SELECT
    a.*

    FROM (SELECT * FROM vrp_pickDeliver(
        /* Passing only shipments that are in the selected vehicle */
        $__vr1__$
          WITH
            __shipments AS (%1$s),
            __vehicles AS (%2$s)
          SELECT * FROM __shipments
          WHERE id IN (SELECT distinct(unnest(stops)) FROM  __vehicles v WHERE id = %5$s)
        $__vr1__$,
        /* Passing only the vehicle selected vehicle question */
        $__vr2__$
          SELECT * FROM  (%2$s) a
          WHERE id = %5$s
        $__vr2__$,
        $__vr3__$ %3$s $__vr3__$,  -- time matrix
        $__vr4__$ %4$s $__vr4__$,  -- tdm
        optimize => false, factor => %6$s, max_cycles => 0, stop_on_all_served => true)
    ) AS a
    WHERE vehicle_id != 0 AND stop_type != -1;
    $_q_$,
    $1, $2, $3, $4, $5, factor);

  -- RAISE exception '%', main_query;
  RETURN QUERY
  EXECUTE main_query;

END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;
/** [vrp_viewRoute_code] */


COMMENT ON FUNCTION vrp_viewRoute(TEXT, TEXT, TEXT, TEXT, BIGINT, FLOAT)
IS 'vrp_viewRoute
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_viewRoute.html
';
