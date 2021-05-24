/*PGR-GNU*****************************************************************
File: pickDeliver.sql

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

CREATE OR REPLACE FUNCTION vrp_pickDeliverRaw(
  TEXT, -- orders SQL
  TEXT, -- vehicles SQL
  TEXT, -- time matrix SQL
  TEXT, -- time dependant multipliers SQL

  execution_date BIGINT DEFAULT 0,
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
  OUT twvTot INTEGER
)

RETURNS SETOF RECORD AS
$BODY$

SELECT * FROM _vrp_pickDeliverRaw(
  _pgr_get_statement($1),
  _pgr_get_statement($2),
  _pgr_get_statement($3),
  _pgr_get_statement($4),
  optimize, factor,
  max_cycles, stop_on_all_served, execution_date);

$BODY$
LANGUAGE SQL
VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION vrp_pickDeliverRaw(TEXT, TEXT, TEXT, TEXT, BIGINT, BOOLEAN, FLOAT, INTEGER, BOOLEAN)
IS 'vrp_pickDeliver
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_pickDeliverRaw.html
';

