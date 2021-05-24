..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

|

* `Documentation <https://vrp.pgrouting.org/>`__ → `vrpRouting 0 <https://vrp.pgrouting.org/0>`__
* Supported Versions
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_pgr_pickDeliver.html>`__
  (`0.0 <https://vrp.pgrouting.org/0/en/vrp_pgr_pickDeliver.html>`__)


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

.. include:: ../sql/pgr_pickDeliver/pgr_pickDeliver.sql
   :start-after: signature start
   :end-before: signature end


Parameters
-------------------------------------------------------------------------------

.. include:: ../sql/pgr_pickDeliver/pgr_pickDeliver.sql
   :start-after: parameters start
   :end-before: parameters end


Inner Queries
-------------------------------------------------------------------------------


Orders SQL
................................................................................

.. include:: ../../src/common/orders_input.c
    :start-after: pgr_pickDeliver start
    :end-before: pgr_pickDeliver end

Vehicles SQL
................................................................................

.. include:: ../../src/common/vehicles_input.c
    :start-after: pgr_pickDeliver start
    :end-before: pgr_pickDeliver end


Time Matrix SQL
.........................................................................................

.. include:: ../../src/common/matrixRows_input.c
    :start-after: pgr_pickDeliver start
    :end-before: pgr_pickDeliver end

Result Columns
-------------------------------------------------------------------------------

.. include:: ../sql/pgr_pickDeliver/pgr_pickDeliver.sql
   :start-after: result start
   :end-before: result end

Example
-------------------------------------------------------------------------------

.. TODO put link

This example use the following data:

.. literalinclude:: doc-pgr_pickDeliver.queries
   :start-after: --q2
   :end-before: --q3


See Also
-------------------------------------------------------------------------------

* :doc:`pgr-category`
* The queries use the :doc:`sampledata` network.

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

