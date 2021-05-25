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

CREATE OR REPLACE FUNCTION vrp_pickDeliver(
  TEXT, -- orders SQL
  TEXT, -- vehicles sql
  TEXT, -- Time matrix sql
  TEXT, -- time dependant multipliers SQL

  execution_date TIMESTAMP DEFAULT (to_timestamp(0) at time zone 'UTC')::TIMESTAMP,
  optimize BOOLEAN DEFAULT true,
  factor FLOAT DEFAULT 1,
  max_cycles INTEGER DEFAULT 1,
  stop_on_all_served BOOLEAN DEFAULT true,

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
  OUT cvTot INTEGER,
  OUT twvTot INTEGER
)

RETURNS SETOF RECORD AS
$BODY$
SELECT

  a.seq::INTEGER,
  a.vehicle_seq::INTEGER,
  a.vehicle_id::BIGINT,
  a.stop_seq::INTEGER,
  CASE
    WHEN stop_type = 1 THEN  1
    WHEN stop_type = 2 THEN  2
    WHEN stop_type = 3 THEN  3
    WHEN stop_type = 6 THEN  4
  END::INTEGER,

  a.stop_id::BIGINT,
  a.order_id::BIGINT,
  a.cargo:: INTEGER,

  make_interval(secs => a.travel_fd),
  (to_timestamp(a.arrival_ft) at time zone 'UTC')::TIMESTAMP,

  make_interval(secs => a.wait_fd),
  (to_timestamp(a.schedule_ft) at time zone 'UTC')::TIMESTAMP,

  make_interval(secs => a.service_fd),
  (to_timestamp(a.departure_ft) at time zone 'UTC')::TIMESTAMP,

  a.cvTot,
  a.twvTot

FROM (
  SELECT *,
    lead(b.wait_fd) OVER(ORDER BY b.seq) AS lead_wait
  FROM
    (SELECT * FROM _vrp_pickDeliver(
      $1,
      $2,
      $3,
      $4,
      optimize,
      factor,
      max_cycles,
      stop_on_all_served,
      execution_date)) AS b) AS a;

$BODY$
LANGUAGE SQL VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION vrp_pickDeliver(TEXT, TEXT, TEXT, TEXT, TIMESTAMP, BOOLEAN, FLOAT, INTEGER, BOOLEAN)
IS 'vrp_pickDeliver
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_pickDeliver.html
';
