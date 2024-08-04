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
  `Latest <https://vrp.pgrouting.org/latest/en/concepts.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/concepts.html>`__)

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

Vroom Inner Queries
...............................................................................

Vroom, because of the data types used internally, some maximum values apply.

For ``TIMESTAMP``:

.. literalinclude:: concepts.queries
   :start-after: q1
   :end-before: q2

For ``INTERVAL``:

.. literalinclude:: concepts.queries
   :start-after: q2
   :end-before: q3


Jobs SQL
*******************************************************************************

.. jobs_start

A ``SELECT`` statement that returns the following columns:

| ``id, location_id``
| ``[setup, service, delivery, pickup, skills, priority, data]``

Maximum values apply from vroom

``setup`` and ``service``

- |intervalmax|

``skills``

- :math:`2147483647`

``priority``

- :math:`100`

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the job.
   - - ``location_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the location of the job.
   - - ``setup``
     - |interval|
     - |interval0|
     - The Job setup duration.

   - - ``service``
     - |interval|
     - |interval0|
     - The Job service duration. Max value:
   - - ``pickup``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers describing multidimensional quantities for
       pickup such as number of items, weight, volume etc.

       - All jobs must have the same value of :code:`array_length(pickup, 1)`
   - - ``delivery``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers describing multidimensional quantities for
       delivery such as number of items, weight, volume etc.

       - All jobs must have the same value of :code:`array_length(delivery, 1)`
   - - ``skills``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers defining mandatory skills.
   - - ``priority``
     - ``INTEGER``
     - :math:`0`
     - Value range: :math:`[0, 100]`
   - - ``data``
     - ``JSONB``
     - ``'{}'::JSONB``
     - Any metadata information of the job.

.. jobs_end

Shipments SQL
*******************************************************************************

.. shipments_start

A ``SELECT`` statement that returns the following columns:

| ``id``
| ``p_location_id, [p_setup, p_service, p_data]``
| ``d_location_id, [d_setup, d_service, d_data]``
| ``[amount, skills, priority]``

Maximum values apply from vroom

``p_setup``, ``p_service``, ``d_setup``, ``d_service``

- |intervalmax|

``skills``

- :math:`2147483647`

``priority``

- :math:`100`

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the shipment.
   - - ``p_location_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the pickup location.
   - - ``p_setup``
     - |interval|
     - |interval0|
     - The pickup setup duration
   - - ``p_service``
     - |interval|
     - |interval0|
     - The pickup service duration
   - - ``p_data``
     - ``JSONB``
     - ``'{}'::JSONB``
     - Any metadata information of the pickup.
   - - ``d_location_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the pickup location.
   - - ``d_setup``
     - |interval|
     - |interval0|
     - The pickup setup duration
   - - ``d_service``
     - |interval|
     - |interval0|
     - The pickup service duration
   - - ``d_data``
     - ``JSONB``
     - ``'{}'::JSONB``
     - Any metadata information of the delivery.
   - - ``amount``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers describing multidimensional quantities
       such as number of items, weight, volume etc.

       - All shipments must have the same value of :code:`array_length(amount,
         1)`

   - - ``skills``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers defining mandatory skills.

       - :math:`values \leq 2147483647`
   - - ``priority``
     - ``INTEGER``
     - :math:`0`
     - Value range: :math:`[0, 100]`

.. shipments_end

Vehicles SQL
*******************************************************************************

.. vroom_vehicles_start

A ``SELECT`` statement that returns the following columns:

| ``id, start_id, end_id``
| ``[capacity, skills, tw_open, tw_close, speed_factor, max_tasks, data]``

Maximum values apply from vroom

``skills``

- :math:`2147483647`

``priority``

- :math:`100`

.. list-table::
   :width: 81
   :widths: 14,20,10,37
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the vehicle.
   - - ``start_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the start location.
   - - ``end_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the end location.
   - - ``capacity``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers describing multidimensional quantities
       such as number of items, weight, volume etc.

       - All vehicles must have the same value of :code:`array_length(capacity,
         1)`
   - - ``skills``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers defining mandatory skills.
   - - ``tw_open``
     - |timestamp|
     - |tw_open_default|
     - Time window opening time.

       - :code:`tw_open \leq tw_close`
   - - ``tw_close``
     - |timestamp|
     - |tw_close_default|
     - Time window closing time.

       - :code:`tw_open \leq tw_close`
   - - ``speed_factor``
     - |ANY-NUMERICAL|
     - :math:`1.0`
     - Vehicle travel time multiplier.

       - Max value of speed factor for a vehicle shall not be greater than 5
         times the speed factor of any other vehicle.
   - - ``max_tasks``
     - ``INTEGER``
     - :math:`2147483647`
     - Maximum number of tasks in a route for the vehicle.

       - A job, pickup, or delivery is counted as a single task.
   - - ``data``
     - ``JSONB``
     - ``'{}'::JSONB``
     - Any metadata information of the vehicle.

**Note**:

- At least one of the ``start_id`` or ``end_id`` shall be present.
- If ``end_id`` is omitted, the resulting route will stop at the last visited
  task, whose choice is determined by the optimization process.
- If ``start_id`` is omitted, the resulting route will start at the first
  visited task, whose choice is determined by the optimization process.
- To request a round trip, specify both ``start_id`` and ``end_id`` as the same
  index.
- A vehicle is only allowed to serve a set of tasks if the resulting load at
  each route step is lower than the matching value in capacity for each metric.
  When using multiple components for amounts, it is recommended to put the most
  important/limiting metrics first.
- It is assumed that all delivery-related amounts for jobs are loaded at vehicle
  start, while all pickup-related amounts for jobs are brought back at vehicle
  end.

.. vroom_vehicles_end

Vroom Matrix SQL
*******************************************************************************

.. vroom_matrix_start

A ``SELECT`` statement that returns the following columns:

| ``start_id, end_id, duration``
| ``[ cost]``

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``start_id``
     - |ANY-INTEGER|
     -
     - Identifier of the start node.
   - - ``end_id``
     - |ANY-INTEGER|
     -
     - Identifier of the end node.
   - - ``duration``
     - |interval|
     -
     - Time to travel from ``start_id`` to ``end_id``
   - - ``cost``
     - |interval|
     - ``duration``
     - Cost of travel from ``start_id`` to ``end_id``

.. vroom_matrix_end

Breaks SQL
*******************************************************************************

.. breaks_start

A ``SELECT`` statement that returns the following columns:

| ``id, vehicle_id``
| ``[service, data]``

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the break.  Unique for the same vehicle.
   - - ``vehicle_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the vehicle.
   - - ``service``
     - |interval|
     - |interval0|
     - The break duration
   - - ``data``
     - ``JSONB``
     - ``'{}'::JSONB``
     - Any metadata information of the break.

.. breaks_end


Time Windows SQL
*******************************************************************************

.. rubric:: Shipment Time Windows SQL

.. shipments_time_windows_start

A ``SELECT`` statement that returns the following columns:

| ``id, tw_open, tw_close``
| ``[kind]``

.. list-table::
   :width: 81
   :widths: 14 14 44
   :header-rows: 1

   - - Column
     - Type
     - Description
   - - ``id``
     - |ANY-INTEGER|
     - Positive unique identifier of the: job, pickup/delivery shipment, or
       break.
   - - ``tw_open``
     - |timestamp|
     - Time window opening time.
   - - ``tw_close``
     - |timestamp|
     - Time window closing time.
   - - ``kind``
     - ``CHAR``
     - Value in ['p', 'd'] indicating whether the time window is for:

       - Pickup shipment, or
       - Delivery shipment.

.. shipments_time_windows_end

.. rubric:: General Time Windows SQL

.. general_time_windows_start

A ``SELECT`` statement that returns the following columns:

``id, tw_open, tw_close``

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Description
   - - ``id``
     - |ANY-INTEGER|
     - Positive unique identifier of the: job, pickup/delivery shipment, or
       break.
   - - ``tw_open``
     - |timestamp|
     - Time window opening time.
   - - ``tw_close``
     - |timestamp|
     - Time window closing time.

.. general_time_windows_end

.. time_windows_note_start

**Note**:

- All timings are in **seconds** when represented as an ``INTEGER``.
- Every row must satisfy the condition: :code:`tw_open ≤ tw_close`.
- Time windows can be interpreted by the users:

  - **Relative values**, e.g.

    - Let the beginning of the planning horizon :math:`0`.
    - for a 4 hour time window (:math:`4 * 3600 = 14400` seconds) starting from
      the planning horizon

      - ``tw_open`` = :math:`0`
      - ``tw_close`` = :math:`14400`

    - Times reported in output relative to the start of the planning horizon.

  - **Absolute values**,

    - Let the beginning of the planning horizon ``2019-07-30 08:00:00``
    - for a 4 hour time window starting from the planning horizon

      - ``tw_open`` = ``2019-07-30 08:00:00``
      - ``tw_close`` = ``2019-07-30 12:00:00``

    -  Times reported in output can be interpreted as ``TIMESTAMP``.

.. time_windows_note_end


Return columns & values
--------------------------------------------------------------------------------

VROOM result columns
...............................................................................

.. vroom_result_start

Returns set of

.. code-block:: none

    (seq, vehicle_seq, vehicle_id, vehicle_data, step_seq, step_type, task_id,
     task_data, arrival, travel_time, service_time, waiting_time, load)

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Description
   - - ``seq``
     - ``BIGINT``
     -  Sequential value starting from **1**.
   - - ``vehicle_seq``
     - ``BIGINT``
     - Sequential value starting from **1** for current vehicles.  The
       :math:`n^{th}` vehicle in the solution.
   - - ``vehicle_id``
     - ``BIGINT``
     - Current vehicle identifier.

       - ``-1``: Vehicle denoting all the unallocated tasks.
       - ``0``: Summary row for the complete problem
   - - ``vehicle_data``
     - ``JSONB``
     - Metadata information of the vehicle.
   - - ``step_seq``
     - ``BIGINT``
     - Sequential value starting from **1** for the stops made by the current
       vehicle. The :math:`m^{th}` stop of the current vehicle.

       - ``0``: Summary row
   - - ``step_type``
     - ``BIGINT``
     - Kind of the step location the vehicle is at:

       - ``0``: Summary row
       - ``1``: Starting location
       - ``2``: Job location
       - ``3``: Pickup location
       - ``4``: Delivery location
       - ``5``: Break location
       - ``6``: Ending location

   - - ``task_id``
     - ``BIGINT``
     - Identifier of the task performed at this step.

       - ``0``: Summary row
       - ``-1``: If the step is starting/ending location.
   - - ``location_id``
     - ``BIGINT``
     - Identifier of the task location.

       - ``0``: Summary row
   - - ``task_data``
     - ``JSONB``
     - Metadata information of the task.
   - - ``arrival``
     - |timestamp|
     - Estimated time of arrival at this step.
   - - ``travel_time``
     - |interval|
     - Travel time from previous ``step_seq`` to current ``step_seq``.

       - ``0``: When ``step_type = 1``
   - - ``setup_time``
     - |interval|
     - Setup time at this step.
   - - ``service_time``
     - |interval|
     - Service time at this step.
   - - ``waiting_time``
     - |interval|
     - Waiting time at this step.
   - - ``departure``
     - |timestamp|
     - Estimated time of departure at this step.

       - :math:`arrival + service\_time + waiting\_time`.
   - - ``load``
     - ``BIGINT``
     - Vehicle load after step completion (with capacity constraints)

**Note**:

- Unallocated tasks are mentioned at the end with :code:`vehicle_id = -1`.
- The last step of every vehicle denotes the summary row, where the columns
  ``travel_time``, ``service_time`` and ``waiting_time`` denote the total time
  for the corresponding vehicle,
- The last row denotes the summary for the complete problem, where the columns
  ``travel_time``, ``service_time`` and ``waiting_time`` denote the total time
  for the complete problem,

.. vroom_result_end


Performance
-------------------------------------------------------------------------------

TBD

How to contribute
-------------------------------------------------------------------------------

.. rubric:: Wiki

* Edit an existing  `vrpRouting Wiki <https://github.com/pgRouting/vrprouting/wiki>`_ page.


.. rubric:: Adding Functionaity to vrpRouting

Consult the `developer's documentation <https://vrp.pgrouting.org/doxy/main/index.html>`_


.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

.. |interval| replace:: :abbr:`ANY-INTERVAL(INTERVAL, SMALLINT, INTEGER, BIGINT)`
.. |interval0| replace:: :abbr:`INTERVAL 0('make_interval(secs => 0), 0)`
.. |intervalmax| replace:: **INTERVAL**: ``make_interval(secs => 4294967295)`` and |br| |ANY-INTEGER|: :math:`4294967295`
.. |timestamp| replace:: :abbr:`ANY-TIMESTAMP(TIMESTAMP, SMALLINT, INTEGER, BIGINT)`
.. |tw_open_default| replace:: :abbr:`TW-OPEN-DEFAULT(to_timestamp(0), 0)`
.. |tw_close_default| replace:: :abbr:`TW-CLOSE-DEFAULT(to_timestamp(4294967295), 4294967295)`
