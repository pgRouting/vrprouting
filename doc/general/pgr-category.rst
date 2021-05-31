..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

|

* `Documentation <https://vrp.pgrouting.org/>`__ → `vrpRouting v0 <https://vrp.pgrouting.org/v0>`__
* Supported Versions
  `Latest <https://vrp.pgrouting.org/latest/en/pgr-category.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/pgr-category.html>`__)


Vehicle Routing Functions - Category (Experimental)
===============================================================================

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr


.. contents::

.. rubric:: Functions

.. toctree::
  :maxdepth: 1

  vrp_pgr_pickDeliver
  vrp_pgr_pickDeliverEuclidean
  vrp_oneDepot




Parameters
...............................................................................


Pick & deliver
*******************************************************************************

Both implementations use the following same parameters:

.. pd_parameters_start

================= ================== ========= =================================================
Column            Type                Default    Description
================= ================== ========= =================================================
**orders_sql**    ``TEXT``                     `Pick & Deliver Orders SQL`_ query containing the orders to be processed.
**vehicles_sql**  ``TEXT``                     `Pick & Deliver Vehicles SQL`_ query containing the vehicles to be used.
**factor**        ``NUMERIC``          1       (Optional) Travel time multiplier. See :ref:`pd_factor`
**max_cycles**    ``INTEGER``          10      (Optional) Maximum number of cycles to perform on the optimization.
**initial_sol**   ``INTEGER``          4       (Optional) Initial solution to be used.

                                               - ``1`` One order per truck
                                               - ``2`` Push front order.
                                               - ``3`` Push back order.
                                               - ``4`` Optimize insert.
                                               - ``5`` Push back order that allows more orders to be inserted at the back
                                               - ``6`` Push front order that allows more orders to be inserted at the front
================= ================== ========= =================================================

.. pd_parameters_end

The non euclidean implementation, additionally has:

================= ================== =================================================
Column            Type                Description
================= ================== =================================================
**matrix_sql**    ``TEXT``             `Pick & Deliver Matrix SQL`_ query containing the distance or travel times.
================= ================== =================================================


Inner Queries
*******************************************************************************

- `Pick & Deliver Orders SQL`_
- `Pick & Deliver Vehicles SQL`_
- `Pick & Deliver Matrix SQL`_


Pick & Deliver Orders SQL
*******************************************************************************

In general, the columns for the orders SQL is the same in both implementation of pick and delivery:

.. pd_orders_sql_general_start

================  ===================   =========== ================================================
Column            Type                  Default     Description
================  ===================   =========== ================================================
**id**            |ANY-INTEGER|                     Identifier of the pick-delivery order pair.
**demand**        |ANY-NUMERICAL|                   Number of units in the order
**p_open**        |ANY-NUMERICAL|                   The time, relative to 0, when the pickup location opens.
**p_close**       |ANY-NUMERICAL|                   The time, relative to 0, when the pickup location closes.
**d_service**     |ANY-NUMERICAL|       0           The duration of the loading at the pickup location.
**d_open**        |ANY-NUMERICAL|                   The time, relative to 0, when the delivery location opens.
**d_close**       |ANY-NUMERICAL|                   The time, relative to 0, when the delivery location closes.
**d_service**     |ANY-NUMERICAL|       0           The duration of the loading at the delivery location.
================  ===================   =========== ================================================


.. pd_orders_sql_general_end


.. pd_orders_sql_matrix_start

For the non euclidean implementation, the starting and ending identifiers are needed:

==================  ===================  ================================================
Column              Type                  Description
==================  ===================  ================================================
**p_node_id**       |ANY-INTEGER|          The node identifier of the pickup, must match a node identifier in the matrix table.
**d_node_id**       |ANY-INTEGER|          The node identifier of the delivery, must match a node identifier in the matrix table.
==================  ===================  ================================================

.. pd_orders_sql_matrix_end


.. pd_orders_euclidean_sql_start

For the euclidean implementation, pick up and delivery  :math:`(x,y)` locations are needed:

================  ===================    ================================================
Column            Type                       Description
================  ===================    ================================================
**p_x**           |ANY-NUMERICAL|         :math:`x` value of the pick up location
**p_y**           |ANY-NUMERICAL|         :math:`y` value of the pick up location
**d_x**           |ANY-NUMERICAL|         :math:`x` value of the delivery location
**d_y**           |ANY-NUMERICAL|         :math:`y` value of the delivery location
================  ===================    ================================================


.. pd_orders_euclidean_sql_end




Pick & Deliver Vehicles SQL
*******************************************************************************

In general, the columns for the vehicles_sql is the same in both implementation of pick and delivery:

.. pd_vehicle_sql_general_start

==================  =================== ================ ================================================
Column              Type                  Default           Description
==================  =================== ================ ================================================
**id**              |ANY-INTEGER|                         Identifier of the pick-delivery order pair.
**capacity**        |ANY-NUMERICAL|                       Number of units in the order
**speed**           |ANY-NUMERICAL|      `1`              Average speed of the vehicle.

**start_open**      |ANY-NUMERICAL|                       The time, relative to 0, when the starting location opens.
**start_close**     |ANY-NUMERICAL|                       The time, relative to 0, when the starting location closes.
**start_service**   |ANY-NUMERICAL|      `0`              The duration of the loading at the starting location.

**end_open**        |ANY-NUMERICAL|      `start_open`     The time, relative to 0, when the ending location opens.
**end_close**       |ANY-NUMERICAL|      `start_close`    The time, relative to 0, when the ending location closes.
**end_service**     |ANY-NUMERICAL|      `start_service`  The duration of the loading at the ending location.
==================  =================== ================ ================================================

.. pd_vehicle_sql_general_end

.. pd_vehicle_sql_matrix_start

For the non euclidean implementation, the starting and ending identifiers are needed:

==================  =================== ================ ================================================
Column              Type                  Default           Description
==================  =================== ================ ================================================
**start_node_id**   |ANY-INTEGER|                         The node identifier of the starting location, must match a node identifier in the matrix table.
**end_node_id**     |ANY-INTEGER|        `start_node_id`  The node identifier of the ending location, must match a node identifier in the matrix table.
==================  =================== ================ ================================================

.. pd_vehicle_sql_matrix_end

.. pd_vehicle_sql_euclidean_start

For the euclidean implementation, starting and ending :math:`(x,y)` locations are needed:

==================  =================== ================ ================================================
Column              Type                  Default           Description
==================  =================== ================ ================================================
**start_x**         |ANY-NUMERICAL|                         :math:`x` value of the coordinate of the starting location.
**start_y**         |ANY-NUMERICAL|                         :math:`y` value of the coordinate of the starting location.
**end_x**           |ANY-NUMERICAL|          `start_x`      :math:`x` value of the coordinate of the ending location.
**end_y**           |ANY-NUMERICAL|          `start_y`      :math:`y` value of the coordinate of the ending location.
==================  =================== ================ ================================================

.. pd_vehicle_sql_euclidean_end


Pick & Deliver Matrix SQL
*******************************************************************************

.. TODO

.. warning:: TODO




Results
*******************************************************************************

..
    OUT seq INTEGER,
    OUT vehicle_seq INTEGER,
    OUT vehicle_id BIGINT,
    OUT stop_seq INTEGER,
    OUT order_id BIGINT,
    OUT stop_type INT,
    OUT cargo FLOAT,
    OUT travel_time FLOAT,
    OUT arrival_time FLOAT,
    OUT wait_time FLOAT,
    OUT service_time FLOAT,
    OUT departure_time FLOAT

.. _return_vrp_matrix_start:

Description of the result (TODO Disussion: Euclidean & Matrix)
*******************************************************************************

.. todo:: fix when everything below is fixed


.. code-block:: none

    RETURNS SET OF
        (seq, vehicle_seq, vehicle_id, stop_seq, stop_type,
            travel_time, arrival_time, wait_time, service_time,  departure_time)
        UNION
        (summary row)

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

.. rubric:: Summary Row

.. warning:: TODO: Review the summary

=================== ============= =================================================
Column              Type           Description
=================== ============= =================================================
**seq**              INTEGER      Continues the Sequential value
**vehicle_seq**      INTEGER      ``-2`` to indicate is a summary row
**vehicle_id**       BIGINT       `Total Capacity Violations` in the solution.
**stop_seq**         INTEGER      `Total Time Window Violations` in the solution.
**stop_type**        INTEGER      ``-1``
**order_id**         BIGINT       ``-1``
**cargo**            FLOAT        ``-1``
**travel_time**      FLOAT        `total_travel_time` The sum of all the `travel_time`
**arrival_time**     FLOAT        ``-1``
**wait_time**        FLOAT        `total_waiting_time` The sum of all the `wait_time`
**service_time**     FLOAT        `total_service_time` The sum of all the `service_time`
**departure_time**   FLOAT        `total_solution_time` = :math:`total\_travel\_time + total\_wait\_time + total\_service\_time`.
=================== ============= =================================================


.. return_vrp_matrix_end






.. rubric:: See Also

* https://en.wikipedia.org/wiki/Vehicle_routing_problem
* The queries use the :doc:`sampledata` network.

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

