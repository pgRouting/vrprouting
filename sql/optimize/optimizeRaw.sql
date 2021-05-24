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

CREATE OR REPLACE FUNCTION vrp_optimizeRaw(
  TEXT, -- orders sql (required)
  TEXT, -- vehicles sql (required)
  TEXT, -- time matrix sql (required)
  TEXT, -- time dependencies SQL

  execution_date  BIGINT  DEFAULT 0,
  factor          FLOAT   DEFAULT 1.0,
  max_cycles      INTEGER DEFAULT 1,

  check_triangle_inequality BOOLEAN DEFAULT false,
  subdivision_kind          INTEGER DEFAULT '0',

  OUT seq INTEGER,
  OUT vehicle_id BIGINT,
  OUT order_id BIGINT
)
RETURNS SETOF RECORD AS
$BODY$

  SELECT *
  FROM _vrp_optimize(
    _pgr_get_statement($1),
    _pgr_get_statement($2),
    _pgr_get_statement($3),
    _pgr_get_statement($4),
    factor, max_cycles, execution_date,
    check_triangle_inequality, subdivision_kind,
    false);

$BODY$
LANGUAGE SQL VOLATILE STRICT;


COMMENT ON FUNCTION vrp_optimizeRaw(TEXT, TEXT, TEXT, TEXT, BIGINT, FLOAT, INTEGER, BOOLEAN, INTEGER)
IS 'vrp_optimizeRaw
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_optimizeRaw.html
';
