/*PGR-GNU*****************************************************************
File: vehiclesAtTime.sql

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

CREATE OR REPLACE FUNCTION _vrp_vehiclesAtTime(
  TEXT,       -- vehicles SQL
  TIMESTAMP,  -- execution time

  dryrun BOOLEAN DEFAULT false,

  OUT id  BIGINT,
  OUT cmd TEXT
)
RETURNS SETOF RECORD AS
$BODY$
DECLARE
main_query TEXT;
BEGIN

  -- select active vehicles at the execution time
  main_query := FORMAT($$
    SELECT a.*, NULL::TEXT
    FROM (%1$s) a
    WHERE s_tw_open <= '%2$s'::TIMESTAMP
    AND e_tw_close >= '%2$s'::TIMESTAMP
    ORDER BY id
    $$, $1, $2);

  IF dryrun THEN
    RETURN QUERY
    SELECT
    NULL::BIGINT, main_query;
    RETURN;
  END IF;

  RETURN QUERY
  EXECUTE format($$
    SELECT id::BIGINT, NULL::TEXT
    FROM (%1$s) a
    $$, main_query);

END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;


COMMENT ON FUNCTION _vrp_vehiclesAtTime(TEXT, TIMESTAMP, BOOLEAN)
IS 'vrp_vehiclesAtTime is a vrprouting internal function';
