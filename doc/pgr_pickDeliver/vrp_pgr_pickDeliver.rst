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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_pgr_pickDeliver.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_pgr_pickDeliver.html>`__)


vrp_pgr_pickDeliver - Experimental
===============================================================================

``vrp_pgr_pickDeliver`` - Pickup and delivery Vehicle Routing Problem

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr

.. rubric:: Availability

Version 0.0.0

* New **experimental** function

  * Ported vrp_pgr_pickDeliver from pgRouting v3.1.3


Synopsis
-------------------------------------------------------------------------------

Problem: Distribute and optimize the pickup-delivery pairs into a fleet of vehicles.

- Optimization problem is NP-hard.
- pickup and Delivery with time windows.
- All vehicles are equal.

  - Same Starting location.
  - Same Ending location which is the same as Starting location.
  - All vehicles travel at the same speed.

- A customer is for doing a pickup or doing a deliver.

  - has an open time.
  - has a closing time.
  - has a service time.
  - has an (x, y) location.

- There is a customer where to deliver a pickup.

  - travel time between customers is distance / speed
  - pickup and delivery pair is done with the same vehicle.
  - A pickup is done before the delivery.


Characteristics
...............................................................................

- All trucks depart at time 0.
- No multiple time windows for a location.
- Less vehicle used is considered better.
- Less total duration is better.
- Less wait time is better.
- the algorithm will raise an exception when

  - If there is a pickup-deliver pair than violates time window
  - The speed, max_cycles, ma_capacity have illegal values

- Six different initial will be optimized
  - the best solution found will be result

.. index::
    single: pickDeliver - Experimental

Signature
-------------------------------------------------------------------------------

.. admonition:: \ \
   :class: signatures

   | pgr_pickDeliver(
   | `Orders SQL`_, `Vehicles SQL`_, `Matrix SQL`_
   | ``[factor, max_cycles, initial_sol]``
   | RETURNS SET OF
   | ``seq, vehicle_number, vehicle_id, stop, order_id, stop_type, cargo,``
   | ``travel_time, arrival_time, wait_time, service_time, departure_time``

Parameters
-------------------------------------------------------------------------------

.. pd_parameters_start

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Column
     - Type
     - Description
   * - `Orders SQL`_
     - ``TEXT``
     - `Orders SQL`_ as described below.
   * - `Vehicles SQL`_
     - ``TEXT``
     - `Vehicles SQL`_ as described below.
   * - `Matrix SQL`_
     - ``TEXT``
     - `Matrix SQL`_ as described below.

.. pd_parameters_end

Optional Parameters
-------------------------------------------------------------------------------

.. pd_optionals_start

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Column
     - Type
     - Default
     - Description
   * - ``factor``
     - ``NUMERIC``
     - 1
     - Travel time multiplier. See :ref:`pd_factor`
   * - ``max_cycles``
     - ``INTEGER``
     - 10
     - Maximum number of cycles to perform on the optimization.
   * - ``initial_sol``
     - ``INTEGER``
     - 4
     - Initial solution to be used.

       - ``1`` One order per truck
       - ``2`` Push front order.
       - ``3`` Push back order.
       - ``4`` Optimize insert.
       - ``5`` Push back order that allows more orders to be inserted at the back
       - ``6`` Push front order that allows more orders to be inserted at the front

.. pd_optionals_end

Inner Queries
-------------------------------------------------------------------------------


Orders SQL
................................................................................

.. include:: concepts.rst
    :start-after: pgr_orders_start
    :end-before: pgr_orders_end

Vehicles SQL
................................................................................

.. include:: concepts.rst
    :start-after: pgr_vehicles_start
    :end-before: pgr_vehicles_end


Matrix SQL
.........................................................................................

.. include:: concepts.rst
    :start-after: pgr_matrix_start
    :end-before: pgr_matrix_end

Result Columns
-------------------------------------------------------------------------------

.. include:: concepts.rst
   :start-after: pd_result_start
   :end-before: pd_result_end

Example
-------------------------------------------------------------------------------

.. TODO put link

This example use the following data:

.. literalinclude:: pgr_pickDeliver.queries
   :start-after: --q2
   :end-before: --q3


See Also
-------------------------------------------------------------------------------

* :doc:`pgr-category`
* The queries use the :doc:`sampledata` network.

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

