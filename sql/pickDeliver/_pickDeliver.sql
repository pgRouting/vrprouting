/*PGR-GNU*****************************************************************
File: _pickDeliver.sql

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

CREATE OR REPLACE FUNCTION _vrp_pickDeliverRaw(
  TEXT, -- orders SQL
  TEXT, -- vehicles SQL
  TEXT, -- matrix SQL
  TEXT, -- multipliers SQL

  BOOLEAN, -- optimize
  FLOAT,   -- factor
  INTEGER, -- max cycles
  BOOLEAN, -- stop on all served
  BIGINT,   -- execution date


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
'MODULE_PATHNAME'
LANGUAGE c VOLATILE STRICT;

CREATE OR REPLACE FUNCTION _vrp_pickDeliver(
  TEXT, -- orders SQL
  TEXT, -- vehicles SQL
  TEXT, -- matrix SQL
  TEXT, -- multipliers SQL

  BOOLEAN, -- optimize
  FLOAT,   -- factor
  INTEGER, -- max cycles
  BOOLEAN, -- stop on all served
  TIMESTAMP,   -- execution date

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
'MODULE_PATHNAME'
LANGUAGE c VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION _vrp_pickDeliverRaw(TEXT, TEXT, TEXT, TEXT, BOOLEAN, FLOAT, INTEGER, BOOLEAN, BIGINT)
IS 'vrprouting internal function';

COMMENT ON FUNCTION _vrp_pickDeliver(TEXT, TEXT, TEXT, TEXT, BOOLEAN, FLOAT, INTEGER, BOOLEAN, TIMESTAMP)
IS 'vrprouting internal function';
