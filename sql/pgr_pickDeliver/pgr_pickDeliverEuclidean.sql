/*PGR-GNU*****************************************************************
File: pickDeliverEuclidean.sql

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo

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
.. signature start

::

    vrp_pgr_pickDeliverEuclidean(Orders SQL, Vehicles SQL [, factor, max_cycles, initial_sol])
    RETURNS SET OF:
        seq, vehicle_seq, vehicle_id,
        stop_seq, stop_type, order_id,
        cargo, travel_time, arrival_time, wait_time, service_time, departure_time)

.. signature end


.. parameters start

================= ================== ========= =================================================
Column            Type                Default    Description
================= ================== ========= =================================================
**Orders SQL**    ``TEXT``                     `Orders SQL`_ query contianing the orders to be processed.
**Vehicles SQL**  ``TEXT``                     `Vehicles SQL`_ query containing the vehicles to be used.
**factor**        ``NUMERIC``          1       Travel time multiplier. See :ref:`pd_factor`
**max_cycles**    ``INTEGER``          10      Maximum number of cycles to perform on the optimization.
**initial_sol**   ``INTEGER``          4       Initial solution to be used.

                                               - ``1`` One order per truck
                                               - ``2`` Push front order.
                                               - ``3`` Push back order.
                                               - ``4`` Optimize insert.
                                               - ``5`` Push back order that allows more orders to be inserted at the back
                                               - ``6`` Push front order that allows more orders to be inserted at the front
================= ================== ========= =================================================

.. parameters end

.. result start

=================== ============= =================================================
Column              Type           Description
=================== ============= =================================================
**seq**              INTEGER      Sequential value starting from **1**.
**vehicle_seq**      INTEGER      Sequential value starting from **1** for current vehicles. The :math:`n_{th}` vehicle in the solution.
**vehicle_id**       BIGINT       Current vehicle identifier.
**stop_seq**         INTEGER      Sequential value starting from **1** for the stops made by the current vehicle. The :math:`m_{th}` stop of the current vehicle.
**stop_type**        INTEGER      Kind of stop location the vehicle is at:

                                  - ``1``: Starting location
                                  - ``2``: Pickup location
                                  - ``3``: Delivery location
                                  - ``6``: Ending location

**order_id**         BIGINT       Pickup-Delivery order pair identifier.

                                  - ``-1``: When no order is involved on the current stop location.

**cargo**            FLOAT        Cargo units of the vehicle when leaving the stop.

**travel_time**      FLOAT        Travel time from previous ``stop_seq`` to current ``stop_seq``.

                                  - ``0`` When ``stop_type = 1``

**arrival_time**     FLOAT        Previous ``departure_time`` plus current ``travel_time``.
**wait_time**        FLOAT        Time spent waiting for current `location` to open.
**service_time**     FLOAT        Service time at current `location`.
**departure_time**   FLOAT        :math:`arrival\_time + wait\_time + service\_time`.

                                  - When ``stop_type = 6`` has the `total_time` used for the current vehicle.
=================== ============= =================================================

.. result end

*/

--v0.0
CREATE FUNCTION vrp_pgr_pickDeliverEuclidean(
    TEXT, -- orders_sql (required)
    TEXT, -- vehicles_sql (required)

    factor FLOAT DEFAULT 1,
    max_cycles INTEGER DEFAULT 10,
    initial_sol INTEGER DEFAULT 4,

    OUT seq INTEGER,
    OUT vehicle_seq INTEGER,
    OUT vehicle_id BIGINT,
    OUT stop_seq INTEGER,
    OUT stop_type INTEGER,
    OUT order_id BIGINT,
    OUT cargo BIGINT,
    OUT travel_time BIGINT,
    OUT arrival_time BIGINT,
    OUT wait_time BIGINT,
    OUT service_time BIGINT,
    OUT departure_time BIGINT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT *
    FROM _vrp_pgr_pickDeliverEuclidean(_pgr_get_statement($1), _pgr_get_statement($2), $3, $4, $5);
$BODY$
LANGUAGE SQL VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION vrp_pgr_pickDeliverEuclidean(TEXT, TEXT, FLOAT, INTEGER, INTEGER)
IS 'vrp_pgr_pickDeliverEuclidean
 - EXPERIMENTAL
 - Parameters:
   - orders SQL with columns:
     - id, demand, p_x, p_t, d_x, d_y, p_open, p_close, d_open, d_close
     - optional columns:
       - p_service := 0
       - d_service := 0
   - vehicles SQL with columns:
     - id, start_x, start_y, capacity, start_open, start_close
     - optional columns:
       - speed := 1
       - start_service := 0
       - end_x := start_x
       - end_y := start_y
       - end_open := start_open
       - end_close := start_close
       - end_service := 0
 - Optional Parameters:
   - factor: default := 1
   - max_cycles: default := 10
   - initial_sol: default := 4
- Documentation:
   - ${PROJECT_DOC_LINK}/vrp_pgr_pickDeliver.html
';
