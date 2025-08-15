..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

|

Concepts
===============================================================================

.. contents::

General documentation
-------------------------------------------------------------------------------

Vehicle Routing Problems `VRP` are **NP-hard** optimization problem, it
generalises the travelling salesman problem (TSP).

- The objective of the VRP is to minimize the total route cost.
- There are several variants of the VRP problem,

**vrpRouting does not try to implement all variants.**


Characteristics
-------------------------------------------------------------------------------

- Capacitated Vehicle Routing Problem `CVRP` where The vehicles have limited
  carrying capacity of the goods.
- Vehicle Routing Problem with Time Windows `VRPTW` where the locations have
  time windows within which the vehicle's visits must be made.
- Vehicle Routing Problem with Pickup and Delivery `VRPPD` where a number of
  goods need to be moved from certain pickup locations to other delivery
  locations.


.. Rubric:: Limitations

- No multiple time windows for a location.
- Less vehicle used is considered better.
- Less total duration is better.
- Less wait time is better.


Pick & Delivery
...............................................................................

Problem: `CVRPPDTW` Capacitated Pick and Delivery Vehicle Routing problem with
Time Windows

- Times are relative to `0`
- The vehicles

  - have start and ending service duration times.
  - have opening and closing times for the start and ending locations.
  - have a capacity.

- The orders

  - Have pick up and delivery locations.
  - Have opening and closing times for the pickup and delivery locations.
  - Have pickup and delivery duration service times.
  - have a demand request for moving goods from the pickup location to the
    delivery location.

- Time based calculations:

  - Travel time between customers is :math:`distance / speed`
  - Pickup and delivery order pair is done by the same vehicle.
  - A pickup is done before the delivery.

.. note To be defined

Getting Started
-------------------------------------------------------------------------------

This is a simple guide to walk you through the steps of getting started
with vrpRouting. In this guide we will cover:

.. contents::
    :local:

Create a routing Database
...............................................................................

The first thing we need to do is create a database and load pgrouting in
the database. Typically you will create a database for each project. Once
you have a database to work in, your can load your data and build your
application in that database. This makes it easy to move your project
later if you want to to say a production server.

For Postgresql 9.2 and later versions

.. code-block:: bash

	createdb mydatabase
	psql mydatabase -c "create extension postgis"
	psql mydatabase -c "create extension pgrouting"


Handling Parameters
*******************************************************************************

To define a problem, several considerations have to be done, to get consistent results.
This section gives an insight of how parameters are to be considered.

- `Capacity and Demand Units Handling`_
- `Locations`_
- `Time Handling`_
- `Factor Handling`_


Capacity and Demand Units Handling
...............................................................................

The `capacity` of a vehicle, can be measured in:

- Volume units like :math:`m^3`.
- Area units like :math:`m^2` (when no stacking is allowed).
- Weight units like :math:`kg`.
- Number of boxes that fit in the vehicle.
- Number of seats in the vehicle

The `demand` request of the pickup-deliver orders must use the same units as the
units used in the vehicle's `capacity`.

To handle problems like:  10 (equal dimension) boxes of apples and 5 kg of
feathers that are to be transported (not packed in boxes).

If the vehicle's `capacity` is measured by `boxes`, a conversion of `kg of
feathers` to `equivalent number of boxes` is needed.
If the vehicle's `capacity` is measured by `kg`, a conversion of `box of apples`
to `equivalent number of kg` is needed.

Showing how the 2 possible conversions can be done

Let:
- :math:`f_boxes`: number of boxes that would be used for `1` kg of feathers.
- :math:`a_weight`: weight of `1` box of apples.

=============== ====================== ==================
Capacity Units  apples                  feathers
=============== ====================== ==================
boxes            10                     :math:`5 * f\_boxes`
kg              :math:`10 * a\_weight`       5
=============== ====================== ==================



Locations
*******************************************************************************

- When using the Euclidean signatures:

  - The vehicles have :math:`(x, y)` pairs for start and ending locations.
  - The orders Have :math:`(x, y)` pairs for pickup and delivery locations.

- When using a matrix:

  - The vehicles have identifiers for the start and ending locations.
  - The orders have identifiers for the pickup and delivery locations.
  - All the identifiers are indices to the given matrix.


Time Handling
...............................................................................

The times are relative to 0

Suppose that a vehicle's driver starts the shift at 9:00 am and ends the shift at 4:30 pm
and the service time duration is 10 minutes with 30 seconds.

All time units have to be converted

============ ================= ==================== ===================== =========
Meaning of 0   time units       9:00 am              4:30 pm               10 min 30 secs
============ ================= ==================== ===================== =========
0:00 am         hours            9                  16.5                   :math:`10.5 / 60  = 0.175`
9:00 am         hours            0                  7.5                    :math:`10.5 / 60  = 0.175`
0:00 am         minutes          :math:`9*60 = 54`  :math:`16.5*60 = 990`  10.5
9:00 am         minutes          0                  :math:`7.5*60 = 540`   10.5
============ ================= ==================== ===================== =========


.. _pd_factor:

Factor Handling
*******************************************************************************

.. TODO

TBD


Group of Functions
-------------------------------------------------------------------------------

TBD



Inner Queries
-------------------------------------------------------------------------------

Pickup-Delivery Inner Queries
...............................................................................

Matrix SQL
*******************************************************************************

.. pgr_matrix_start

A ``SELECT`` statement that returns the following columns:

``start_vid, end_vid, agg_cost``

.. list-table::
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Description
   - - ``start_vid``
     - |ANY-INTEGER|
     - Identifier of a node.
   - - ``end_vid``
     - |ANY-INTEGER|
     - Identifier of a node.
   - - ``agg_cost``
     - |ANY-NUMERICAL|
     - Cost to travel from ``start_vid`` to ``end_vid``

.. pgr_matrix_end

Orders SQL
*******************************************************************************

.. rubric:: Orders

.. pgr_orders_start

A ``SELECT`` statement that returns the following columns:

| ``id, amount``
| ``p_id, p_tw_open, p_tw_close, [p_service]``
| ``d_id, d_tw_open, d_tw_close, [d_service]``

.. list-table::
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Identifier of the pick-delivery order pair.
   - - ``amount``
     - |ANY-NUMERICAL|
     -
     - Number of units in the order.
   - - ``p_id``
     - |ANY-INTEGER|
     -
     - Identifier of the pickup node.

       - Must match a node identifier in the `Matrix SQL`_.
   - - ``p_tw_open``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the pickup location opens.
   - - ``p_tw_close``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the pickup location closes.

       - :math:`p\_tw\_open < p\_tw\_close < 9223372036854775807`
   - - ``p_service``
     - |ANY-INTEGER|
     - 0
     - The duration of the loading at the pickup location.
   - - ``d_id``
     - |ANY-INTEGER|
     -
     - Identifier of the delivery node.

       - Must match a node identifier in the `Matrix SQL`_.
   - - ``d_tw_open``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the delivery location opens.
   - - ``d_tw_close``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the delivery location closes.

       - :math:`d\_tw\_open < d\_tw\_close <= 9223372036854775807`
   - - ``d_service``
     - |ANY-INTEGER|
     - 0
     - The duration of the unloading at the delivery location.

.. pgr_orders_end

.. rubric:: Euclidean Orders

.. pgr_orders_e_start

A ``SELECT`` statement that returns the following columns:

| ``id, amount``
| ``p_x, p_y, p_tw_open, p_tw_close, [p_service,]``
| ``d_x, d_y, d_tw_open, d_tw_close, [d_service]``


.. list-table::
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Identifier of the pick-delivery order pair.
   - - ``amount``
     - |ANY-NUMERICAL|
     -
     - Number of units in the order.
   - - ``p_x``
     - |ANY-NUMERICAL|
     -
     - :math:`x` value of the pickup location.
   - - ``p_y``
     - |ANY-NUMERICAL|
     -
     - :math:`y` value of the pickup location.
   - - ``p_tw_open``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the pickup location opens.
   - - ``p_tw_close``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the pickup location closes.

       - :math:`p\_tw\_open < p\_tw\_close < 9223372036854775807`
   - - ``p_service``
     - |ANY-INTEGER|
     - 0
     - The duration of the loading at the pickup location.
   - - ``d_x``
     - |ANY-NUMERICAL|
     -
     - :math:`x` value of the delivery location.
   - - ``d_y``
     - |ANY-NUMERICAL|
     -
     - :math:`y` value of the delivery location.
   - - ``d_tw_open``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the delivery location opens.
   - - ``d_tw_close``
     - |ANY-INTEGER|
     -
     - The time, relative to 0, when the delivery location closes.

       - :math:`d\_tw\_open < d\_tw\_close <= 9223372036854775807`
   - - ``d_service``
     - |ANY-INTEGER|
     - 0
     - The duration of the unloading at the delivery location.

.. pgr_orders_e_end

Vehicles SQL
*******************************************************************************

.. rubric:: Vehicles

.. pgr_vehicles_start

A ``SELECT`` statement that returns the following columns:

| ``id, capacity, [speed,]``
| ``s_id, [s_tw_open, s_tw_close, s_service,]``
| ``[e_id, e_tw_open, e_tw_close, e_service]``

.. list-table::
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Identifier of the vehicle
   - - ``capacity``
     - |ANY-INTEGER|
     -
     - Capacity of the vehicle.

       - :math:`0 < capacity <= 4294967295`.
   - - ``speed``
     - |ANY-NUMERICAL|
     - 1
     - Speed of the vehicle.
   - - ``s_id``
     - |ANY-INTEGER|
     -
     - The node identifier of the starting location.

       - Must match a node identifier in the `Matrix SQL`_.
   - - ``s_tw_open``
     - |ANY-INTEGER|
     - 0
     - The time, relative to 0, when the starting location opens.
   - - ``s_tw_close``
     - |ANY-INTEGER|
     - |MAX-BIGINT|
     - The time, relative to 0, when the starting location closes.

       - :math:`s\_tw\_open < s\_tw\_close <= 9223372036854775807`
   - - ``s_service``
     - |ANY-INTEGER|
     - 0
     - Duration of any task at the starting location,
   - - ``e_id``
     - |ANY-INTEGER|
     - ``s_id``
     - The node identifier of the ending location.

       - Must match a node identifier in the `Matrix SQL`_.
   - - ``e_tw_open``
     - |ANY-INTEGER|
     - ``s_tw_open``
     - The time, relative to 0, when the ending location opens.
   - - ``e_tw_close``
     - |ANY-INTEGER|
     - ``s_tw_close``
     - The time, relative to 0, when the ending location closes.

       - :math:`e\_tw\_open < e\_tw\_close <= 9223372036854775807`
   - - ``e_service``
     - |ANY-INTEGER|
     - ``s_service``
     - The duration of any task at the ending location

.. pgr_vehicles_end

.. rubric:: Euclidean Vehicles

.. pgr_vehicles_e_start

A ``SELECT`` statement that returns the following columns:

| ``id, capacity, [speed,]``
| ``s_x, s_y, [s_tw_open, s_tw_close, s_service]``
| ``[e_x, e_y, e_tw_open, e_tw_close, e_service]``

.. list-table::
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Identifier of the vehicle.
   - - ``capacity``
     - |ANY-INTEGER|
     -
     - Capacity of the vehicle. :math:`0 < capacity <= 4294967295`.
   - - ``speed``
     - |ANY-NUMERICAL|
     - 1
     - Speed of the vehicle.
   - - ``s_x``
     - |ANY-NUMERICAL|
     -
     - :math:`x` value of the coordinate of the starting location.
   - - ``s_y``
     - |ANY-NUMERICAL|
     -
     - :math:`y` value of the coordinate of the starting location.
   - - ``s_tw_open``
     - |ANY-INTEGER|
     - 0
     - The time, relative to 0, when the starting location opens.
   - - ``s_tw_close``
     - |ANY-INTEGER|
     - |MAX-BIGINT|
     - The time, relative to 0, when the starting location closes.

       - :math:`s\_tw\_open < s\_tw\_close <= 9223372036854775807`
   - - ``s_service``
     - |ANY-INTEGER|
     - 0
     - Duration of any task at the ending location,
   - - ``e_x``
     - |ANY-NUMERICAL|
     -
     - :math:`x` value of the coordinate of the ending location.
   - - ``e_y``
     - |ANY-NUMERICAL|
     -
     - :math:`y` value of the coordinate of the ending location.
   - - ``e_tw_open``
     - |ANY-INTEGER|
     - ``s_tw_open``
     - The time, relative to 0, when the ending location opens.
   - - ``e_tw_close``
     - |ANY-INTEGER|
     - ``s_tw_close``
     - The time, relative to 0, when the ending location closes.

       - :math:`e\_tw\_open < e\_tw\_close <= 9223372036854775807`
   - - ``e_service``
     - |ANY-INTEGER|
     - ``s_service``
     - The duration of any task at the ending location

.. pgr_vehicles_e_end


Return columns & values
--------------------------------------------------------------------------------

Pick-Deliver result columns
...............................................................................

.. rubric:: Results

.. pd_result_start

Returns set of

| ``seq, vehicle_number, vehicle_id, stop_seq, stop_type, stop_id, order_id, cargo,``
| ``travel_time, arrival_time, wait_time, service_time, departure_time``

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Column
     - Type
     - Description
   * - ``seq``
     - ``INTEGER``
     -   Sequential value starting from **1**.
   * - ``vehicle_seq``
     - ``INTEGER``
     - Sequential value starting from **1** for current vehicles. The
       :math:`n_{th}` vehicle in the solution.
   * - ``vehicle_id``
     - ``BIGINT``
     - Current vehicle identifier.
   * - ``stop_seq``
     - ``INTEGER``
     - Sequential value starting from **1** for the stops made by the current
       vehicle. The :math:`m_{th}` stop of the current vehicle.
   * - ``stop_type``
     - ``INTEGER``
     - Kind of stop location the vehicle is at:

       - ``1``: Starting location
       - ``2``: Pickup location
       - ``3``: Delivery location
       - ``6``: Ending location
   * - ``stop_id``
     - ``BIGINT``
     - Identifier of the stop.
   * - ``order_id``
     - ``BIGINT``
     - Pickup-Delivery order pair identifier.

       - ``-1``: When no order is involved on the current stop location.
   * - ``cargo``
     - ``BIGINT``
     -   Cargo units of the vehicle when leaving the stop.
   * - ``travel_time``
     - ``BIGINT``
     - Travel time from previous ``stop_seq`` to current ``stop_seq``.

       - ``0`` When ``stop_type = 1``
   * - ``arrival_time``
     - ``BIGINT``
     -   Previous ``departure_time`` plus current ``travel_time``.
   * - ``wait_time``
     - ``BIGINT``
     - Time spent waiting for current `location` to open.
   * - ``service_time``
     - ``BIGINT``
     - Service time at current `location`.
   * - ``departure_time``
     - ``BIGINT``
     - :math:`arrival\_time + wait\_time + service\_time`.

       - When ``stop_type = 6`` has the `total_time` used for the current vehicle.

.. pd_result_end

.. rubric:: Euclidean Results

.. pde_result_start

Returns set of

| ``seq, vehicle_seq, vehicle_id, stop_seq, stop_type, order_id, cargo,``
| ``travel_time, arrival_time, wait_time, service_time, departure_time``

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Column
     - Type
     - Description
   * - ``seq``
     - ``INTEGER``
     - Sequential value starting from **1**.
   * - ``vehicle_seq``
     - ``INTEGER``
     - Sequential value starting from **1** for current vehicles. The
       :math:`n_{th}` vehicle in the solution.
   * - ``vehicle_id``
     - ``BIGINT``
     - Current vehicle identifier.
   * - ``stop_seq``
     - ``INTEGER``
     - Sequential value starting from **1** for the stops made by the current
       vehicle. The :math:`m_{th}` stop of the current vehicle.
   * - ``stop_type``
     - ``INTEGER``
     - Kind of stop location the vehicle is at:

       - ``1``: Starting location
       - ``2``: Pickup location
       - ``3``: Delivery location
       - ``6``: Ending location
   * - ``order_id``
     - ``BIGINT``
     - Pickup-Delivery order pair identifier.

       - ``-1``: When no order is involved on the current stop location.
   * - ``cargo``
     - ``BIGINT``
     -   Cargo units of the vehicle when leaving the stop.
   * - ``travel_time``
     - ``BIGINT``
     - Travel time from previous ``stop_seq`` to current ``stop_seq``.

       - ``0`` When ``stop_type = 1``
   * - ``arrival_time``
     - ``BIGINT``
     -   Previous ``departure_time`` plus current ``travel_time``.
   * - ``wait_time``
     - ``BIGINT``
     - Time spent waiting for current `location` to open.
   * - ``service_time``
     - ``BIGINT``
     - Service time at current `location`.
   * - ``departure_time``
     - ``BIGINT``
     - :math:`arrival\_time + wait\_time + service\_time`.

       - When ``stop_type = 6`` has the `total_time` used for the current vehicle.

.. pde_result_end

.. rubric:: Adding Functionaity to vrpRouting

Consult the `developer's documentation <https://vrp.pgrouting.org/doxy/main/index.html>`_

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


Inner Queries 1
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

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
