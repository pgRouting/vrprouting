/*PGR-GNU*****************************************************************
File: vrp_vroomJobsPlain.sql

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
CREATE FUNCTION vrp_vroomJobsPlain(
    TEXT,  -- jobs_sql (required)
    TEXT,  -- jobs_time_windows_sql (required)
    TEXT,  -- vehicles_sql (required)
    TEXT,  -- breaks_sql (required)
    TEXT,  -- breaks_time_windows_sql (required)
    TEXT,  -- matrix_sql (required)

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
$BODY$
    SELECT *
    FROM _vrp_vroom(_pgr_get_statement($1), _pgr_get_statement($2), NULL, NULL,
                    _pgr_get_statement($3), _pgr_get_statement($4),
                    _pgr_get_statement($5), _pgr_get_statement($6), 1::SMALLINT, true);
$BODY$
LANGUAGE SQL VOLATILE;


-- COMMENTS

COMMENT ON FUNCTION vrp_vroomJobsPlain(TEXT, TEXT, TEXT, TEXT, TEXT, TEXT)
IS 'vrp_vroomJobsPlain
 - EXPERIMENTAL
 - Parameters:
   - Jobs SQL with columns:
       id, location_index [, service, delivery, pickup, skills, priority, time_windows]
   - Jobs Time Windows SQL with columns:
       id, tw_open, tw_close
   - Vehicles SQL with columns:
       id, start_index, end_index
       [, service, delivery, pickup, skills, priority, time_window, breaks_sql, steps_sql]
   - Breaks SQL with columns:
       id [, service]
   - Breaks Time Windows SQL with columns:
       id, tw_open, tw_close
   - Matrix SQL with columns:
       start_vid, end_vid, agg_cost
 - Documentation:
   - ${PROJECT_DOC_LINK}/vrp_vroomJobsPlain.html
';
