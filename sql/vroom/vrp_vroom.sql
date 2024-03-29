/*PGR-GNU*****************************************************************
File: vrp_vroom.sql

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

/*
signature start

.. code-block:: none

    vrp_vroom(
      Jobs SQL, Jobs Time Windows SQL,
      Shipments SQL, Shipments Time Windows SQL,
      Vehicles SQL, Breaks SQL, Breaks Time Windows SQL,
      Matrix SQL [, exploration_level] [, timeout])  -- Experimental on v0.2

    RETURNS SET OF
    (seq, vehicle_seq, vehicle_id, vehicle_data, step_seq, step_type, task_id,
     task_data, arrival, travel_time, service_time, waiting_time, departure, load)

signature end

default signature start

.. code-block:: none

    vrp_vroom(
      Jobs SQL, Jobs Time Windows SQL,
      Shipments SQL, Shipments Time Windows SQL,
      Vehicles SQL, Breaks SQL, Breaks Time Windows SQL,
      Matrix SQL)

    RETURNS SET OF
    (seq, vehicle_seq, vehicle_id, vehicle_data, step_seq, step_type, task_id,
     task_data, arrival, travel_time, service_time, waiting_time, departure, load)

default signature end

parameters start

============================== =========== =========================================================
Parameter                      Type        Description
============================== =========== =========================================================
**Jobs SQL**                   ``TEXT``    `Jobs SQL`_ query describing the single-location
                                           pickup and/or delivery tasks.
**Jobs Time Windows SQL**      ``TEXT``    `Time Windows SQL`_ query describing valid slots
                                           for job service start.
**Shipments SQL**              ``TEXT``    `Shipments SQL`_ query describing pickup and delivery
                                           tasks that should happen within same route.
**Shipments Time Windows SQL** ``TEXT``    `Time Windows SQL`_ query describing valid slots
                                           for pickup and delivery service start.
**Vehicles SQL**               ``TEXT``    `Vehicles SQL`_ query describing the available vehicles.
**Breaks SQL**                 ``TEXT``    `Breaks SQL`_ query describing the driver breaks.
**Breaks Time Windows SQL**    ``TEXT``    `Time Windows SQL`_ query describing valid slots for
                                           break start.
**Matrix SQL**                 ``TEXT``    `Time Matrix SQL`_ query containing the distance or
                                           travel times between the locations.
============================== =========== =========================================================

parameters end

optional parameters start

===================== ============ ============================= =================================================
Parameter             Type         Default                       Description
===================== ============ ============================= =================================================
**exploration_level** ``INTEGER``  :math:`5`                     Exploration level to use while solving.

                                                                 - Ranges from ``[0, 5]``
                                                                 - A smaller exploration level gives faster result.

**timeout**           ``INTERVAL`` '-00:00:01'::INTERVAL         Timeout value to stop the solving process.

                                                                 - Gives the best possible solution within a time
                                                                   limit. Note that some additional seconds may be
                                                                   required to return back the data.
                                                                 - Any negative timeout value is ignored.
===================== ============ ============================= =================================================

optional parameters end

result start

Returns set of

.. code-block:: none

    (seq, vehicle_seq, vehicle_id, vehicle_data, step_seq, step_type, task_id,
     task_data, arrival, travel_time, service_time, waiting_time, load)

=================== ================= =================================================
Column              Type              Description
=================== ================= =================================================
**seq**              ``BIGINT``       Sequential value starting from **1**.

**vehicle_seq**      ``BIGINT``       Sequential value starting from **1** for current vehicles.
                                      The :math:`n^{th}` vehicle in the solution.

                                      - ``0``: Summary row for the complete problem

**vehicle_id**       ``BIGINT``       Current vehicle identifier.

                                      - ``-1``: Vehicle denoting all the unallocated tasks.
                                      - ``0``: Summary row for the complete problem

**vehicle_data**     ``JSONB``        Metadata information of the vehicle

**step_seq**         ``BIGINT``       Sequential value starting from **1** for the stops
                                      made by the current vehicle. The :math:`m^{th}` stop
                                      of the current vehicle.

                                      - ``0``: Summary row

**step_type**        ``INTEGER``      Kind of the step location the vehicle is at:

                                      - ``0``: Summary row
                                      - ``1``: Starting location
                                      - ``2``: Job location
                                      - ``3``: Pickup location
                                      - ``4``: Delivery location
                                      - ``5``: Break location
                                      - ``6``: Ending location

**task_id**          ``BIGINT``       Identifier of the task performed at this step.

                                      - ``0``: Summary row
                                      - ``-1``: If the step is starting/ending location.

**location_id**      ``BIGINT``       Identifier of the task location.

                                      - ``0``: Summary row

**task_data**        ``JSONB``        Metadata information of the task

**arrival**          |timestamp|      Estimated time of arrival at this step.

**travel_time**      |interval|       Travel time from previous ``step_seq`` to current ``step_seq``.

                                      - ``0``: When ``step_type = 1``

**setup_time**       |interval|       Setup time at this step.

**service_time**     |interval|       Service time at this step.

**waiting_time**     |interval|       Waiting time upon arrival at this step.

**departure**        |timestamp|      Estimated time of arrival at this step.

                                      - :math:`arrival + service\_time + waiting\_time`.

**load**             ``BIGINT``       Vehicle load after step completion (with capacity constraints)
=================== ================= =================================================

**Note**:

- Unallocated tasks are mentioned at the end with :code:`vehicle_id = -1`.
- The last step of every vehicle denotes the summary row, where the columns ``travel_time``, ``service_time`` and ``waiting_time`` denote the total time for the corresponding vehicle,
- The last row denotes the summary for the complete problem, where the columns ``travel_time``, ``service_time`` and ``waiting_time`` denote the total time for the complete problem,

result end
*/

-- v0.2
CREATE FUNCTION vrp_vroom(
    TEXT,  -- jobs_sql (required)
    TEXT,  -- jobs_time_windows_sql (required)
    TEXT,  -- shipments_sql (required)
    TEXT,  -- shipments_time_windows_sql (required)
    TEXT,  -- vehicles_sql (required)
    TEXT,  -- breaks_sql (required)
    TEXT,  -- breaks_time_windows_sql (required)
    TEXT,  -- matrix_sql (required)

    exploration_level INTEGER DEFAULT 5,
    timeout INTERVAL DEFAULT '-00:00:01'::INTERVAL,

    OUT seq BIGINT,
    OUT vehicle_seq BIGINT,
    OUT vehicle_id BIGINT,
    OUT vehicle_data JSONB,
    OUT step_seq BIGINT,
    OUT step_type INTEGER,
    OUT task_id BIGINT,
    OUT location_id BIGINT,
    OUT task_data JSONB,
    OUT arrival TIMESTAMP,
    OUT travel_time INTERVAL,
    OUT setup_time INTERVAL,
    OUT service_time INTERVAL,
    OUT waiting_time INTERVAL,
    OUT departure TIMESTAMP,
    OUT load BIGINT[])
RETURNS SETOF RECORD AS
$BODY$
BEGIN
    IF exploration_level < 0 OR exploration_level > 5 THEN
        RAISE EXCEPTION 'Invalid value found on ''exploration_level'''
        USING HINT = format('Value found: %s. It must lie in the range 0 to 5 (inclusive)', exploration_level);
    END IF;

    RETURN QUERY
    SELECT
      A.seq,
      A.vehicle_seq,
      A.vehicle_id,
      A.vehicle_data::JSONB,
      A.step_seq,
      A.step_type,
      A.task_id,
      A.location_id,
      A.task_data::JSONB,
      (to_timestamp(A.arrival) at time zone 'UTC')::TIMESTAMP,
      make_interval(secs => A.travel_time),
      make_interval(secs => A.setup_time),
      make_interval(secs => A.service_time),
      make_interval(secs => A.waiting_time),
      (to_timestamp(A.departure) at time zone 'UTC')::TIMESTAMP,
      A.load
    FROM _vrp_vroom(_pgr_get_statement($1), _pgr_get_statement($2), _pgr_get_statement($3),
                    _pgr_get_statement($4), _pgr_get_statement($5), _pgr_get_statement($6),
                    _pgr_get_statement($7), _pgr_get_statement($8), exploration_level,
                    EXTRACT(epoch FROM timeout)::INTEGER, 0::SMALLINT, false) A;
END;
$BODY$
LANGUAGE plpgsql VOLATILE;


-- COMMENTS

COMMENT ON FUNCTION vrp_vroom(TEXT, TEXT, TEXT, TEXT, TEXT, TEXT, TEXT, TEXT, INTEGER, INTERVAL)
IS 'vrp_vroom
 - EXPERIMENTAL
 - Parameters:
   - Jobs SQL with columns:
       id, location_id [, service, delivery, pickup, skills, priority, time_windows]
   - Jobs Time Windows SQL with columns:
       id, tw_open, tw_close
   - Shipments SQL with columns:
       p_id, p_location_id [, p_service, p_time_windows],
       d_id, d_location_id [, d_service, d_time_windows] [, amount, skills, priority]
   - Shipments Time Windows SQL with columns:
       id, kind, tw_open, tw_close
   - Vehicles SQL with columns:
       id, start_id, end_id
       [, service, delivery, pickup, skills, priority, time_window, breaks_sql, steps_sql]
   - Breaks SQL with columns:
       id [, service]
   - Breaks Time Windows SQL with columns:
       id, tw_open, tw_close
   - Matrix SQL with columns:
       start_vid, end_vid, agg_cost
- Optional parameters
   - exploration_level := 5
   - timeout := ''-00:00:01''::INTERVAL
 - Documentation:
   - ${PROJECT_DOC_LINK}/vrp_vroom.html
';
