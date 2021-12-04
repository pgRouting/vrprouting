/*PGR-GNU*****************************************************************
File: _vrp_vroom.sql

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2021 Ashish Kumar
Mail: ashishkr23438@gmail.com

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

-- v0.2
CREATE FUNCTION _vrp_vroom(
    jobs_sql TEXT,
    jobs_time_windows_sql TEXT,
    shipments_sql TEXT,
    shipments_time_windows_sql TEXT,
    vehicles_sql TEXT,
    breaks_sql TEXT,
    breaks_time_windows_sql TEXT,
    matrix_sql TEXT,

    exploration_level SMALLINT,
    timeout INTEGER,

    fn SMALLINT,
    is_plain BOOLEAN,

    OUT seq BIGINT,
    OUT vehicle_seq BIGINT,
    OUT vehicle_id BIGINT,
    OUT step_seq BIGINT,
    OUT step_type INTEGER,
    OUT task_id BIGINT,
    OUT arrival INTEGER,
    OUT travel_time INTEGER,
    OUT service_time INTEGER,
    OUT waiting_time INTEGER,
    OUT load BIGINT[])
RETURNS SETOF RECORD AS
 'MODULE_PATHNAME'
LANGUAGE C VOLATILE;

-- COMMENTS

COMMENT ON FUNCTION _vrp_vroom(TEXT, TEXT, TEXT, TEXT, TEXT, TEXT, TEXT, TEXT, SMALLINT, INTEGER, SMALLINT, BOOLEAN)
IS 'pgRouting internal function';
